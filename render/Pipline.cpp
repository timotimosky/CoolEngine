#include "Pipline.h"
#include "Camera.h"
#include "UObject.h"


camera cameras[MAX_NUM_CAMERA];
camera camera_main;

//位运算：
//  a != b----->a = a | b, a 或者 b 只要有一个为 1, 那么，a 的最终结果就为 1
//  a &= b----->a = a & b, a 和 b 二者必须都为 1, 那么，a 的最终结果才为 1
//异或  a ^= b----->a = a ^ b, 当且仅当 a 和 b 的值不一致时，a 的最终结果才为1，否则为0

// 检查齐次坐标同 cvv 的边界用于视锥裁剪

//这里的w 是用Z来当做深度计算 目前取的是 1/Z 非线性的缩放 当Z无限大的时候 W=0，此刻这个点无意义。表明视野中无限远的一个点
int transform_check_cvv(const vector_t *v)
{
	float w = v->w; //拿到这个顶点的深度
	int check = 0;

	//保证 Z在0-w之间  （保证Z/w在0-1之间）
	if (v->z < 0.0f)  // 在CVV背面(摄像头背面)
		check |= 1;
	if (v->z >  w) //z轴 超过了W的值， 说明  到时候 (x,y,z,w)除以w后投影到正方体，  Z/W>1.  而正方体的时候， Z 需要在0-1之间
		check |= 2;

	//保证 x  y 都在 -w和w之间 （-1和1之间）
	if (v->x < -w)  //x轴 超过了W的值， 说明  到时候 (x,y,z,w)除以w后投影到正方体，  x/W>1.  而正方体的时候， x 需要在-1 到 1之间
		check |= 4;
	if (v->x >  w)
		check |= 8;
	if (v->y < -w)  //同上
		check |= 16;
	if (v->y >  w)
		check |= 32;
	return check;
}


//transform_homogenize翻转的过程,用于把像素从屏幕返回到主摄像机空间，好进行灯光的深度计算。
void transform_homogenize_reverse(const transform_t *ts, point_t *x, const point_t *y)
{
	float rhw = 1.0f / y->w;

	x->x = (y->x * 2 / ts->screen_width - 1.0f)*rhw;

	x->y = (1.0f-y->y *2 / ts->screen_height ) *rhw;

	x->z = x->z *rhw;

	x->w = rhw;
}


//TODO:以后传入摄像机宽度，因为rendering textrue 和阴影投影是使用其他摄像机的近截面的长宽
// 归一化，然后光栅化，得到屏幕坐标 
void transform_homogenize(const transform_t *ts, point_t *y, const point_t *x)   
{
	float rhw = 1.0f / x->w;  //这里使用坐标的W值的倒数

							  //ts->w = 800 ts->h =600

							  //这条代码分几步
							  // x->x * rhw  实现了梯形的CVV空间转到矩形空间。   x取值范围在-1到1之间
							  // (x->x * rhw + 1.0f)* 0.5f;  实现了 从-1到1转到 0-1取值
							  // 最后 * ts->screen_width  按屏幕的分辨率的宽度放大到合适的位置。

							  // 变换后的坐标起始点是坐标系的中心点    
							  // 但是，在屏幕上，我们以左上角为起始点    
							  // 我们需要重新计算使他们的起始点变成左上角  所以+1 再乘0.5
	y->x = (x->x * rhw + 1.0f) * ts->screen_width * 0.5f;


	y->y = (1.0f - x->y * rhw) * ts->screen_height * 0.5f;  //同上

	y->z = x->z * rhw;  //得到在矩形空间的Z深度
						//y->w = 1.0f;  //这里 因为 是 (x,y,z,w) /w  所以 w直接赋值为1了
						//优化:这里w用来保存rhw
	y->w = rhw;
}


void vertex_rhw_init(vertex_t *v) {
	float rhw = v->pos.w;

	//根据齐次坐标的W来缩放u v
	v->tc.u *= rhw;
	v->tc.v *= rhw;

	//新增：不再缩放颜色
	//这里缩放颜色，只是用来做效果，Z轴越远，颜色越小 越黑
	v->color.r *= rhw;
	v->color.g *= rhw;
	v->color.b *= rhw;
}

//这里是顶点插值
void vertex_interp(int render_state,  vertex_t *y, const vertex_t *x1, const vertex_t *x2, float t)
{
	//顶点位置插值，通过相似三角形 获得目前扫描线左右的起始点的结束点。
	vector_interp(&y->pos, &x1->pos, &x2->pos, t);

	y->tc.u = interp(x1->tc.u, x2->tc.u, t); //uv插值  根据相似三角形原理  根据Y轴上的比例，插值
	y->tc.v = interp(x1->tc.v, x2->tc.v, t);

	y->color.r = interp(x1->color.r, x2->color.r, t);  //颜色插值
	y->color.g = interp(x1->color.g, x2->color.g, t);
	y->color.b = interp(x1->color.b, x2->color.b, t);

	y->shadowPos_z = interp(x1->shadowPos_z, x2->shadowPos_z, t);

	vector_interp(&y->worldPos, &x1->worldPos, &x2->worldPos, t);

	if (render_state & (RENDER_STATE_verterNormal_color))
		y->vertexLight = interp(x1->vertexLight, x2->vertexLight, t);
}

//这是扫描线的像素插值  计算每个像素点的过渡的差值  包括坐标x,y,z,w 颜色rpg
void vertex_division(int render_state, vertex_t *y, const vertex_t *begin, const vertex_t *end, float width) {

	float inv = 1.0f / width; //使用扫描线的宽度来插值， 扫描线长度/它的宽度，就等于这个扫描线拥有的像素数量
	y->pos.x = (end->pos.x - begin->pos.x) * inv;
	y->pos.y = (end->pos.y - begin->pos.y) * inv;
	y->pos.z = (end->pos.z - begin->pos.z) * inv;
	y->pos.w = (end->pos.w - begin->pos.w) * inv; //注意，这里的W值也是插值的  w倒数用于作为深度。  非线性插值，用的 1/w

	//TODO： 阴影 以后决定是否插值
	y->shadowPos_z = (end->shadowPos_z - begin->shadowPos_z) * inv;

	y->tc.u = (end->tc.u - begin->tc.u) * inv;
	y->tc.v = (end->tc.v - begin->tc.v) * inv;
	y->color.r = (end->color.r - begin->color.r) * inv;
	y->color.g = (end->color.g - begin->color.g) * inv;
	y->color.b = (end->color.b - begin->color.b) * inv;

	if (render_state & RENDER_STATE_verterNormal_color)//计算扫描线上 起始点的灯光插值的步长
		y->vertexLight = (end->vertexLight - begin->vertexLight) * inv;

	
}


// 根据三角形拆分为 0-2 个三角形，并且返回合法三角形的数量
int trapezoid_init_triangle(trapezoid_t *trap, const vertex_t *p1, const vertex_t *p2, const vertex_t *p3) //增加法线传进三角形
{
	const vertex_t *p;

	if (p1->pos.y > p2->pos.y)
		p = p1, p1 = p2, p2 = p; //如果P1 比P2高 交换P1 和 P2
	if (p1->pos.y > p3->pos.y)
		p = p1, p1 = p3, p3 = p;//交换P1 和 P3 确保P1是最低下的
	if (p2->pos.y > p3->pos.y)
		p = p2, p2 = p3, p3 = p; //确保p2是中间  P3是最顶


	if (p1->pos.y == p2->pos.y && p1->pos.y == p3->pos.y)//Y轴相同
		return 0;		
	if (p1->pos.x == p2->pos.x && p1->pos.x == p3->pos.x)//X轴相同
		return 0;

	// P1- - P2  
	// -  -  
	// - -  
	// -  
	// P3
	if (p1->pos.y == p2->pos.y)  //Y轴相同，只会有0到1三角形了，不需要拆分了
	{
		if (p1->pos.x > p2->pos.x)
			p = p1, p1 = p2, p2 = p;

		trap[0].top = p1->pos.y;   //top 是P1的Y
		trap[0].bottom = p3->pos.y; //bottom 是P3的Y
		trap[0].left.v1 = *p1;  // 限制梯形的左边坐标
		trap[0].left.v2 = *p3;  //
		trap[0].right.v1 = *p2; //
		trap[0].right.v2 = *p3; //
		return (trap[0].top < trap[0].bottom) ? 1 : 0;  //三点一线了 
	}

	// 因为屏幕的顺序是,左上角为原点：Y轴的排序是  P3>P2>P1 
	// 对于第一种情况来说，三角形是这样的：   
	// P1  
	// -  
	// --   
	// - -  
	// -  -  
	// -   - P2  
	// -  -  
	// - -  
	// -  
	// P3
	if (p2->pos.y == p3->pos.y) //朝下的三角形
	{
		if (p2->pos.x > p3->pos.x)
			p = p2, p2 = p3, p3 = p;
		trap[0].top = p1->pos.y;
		trap[0].bottom = p3->pos.y;
		trap[0].left.v1 = *p1;
		trap[0].left.v2 = *p2;
		trap[0].right.v1 = *p1;
		trap[0].right.v2 = *p3;
		return (trap[0].top < trap[0].bottom) ? 1 : 0;
	}

	//计算斜率 //因为后面要做插值，所以这里直接计算出斜率 既可以拆分三角形，又可以后面插值用，会比较好
	float dP1P2 = (p2->pos.x - p1->pos.x) / (p2->pos.y - p1->pos.y);
	float dP1P3 = (p3->pos.x - p1->pos.x) / (p3->pos.y - p1->pos.y);

	//这里分割为两个三角形
	trap[0].top = p1->pos.y;
	trap[0].bottom = p2->pos.y;
	trap[1].top = p2->pos.y;
	trap[1].bottom = p3->pos.y;

	// 对于第一种情况来说，三角形是这样的： triangle left  
	//       P1  
	//        -  
	//       --   
	//      - -  
	//     -  -  
	// P2 -   -   
	//     -  -  
	//      - -  
	//        -  
	//       P3 
	if (dP1P2<dP1P3) {
		trap[0].left.v1 = *p1;
		trap[0].left.v2 = *p2;
		trap[0].right.v1 = *p1;
		trap[0].right.v2 = *p3;

		trap[1].left.v1 = *p2;
		trap[1].left.v2 = *p3;
		trap[1].right.v1 = *p1;
		trap[1].right.v2 = *p3;
	}
	// 对于第二种情况来说，三角形是这样的：triangle right  
	// P1  
	// -  
	// --   
	// - -  
	// -  -  
	// -   - P2  
	// -  -  
	// - -  
	// -  
	// P3 
	else {
		trap[0].left.v1 = *p1;
		trap[0].left.v2 = *p3;
		trap[0].right.v1 = *p1;
		trap[0].right.v2 = *p2;

		trap[1].left.v1 = *p1;
		trap[1].left.v2 = *p3;
		trap[1].right.v1 = *p2;
		trap[1].right.v2 = *p3;
	}

	return 2;
}

//提前求出斜率，优化
//按照Y坐标计算出左右两条边纵坐标等于Y的顶点
void trapezoid_edge_interp(device_t *device, trapezoid_t *trap, float y ) {

	//梯度
	float t1 = (y - trap->left.v1.pos.y) / (trap->left.v2.pos.y - trap->left.v1.pos.y);
	//float worldti = (y - trap->left.v1.worldPos.y) / (trap->left.v2.worldPos.y - trap->left.v1.pos.y);

	vertex_interp(device->render_state, &trap->left.v, &trap->left.v1, &trap->left.v2, t1); //三角形左边的斜线 决定扫描线左边起点
																  //梯度
	//float t2 = (y - trap->right.v1.pos.y) / (trap->right.v2.pos.y - trap->right.v1.pos.y);

	//vertex_interp(&trap->right.v, &trap->right.v1, &trap->right.v2, t2);//三角形右边的斜线 决定扫描线右边终点
																	//梯度
	//float t1 = (y - trap->left.v1.worldPos.y) / (trap->left.v2.worldPos.y - trap->left.v1.worldPos.y);
//	vertex_interp(&trap->left.v, &trap->left.v1, &trap->left.v2, t1); //三角形左边的斜线 决定扫描线左边起点
																	  //梯度
	float t2 = (y - trap->right.v1.pos.y) / (trap->right.v2.pos.y - trap->right.v1.pos.y);
	vertex_interp(device->render_state,&trap->right.v, &trap->right.v1, &trap->right.v2, t2);//三角形右边的斜线 决定扫描线右边终点

}

// 根据左右两边的端点，初始化计算出扫描线的起点和步长
void trapezoid_init_scan_line(int  renderState,const trapezoid_t *trap, scanline_t *scanline, int y) {

	float width = trap->right.v.pos.x - trap->left.v.pos.x; //扫描线宽度
	scanline->x = (int)(trap->left.v.pos.x + 0.5f); //扫描线起点   +0.5 四舍五入
	scanline->width = (int)(trap->right.v.pos.x + 0.5f) - scanline->x; //获得整型的扫描线宽度
	scanline->y = y; //扫描线Y值

	scanline->v = trap->left.v; //扫描线的左边起点

	if (trap->left.v.pos.x >= trap->right.v.pos.x) //如果左边X轴大于等于右边，则说明 这条扫描线不存在
		scanline->width = 0;

	//计算扫描线的步长  也就是扫描线内部，每一个像素到下一个像素，所需要的插值
	vertex_division(renderState,&scanline->step, &trap->left.v, &trap->right.v, width);
}


// 设置当前纹理
void device_set_texture(device_t *device, void *bits, long pitch, int w, int h) {


	char *ptr = (char*)bits; //
	int j;
	assert(w <= 1024 && h <= 1024); //不能超过1024

	for (j = 0; j < h; ptr += pitch, j++) 	//重新计算每行纹理的指针    device->texture的行数 是纹理的高度 
		device->texture[j] = (IUINT32*)ptr;
	device->tex_width = w;
	device->tex_height = h;
	device->max_u = (float)(w - 1);
	device->max_v = (float)(h - 1);
}

// 画点 1.光栅化2D点（就是在二维数组上画点，了解色彩基本原理，并解决影像输出问题）
void device_pixel(device_t *device, int x, int y, IUINT32 color) {
	if (((IUINT32)x) < (IUINT32)device->width && ((IUINT32)y) < (IUINT32)device->height) //这里以后优化，光栅化那里已经判断了 像素是否在屏幕内
	{
		device->framebuffer[y][x] = color;
	}
}


// x y都必须是整数。。。
// 绘制线段 2.光栅化2D直线（布雷森漢姆直線演算法、吴小林直线算法等） 起点-终点，寻找线段上离二维栅格最近的像素点
void device_draw_line(device_t *device, int x1, int y1, int x2, int y2, IUINT32 c)
{
	int x, y, rem = 0;

	//这个没有寻找最近的栅格...   只是计算了下距离。
	if (x1 == x2 && y1 == y2)  //同一点，绘制一个像素点
	{
		device_pixel(device, x1, y1, c);
	}
	else if (x1 == x2)  //说明是垂直线
	{
		int inc = (y1 <= y2) ? 1 : -1;
		for (y = y1; y != y2; y += inc) //朝上或者朝下 
			device_pixel(device, x1, y, c);
		device_pixel(device, x2, y2, c);
	}
	else if (y1 == y2) //说明是平行线
	{
		int inc = (x1 <= x2) ? 1 : -1;
		for (x = x1; x != x2; x += inc)
			device_pixel(device, x, y1, c);
		device_pixel(device, x2, y2, c);
	}
	else
	{
		int dx = (x1 < x2) ? x2 - x1 : x1 - x2;
		int dy = (y1 < y2) ? y2 - y1 : y1 - y2;

		if (dx >= dy) //说明跟X轴夹角小于45
		{
			if (x2 < x1)
				x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
			for (x = x1, y = y1; x <= x2; x++)
			{
				device_pixel(device, x, y, c);
				rem += dy;
				if (rem >= dx)
				{
					rem -= dx;
					y += (y2 >= y1) ? 1 : -1;
					device_pixel(device, x, y, c);
				}
			}
			device_pixel(device, x2, y2, c);
		}
		else
		{
			if (y2 < y1)
				x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
			for (x = x1, y = y1; y <= y2; y++)
			{
				device_pixel(device, x, y, c);
				rem += dx;
				if (rem >= dy)
				{
					rem -= dy;
					x += (x2 >= x1) ? 1 : -1;
					device_pixel(device, x, y, c);
				}
			}
			device_pixel(device, x2, y2, c);
		}
	}
}

//以后直接判断 texture是否为null，删除纹理模式

// 根据坐标读取纹理
IUINT32 device_texture_read(const device_t *device, float u, float v) {
	int x, y;
	u = u * device->max_u;
	v = v * device->max_v;
	x = (int)(u + 0.5f); //四舍五入
	y = (int)(v + 0.5f);
	x = CMID(x, 0, device->tex_width - 1);
	y = CMID(y, 0, device->tex_height - 1);
	return device->texture[y][x];
}


//=====================================================================
// 像素阶段
//=====================================================================

// 阴影深度图，1. 存储在每个顶点

//2. 在摄像机阶段 要转换 插值 到像素位置的深度图来比较

//3. 要给每个像素，要转换回阴影摄像机空间，去跟深度图对比深度

// 绘制扫描线
void device_draw_scanline(device_t *device, scanline_t *scanline, float surfaceLight)
{
	int scanlineY = scanline->y;

	IUINT32 *framebuffer = device->framebuffer[scanlineY]; //拿到当前这一行的扫描线起点属性，再通过步进插值 帧缓冲
	float *zbuffer = device->zbuffer[scanlineY]; //拿到当前这一行的扫描线  Zbuffer
	float *shadowbuffer = device->shadowbuffer; //拿到当前这一行的扫描线  Zbuffer
	int scanlineX = scanline->x;
	int lineWidth = scanline->width;
	int width = device->width; //显示器范围  TODO：以后要加上摄像机范围
	int render_state = device->render_state;

	//逐个拿到当前扫描线的像素
	for (; lineWidth > 0; scanlineX++, lineWidth--)
	{
		// 从0-w递增的x ，只要小于设备宽度，就渲染。
		if (scanlineX >= 0 && scanlineX < width)
		{
			//每次for循环后要后移，注意不要缓存他
			float rhw = scanline->v.pos.w; //前期用于坐标转换，后期用于存储深度

			//表示没有被遮挡 或者表示不进行深度测试
			if (zbuffer == NULL || rhw >= zbuffer[scanlineX]) //判断深度是否大于缓存中Zbuffer  默认深度为0  //这里深度测试，是判断的扫描线的起点的深度，按理说，应该在后面，判断像素深度
			{
				if (zbuffer != NULL)
					zbuffer[scanlineX] = rhw;//最前面的深度
				 			
				//计算shadow 或者是 rendertextrue

				point_t interpos = scanline->v.pos;

				////从CVV空间回到主摄像机空间
				//transform_homogenize_reverse(&device->transform,&interpos, &interpos);

				//使用转置矩阵 从主摄像机空间回到世界空间
 				point_t worldpos = scanline->v.worldPos;
				//worldpos.w = 1;


				//再回到阴影相机的深度空间
				camera *shadow_camera = &cameras[0];
				matrix_apply(&worldpos, &worldpos, &shadow_camera->view);
				matrix_apply(&worldpos, &worldpos, &shadow_camera->projection_trans);

				transform_homogenize(&device->transform, &worldpos, &worldpos);

				//device->shadowbuffer[y*width + x] = z;



				//准备像素阶段的数据   将用于模型的顶点结构转换为用于像素阶段的v2f结构
				v2f vf;
				vf.color = scanline->v.color;
				vf.pos = scanline->v.pos;
				vf.normal = scanline->v.normal;

				//scanlineY*width + scanlineX]
				int shadowY  =(int)(worldpos.y + 0.5);

				int shadowX  = (int)(worldpos.x + 0.5);

				float pcfDepth = device->shadowbuffer[shadowY*shadow_camera->width + shadowX];

				//point_t pos;//位置

				//texcoord_t tc; //纹理坐标
				//color_t color; //颜色       
				//vector_t normal; //给寄存器增加法线    将这个合并 float rhw 合并到;  //该顶点的之前的w的倒数， 也就是缩放大小
				//point_t worldPos;  //之前在世界坐标的位置
				//float vertexLight; //顶点灯光的强度
				//point_t shadowPos; //投影摄像机的光栅化位置

				//frag_shader(device, &vf, &vf.color);


				if (render_state & RENDER_STATE_COLOR) //使用位运算判断  //颜色模式是通过 顶点的W值来插值，但平面着色模式不是
				{
					//获取颜色 0-1
					float r = vf.color.r; //*pixRhw;   //插值回来
					float g = vf.color.g; //*pixRhw;
					float b = vf.color.b; //*pixRhw;

					//转为255
					int R = (int)(r * 255.0f);
					int G = (int)(g * 255.0f);
					int B = (int)(b * 255.0f);

					//保证在0-255之间
					R = CMID(R, 0, 255);
					G = CMID(G, 0, 255);
					B = CMID(B, 0, 255);

					//在像素阶段 根据深度图，计算深度，是否要产生阴影
					if (device->shadowbuffer != NULL)
					{
						float z = scanline->v.shadowPos_z;
						//float pcfDepth = device->shadowbuffer[scanlineY*width + scanlineX];
						//if (pcfDepth < 0)
						//{
						//	pcfDepth = pcfDepth;
						//}


 						if (pcfDepth >0 && z > pcfDepth && pcfDepth !=0)
						{
							//printf("nowZ========%f------oldZ==========%f\n", z, device->shadowbuffer[scanlineY*width + scanlineX]);
							R *= 0.3f;
							G *= 0.3f;
							B *= 0.3f;
						}
					}
		
					framebuffer[scanlineX] = (R << 16) | (G << 8) | (B);  //<<是左移运算符    RGB 每个占8位.  或运算，恰好保留有1的位。 就合并RGB为一个int了
					
				}

				else if (render_state & RENDER_STATE_TEXTURE)
				{
					float u = scanline->v.tc.u;//* pixRhw;
					float v = scanline->v.tc.v;//* pixRhw;
					IUINT32 cc = device_texture_read(device, u, v);
					framebuffer[scanlineX] = cc;
				}

				//顶点插值
				else if (render_state &(RENDER_STATE_verterNormal_color))
				{
					//asset(v.color.r<0.2f);
					//获取颜色 0-1
					float r = vf.color.r + AmbientLight.r; //* pixRhw; 
					float g = vf.color.g + AmbientLight.g;//* pixRhw;
					float b = vf.color.b + AmbientLight.b;//* pixRhw;

					
					float verterLight = scanline->v.vertexLight + scanline->step.vertexLight;

					//转为255
					int R = (int)(r * 255.0f* verterLight);
					int G = (int)(g * 255.0f* verterLight);
					int B = (int)(b * 255.0f* verterLight);

					//保证在0-255之间
					R = CMID(R, 0, 255);
					G = CMID(G, 0, 255);
					B = CMID(B, 0, 255);

					//位运算 
					framebuffer[scanlineX] = (R << 16) | (G << 8) | (B);  //<<是左移运算符    RGB 每个占8位.  或运算，恰好保留有1的位。 就合并RGB为一个int了

				}

				else if (render_state &(RENDER_STATE_surfaceNormal_color))
				{
					//获取颜色 0-1
					float r = vf.color.r;// *pixRhw; 
					float g = vf.color.g;// *pixRhw;
					float b = vf.color.b;// *pixRhw;

					//转为255
					int R = (int)(r * 255.0f*surfaceLight);
					int G = (int)(g * 255.0f*surfaceLight);
					int B = (int)(b * 255.0f*surfaceLight);

					//保证在0-255之间
					R = CMID(R, 0, 255);
					G = CMID(G, 0, 255);
					B = CMID(B, 0, 255);

					//位运算 
					framebuffer[scanlineX] = (R << 16) | (G << 8) | (B);  //<<是左移运算符    RGB 每个占8位.  或运算，恰好保留有1的位。 就合并RGB为一个int了

				}
			}
		}

		if (scanlineX >= width)
			break;
		vertex_add(&scanline->v, &scanline->step); //根据之前计算出来的插值，增加插值生成下一个点的数据,用于下一个点
	}
}


void device_draw_scanline_shadow(device_t *device, scanline_t *scanline, float surfaceLight)
{
	int scanlineY = scanline->y;

	IUINT32 *framebuffer = device->framebuffer[scanlineY]; //拿到当前这一行的扫描线起点属性，再通过步进插值 帧缓冲
	float *zbuffer = device->zbuffer[scanlineY]; //拿到当前这一行的扫描线  Zbuffer
	int scanlineX = scanline->x;
	int lineWidth = scanline->width;
	int width = device->width; //显示器范围  TODO：以后要加上摄像机范围
	int render_state = device->render_state;


	//逐个拿到当前扫描线的像素
	for (; lineWidth > 0; scanlineX++, lineWidth--)
	{
		// 从0-w递增的x ，只要小于设备宽度，就渲染。
		if (scanlineX >= 0 && scanlineX < width)
		{
			//每次for循环后要后移，注意不要缓存他
			float rhw = scanline->v.pos.w; //前期用于坐标转换，后期用于存储深度

			//表示没有被遮挡 或者表示不进行深度测试
			if (zbuffer == NULL || rhw >= zbuffer[scanlineX]) //判断深度是否大于缓存中Zbuffer  默认深度为0  //这里深度测试，是判断的扫描线的起点的深度，按理说，应该在后面，判断像素深度
			{
				if (zbuffer != NULL)
					zbuffer[scanlineX] = rhw;//最前面的深度

#pragma region 计算阴影颜色 


				//根据深度图，写入深度图  TODO: 以后这块分离为pass = 顶点+像素
				if (device->shadowbuffer != NULL) 
				{
					float z = scanline->v.shadowPos_z;
					//printf("nowZ========%f------oldZ==========%f\n",z , device->shadowbuffer[scanlineY*width + scanlineX]);
					//记录最小（离）

					//if (z < 0)
					//{
					//	z = z;
					//}

					//if (device->shadowbuffer[scanlineY*width + scanlineX] < 0) {
					//	device->shadowbuffer[scanlineY*width + scanlineX] = device->shadowbuffer[scanlineY*width + scanlineX];
					//}

					int index = scanlineY * width + scanlineX;

					if ( device->shadowbuffer[index] <=0) {
						device->shadowbuffer[index] = z;
					}
					if (z < device->shadowbuffer[index] && device->shadowbuffer[index] != 0) {
						device->shadowbuffer[index] = z;
					}
				}
#pragma endregion

				//计算shadow 或者是 rendertextrue

				//point_t interpos = scanline->v.pos;


				//transform_homogenize_reverse(&device->transform, &interpos, &interpos);//从CVV空间到屏幕

				//使用转置矩阵 从主摄像机空间回到世界空间


				//准备像素阶段的数据   将用于模型的顶点结构转换为用于像素阶段的v2f结构
				v2f vf;
				vf.color = scanline->v.color;
				vf.pos = scanline->v.pos;
				vf.normal = scanline->v.normal;

				//point_t pos;//位置

				//texcoord_t tc; //纹理坐标
				//color_t color; //颜色       
				//vector_t normal; //给寄存器增加法线    将这个合并 float rhw 合并到;  //该顶点的之前的w的倒数， 也就是缩放大小
				//point_t worldPos;  //之前在世界坐标的位置
				//float vertexLight; //顶点灯光的强度
				//point_t shadowPos; //投影摄像机的光栅化位置

				//frag_shader(device, &vf, &vf.color);

				//TODO: 生成深度图

				if (render_state & RENDER_STATE_COLOR) //使用位运算判断  //颜色模式是通过 顶点的W值来插值，但平面着色模式不是
				{
					//获取颜色 0-1
					float r = vf.color.r; //*pixRhw;   //插值回来
					float g = vf.color.g; //*pixRhw;
					float b = vf.color.b; //*pixRhw;

					//转为255
					int R = (int)(r * 255.0f);
					int G = (int)(g * 255.0f);
					int B = (int)(b * 255.0f);

					//保证在0-255之间
					R = CMID(R, 0, 255);
					G = CMID(G, 0, 255);
					B = CMID(B, 0, 255);

					//根据深度图，计算深度，是否要产生阴影
					//if (device->shadowbuffer != NULL)
					//{
					//	//float z = scanline->v.shadowPos_z;
					//
					//	////记录最小（离）

					//	//if (device->shadowbuffer[scanlineY*width + scanlineX] == 0)
					//	//{
					//	////	printf("2222222");
					//	//	device->shadowbuffer[scanlineY*width + scanlineX] = z;
					//	//	framebuffer[scanlineX] = (R << 16) | (G << 8) | (B);
					//	//}

					//	//else if (z > device->shadowbuffer[scanlineY*width + scanlineX]) 
					//	//{
					//	//	//printf("111111111111");
					//	//	device->shadowbuffer[scanlineY*width + scanlineX] = z;
					//	//	framebuffer[scanlineX] = (R << 16) | (G << 8) | (B);
					//	//}
					//	//else
					//	{
					//		//printf("nowZ========%f------oldZ==========%f\n", z, device->shadowbuffer[scanlineY*width + scanlineX]);
					//		R = 0;
					//		G = 0;
					//		B = 0;
					//		framebuffer[scanlineX] = (R << 16) | (G << 8) | (B);
					//	}
					//}
					//else 
					//framebuffer[scanlineX] = (R << 16) | (G << 8) | (B);  //<<是左移运算符    RGB 每个占8位.  或运算，恰好保留有1的位。 就合并RGB为一个int了
				}
			}
		}

		if (scanlineX >= width)
			break;
		vertex_add(&scanline->v, &scanline->step); //根据之前计算出来的插值，增加插值生成下一个点的数据,用于下一个点
	}
}
void device_render_trap_shadow(device_t *device, trapezoid_t *trap, float surfaceLight)
{
	scanline_t scanline;
	int j, top, bottom;
	top = (int)(trap->top + 0.5f);
	bottom = (int)(trap->bottom + 0.5f);

	for (j = top; j < bottom; j++)  //从top到bottom 每一个y轴像素一个扫描线
	{
		if (j >= 0 && j < device->height)
		{   
			// 通过Y坐标差---得到当前边的斜率，插值---颜色、深度 坐标
			trapezoid_edge_interp(device, trap, (float)j + 0.5f);

			// 根据左右两边的端点，初始化计算出扫描线的起点和步长   包括扫描线的x y  z  w的步长 
			trapezoid_init_scan_line(device->render_state, trap, &scanline, j);

			//TODO: Z buffer 需要在下一个摄像机之前清空

			//绘制扫描线，通过步长得到最终值
			device_draw_scanline_shadow(device, &scanline, surfaceLight);
		}
		if (j >= device->height)
			break;
	}
}


void device_render_trap(device_t *device, trapezoid_t *trap, float surfaceLight)
{
	scanline_t scanline;
	int j, top, bottom;
	top = (int)(trap->top + 0.5f);
	bottom = (int)(trap->bottom + 0.5f);

	for (j = top; j < bottom; j++)  //从top到bottom 每一个y轴像素一个扫描线
	{
		if (j >= 0 && j < device->height)
		{
			// 按照Y坐标计算出当前扫描线的X坐标，并且插值 颜色、深度
			trapezoid_edge_interp(device, trap, (float)j + 0.5f);

			// 根据左右两边的端点，初始化计算出扫描线的起点和步长   包括扫描线的x y  z  w的步长 
			trapezoid_init_scan_line(device->render_state,trap, &scanline, j);

			//绘制扫描线，通过步长得到最终值
			device_draw_scanline(device, &scanline, surfaceLight);
		}
		if (j >= device->height)
			break;
	}
}


//查询这个三角面的阴影深度图
void device_draw_primitive_shadow(device_t *device, vertex_t *v1, vertex_t *v2, vertex_t *v3)
{
	//1--------物体空间------------------------------

	point_t world_pos1, world_pos2, world_pos3, //世界坐标
		raster_pos1, raster_pos2, raster_pos3, // 光栅化坐标，在一个2D平面上，对应到屏幕上真正的像素了（本应该通过当前的采样方式来对应，这里直接是按屏幕像素大小缩放）	
		project_pos1, project_pos2, project_pos3; //投影坐标 cvv空间


	transform_t transform = device->transform;

	//2--------世界空间----------计算光照---------------------------------如果是烘焙 没法考虑摄像机遮挡，所以在这里直接计算---//

	//坐标转到世界空间
	matrix_apply(&world_pos1, &v1->pos, &transform.model);
	matrix_apply(&world_pos2, &v2->pos, &transform.model);
	matrix_apply(&world_pos3, &v3->pos, &transform.model);

	 //背面剔除
	if (device->cull > 0)
	{
		float cullValue = CullCalcutate(&world_pos1, &world_pos2, &world_pos3, &device->curCamera.eye);
		if (device->cull == 1)
		{
			if (cullValue <= 0)
				return;
		}
		else if (device->cull == 2) {
			if (cullValue > 0)
				return;
		}
	}

	// 这里的裁剪不准确，只要有顶点不满足，则剔除，可以完善为具体判断几个点在 cvv内以及同cvv相交平面的坐标比例
	// 进行进一步精细裁剪，将一个分解为几个完全处在 cvv内的三角形

	//--------------------------------4.CVV空间裁剪(视锥裁剪)-----------------------------

	matrix_apply(&project_pos1, &v1->pos, &((&transform)->mvp));
	matrix_apply(&project_pos2, &v2->pos, &((&transform)->mvp));
	matrix_apply(&project_pos3, &v3->pos, &((&transform)->mvp));

	if (transform_check_cvv(&project_pos1) != 0) return;
	if (transform_check_cvv(&project_pos2) != 0) return;
	if (transform_check_cvv(&project_pos3) != 0) return;


	//法线转换到世界空间
	/*vector_t world_normal1, world_normal2, world_normal3;
	matrix_apply(&world_normal1, &v1->normal, &transform.model);
	matrix_apply(&world_normal2, &v2->normal, &transform.model);
	matrix_apply(&world_normal3, &v3->normal, &transform.model);*/
	//阴影预备
	//DisVertexToLight(lightPosition, &world_pos1);

	//转换到灯光空间


	//--------如果是动态物体-------在视锥裁剪之后计算，比较节约性能---------另外,部分shader特效需要在摄像机空间计算法线、摄像机、灯光三者的角度------//


	//灯光参数，用于乘基本颜色     (0-1)之间 被用作颜色的亮度 
	float surfaceLight = 0; //表面灯光
	float v1Light = 0;

	transform_t shadow_transform = device->transform;

	//避免重复计算rhw. 在光栅化初期计算完了就保存
	//2.------------------光栅化空间--------------5.光栅化过程：先归一化，从CVV空间到矩形空间，然后从 -1到1变换到0到1的取值范围，然后 拿到屏幕的长宽，取得屏幕坐标-------------
	transform_homogenize(&shadow_transform, &raster_pos1, &project_pos1);
	transform_homogenize(&shadow_transform, &raster_pos2, &project_pos2);
	transform_homogenize(&shadow_transform, &raster_pos3, &project_pos3);


	//---------TODO：生成阴影摄像机的深度图
	//如果阴影,插值
	point_t shadow_view_pos1, shadow_view_pos2, shadow_view_pos3;

	//切换到光源视点，渲染一张图（平行光是正交投影,点光源是透视投影）

	matrix_apply(&shadow_view_pos1, &v1->pos, &((&shadow_transform)->mv));
	matrix_apply(&shadow_view_pos2, &v2->pos, &((&shadow_transform)->mv));
	matrix_apply(&shadow_view_pos3, &v3->pos, &((&shadow_transform)->mv));


	//TODO：这里以后要改为投影摄像机的成像大小 不用屏幕大小
   //transform_homogenize(&shadow_transform, &shadow_raster_pos1, &shadow_project_pos1);
   //transform_homogenize(&shadow_transform, &shadow_raster_pos2, &shadow_project_pos2);
   //transform_homogenize(&shadow_transform, &shadow_raster_pos3, &shadow_project_pos3);
   //point_t shadow_points[3] = point_t{ shadow_raster_pos1, shadow_raster_pos2, shadow_raster_pos3 };


		vertex_t t1 = *v1, t2 = *v2, t3 = *v3;
		trapezoid_t traps[2];
		int n;

		//这是屏幕坐标
		t1.pos = raster_pos1;
		t2.pos = raster_pos2;
		t3.pos = raster_pos3;

		t1.shadowPos_z = shadow_view_pos1.z;
		t2.shadowPos_z = shadow_view_pos2.z;
		t3.shadowPos_z = shadow_view_pos3.z;

		//vertex_rhw_init(&t1);	// 初始化 w
		//vertex_rhw_init(&t2);	// 初始化 w
		//vertex_rhw_init(&t3);	// 初始化 w

		//TODO:这里的插值颜色，要三个顶点决定

		// 拆分三角形为0-2个三角形，并且返回可用三角形数量   注意，拆分后的上三角形的底边平行于X轴，下三角形的顶边平行于X轴
		n = trapezoid_init_triangle(traps, &t1, &t2, &t3);

		//对两个梯形继续拆分为扫描线，一条线一条线的渲染
		if (n >= 1)
			device_render_trap_shadow(device, &traps[0], surfaceLight);
		if (n >= 2)
			device_render_trap_shadow(device, &traps[1], surfaceLight);
	


}


/*
1.物体
2.世界
3.摄像机(viewing frustum,视锥)----------------- 投影有2种方法：正投影和透视投影
近平面,是梯形体较小的矩形面,作为投影平面,
远平面是梯形体 较大的矩形,在这个梯形体中的所有顶点数据是可见的,而超出这个梯形体之外 的场景数据,会被视点去除(Frustum Culling,也称之为视锥裁剪)。
4. project and clip space  统称 投影和裁剪空间.
规范立方体(Canonical view volume, CVV，规范利于图元裁剪)------------立方体空间的w=1的投影空间（矩形，CVV 的近平面(梯形体较小的矩形面)）


确定只有当图元完全或部分的存在于视锥内部时,才需要将其光栅化。当一 个图元完全位于视体(此时视体以及变换为 CVV)内部时,
它可以直接进入下一个阶段;完全在视体外部的图元,将被剔除;
5. 光栅化..开始----------根据分辨率形成对应的像素，给定颜色、深度，放大到符合分辨率的屏幕空间

*/

//只有当我们编程创建3D对象才会需要计算面的法线向量。建模软件（3DMax和Maya等）通常在存储的数据格式中包含了面的法线信息。
// 根据 render_state 绘制原始三角形   
void device_draw_primitive(device_t *device, vertex_t *v1, vertex_t *v2, vertex_t *v3)
{
	//1--------物体空间------------------------------

	point_t world_pos1, world_pos2, world_pos3, //世界坐标
		raster_pos1, raster_pos2, raster_pos3, // 光栅化坐标，在一个2D平面上，对应到屏幕上真正的像素了（本应该通过当前的采样方式来对应，这里直接是按屏幕像素大小缩放）	
		project_pos1, project_pos2, project_pos3; //投影坐标 cvv空间

	int render_state = device->render_state;

	transform_t transform = device->transform;

	//2--------世界空间----------计算光照---------------------------------如果是烘焙 没法考虑摄像机遮挡，所以在这里直接计算---//

	//坐标转到世界空间
	matrix_apply(&world_pos1, &v1->pos, &transform.model);
	matrix_apply(&world_pos2, &v2->pos, &transform.model);
	matrix_apply(&world_pos3, &v3->pos, &transform.model);

	// 背面剔除
	if (device->cull > 0)
	{
		float cullValue = CullCalcutate(&world_pos1, &world_pos2, &world_pos3, &device->curCamera.eye);
		if (device->cull == 1)
		{
			if (cullValue <= 0)
				return;
		}
		else if (device->cull == 2) {
			if (cullValue> 0)
				return;
		} 
	} 

	// 这里的裁剪不准确，只要有顶点不满足，则剔除，可以完善为具体判断几个点在 cvv内以及同cvv相交平面的坐标比例
	// 进行进一步精细裁剪，将一个分解为几个完全处在 cvv内的三角形

	//--------------------------------4.CVV空间裁剪(视锥裁剪)-----------------------------

	matrix_apply(&project_pos1, &v1->pos, &((&transform)->mvp));
	matrix_apply(&project_pos2, &v2->pos, &((&transform)->mvp));
	matrix_apply(&project_pos3, &v3->pos, &((&transform)->mvp));

	if (transform_check_cvv(&project_pos1) != 0) return;
	if (transform_check_cvv(&project_pos2) != 0) return;
	if (transform_check_cvv(&project_pos3) != 0) return;


	//法线转换到世界空间
	vector_t world_normal1, world_normal2, world_normal3;
	matrix_apply(&world_normal1, &v1->normal, &transform.model);
	matrix_apply(&world_normal2, &v2->normal, &transform.model);
	matrix_apply(&world_normal3, &v3->normal, &transform.model);
	//阴影预备
	//DisVertexToLight(lightPosition, &world_pos1);

	//转换到灯光空间


	//--------如果是动态物体-------在视锥裁剪之后计算，比较节约性能---------另外,部分shader特效需要在摄像机空间计算法线、摄像机、灯光三者的角度------//


	//灯光参数，用于乘基本颜色     (0-1)之间 被用作颜色的亮度 
	float surfaceLight = 0; //表面灯光
	float v1Light = 0;

	if (render_state & RENDER_STATE_surfaceNormal_color)
	{
		surfaceLight = calculateGroudShader(&world_pos1, &world_pos2, &world_pos3); //表面灯光
																					//printf("surfaceLight出来%f", surfaceLight);
	}
	else if (render_state & RENDER_STATE_verterNormal_color)
	{
		v1->vertexLight = calculateVertexLight(&world_pos1, &world_normal1);
		v2->vertexLight = calculateVertexLight(&world_pos2, &world_normal2);
		v3->vertexLight = calculateVertexLight(&world_pos3, &world_normal3);
	}


	//避免重复计算rhw. 在光栅化初期计算完了就保存
	//2.------------------光栅化空间--------------5.光栅化过程：先归一化，从CVV空间到矩形空间，然后从 -1到1变换到0到1的取值范围，然后 拿到屏幕的长宽，取得屏幕坐标-------------
	transform_homogenize(&device->transform, &raster_pos1, &project_pos1);
	transform_homogenize(&device->transform, &raster_pos2, &project_pos2);
	transform_homogenize(&device->transform, &raster_pos3, &project_pos3);


	//---------TODO：生成阴影摄像机的深度图
	//如果阴影,插值
	point_t shadow_view_pos1, shadow_view_pos2, shadow_view_pos3;

	//切换到光源视点，渲染一张图（平行光是正交投影,点光源是透视投影）

	transform_t shadow_transform = device->transform;

	matrix_apply(&shadow_view_pos1, &v1->pos, &((&shadow_transform)->mv));
	matrix_apply(&shadow_view_pos2, &v2->pos, &((&shadow_transform)->mv));
	matrix_apply(&shadow_view_pos3, &v3->pos, &((&shadow_transform)->mv));


	 //TODO：这里以后要改为投影摄像机的成像大小 不用屏幕大小
	//transform_homogenize(&shadow_transform, &shadow_raster_pos1, &shadow_project_pos1);
	//transform_homogenize(&shadow_transform, &shadow_raster_pos2, &shadow_project_pos2);
	//transform_homogenize(&shadow_transform, &shadow_raster_pos3, &shadow_project_pos3);
	//point_t shadow_points[3] = point_t{ shadow_raster_pos1, shadow_raster_pos2, shadow_raster_pos3 };


	//纹理或者色彩绘制
	if (render_state & (RENDER_STATE_TEXTURE | RENDER_STATE_COLOR | RENDER_STATE_surfaceNormal_color | RENDER_STATE_verterNormal_color))
	{
		vertex_t t1 = *v1, t2 = *v2, t3 = *v3;
		trapezoid_t traps[2];
		int n;

		//这是屏幕坐标
		t1.pos = raster_pos1;
		t2.pos = raster_pos2;
		t3.pos = raster_pos3;

		t1.shadowPos_z = shadow_view_pos1.z;
		t2.shadowPos_z = shadow_view_pos2.z;
		t3.shadowPos_z = shadow_view_pos3.z;

		t1.worldPos = world_pos1;
		t2.worldPos = world_pos2;
		t3.worldPos = world_pos3;


		//vertex_rhw_init(&t1);	// 初始化 w
		//vertex_rhw_init(&t2);	// 初始化 w
		//vertex_rhw_init(&t3);	// 初始化 w

		//TODO:这里的插值颜色，要三个顶点决定

		// 拆分三角形为0-2个三角形，并且返回可用三角形数量   注意，拆分后的上三角形的底边平行于X轴，下三角形的顶边平行于X轴
		n = trapezoid_init_triangle(traps, &t1, &t2, &t3);

		//对两个梯形继续拆分为扫描线，一条线一条线的渲染
		if (n >= 1)
			device_render_trap(device, &traps[0], surfaceLight);
		if (n >= 2)
			device_render_trap(device, &traps[1], surfaceLight);
	}

	else if (render_state & RENDER_STATE_WIREFRAME)
	{
		// 线框绘制
		device_draw_line(device, (int)raster_pos1.x, (int)raster_pos1.y, (int)raster_pos2.x, (int)raster_pos2.y, device->foreground);
		device_draw_line(device, (int)raster_pos1.x, (int)raster_pos1.y, (int)raster_pos3.x, (int)raster_pos3.y, device->foreground);
		device_draw_line(device, (int)raster_pos3.x, (int)raster_pos3.y, (int)raster_pos2.x, (int)raster_pos2.y, device->foreground);
	}
}

//顶点数据
void frag_shader(device_t *device, v2f *vf, color_t *color) {

	//如果开启阴影
	if(dirLight.shadow)
	{
		// fragPos -> 灯的坐标系 -> 灯的透视矩阵 -> 求得z坐标比较

		point_t tempPos = vf->pos; //这是像素坐标 还是世界坐标？
		
		//matrix_apply(&tempPos, &tempPos, &device->transform_shadow.mvp);
		//transform_homogenize(&device->transform_shadow, &tempPos, &tempPos);
		int y = (int)(tempPos.y + 0.5); //?
		int x = (int)(tempPos.x + 0.5);//?

		//只计算正面像素的阴影（以灯光视角）
		//vector_t tempNormal = vf->normal;
		//matrix_apply(&tempNormal, &tempNormal, &(device->transform_shadow.mv));
		//float dot = vector_dotproduct(&tempNormal, &cameras[0].front);

		//if (dot>0)
		//{
		//	float bias = 0.015 * (1.0 - dot);
		//	if (bias < 0.002f) bias = 0.001;
		//	if (y >= 0 && x >= 0 && y < camera_main.height && x < camera_main.width) {
		//		float shadow = 0.0;
		//		for (int i = -1; i <= 1; ++i)
		//		{
		//			for (int j = -1; j <= 1; ++j)
		//			{
		//				if (y + j < 0 || y + j >= camera_main.height || x + i < 0 || x + i >= camera_main.width)
		//					continue;
		//				float pcfDepth = device->shadowbuffer[(y + j)*camera_main.width + (x + i)];
		//				shadow += tempPos.z - bias > pcfDepth ? 1.0 : 0.0;
		//			}
		//		}
		//		shadow /= 9.0;

		//		color_t temp = { 0.3f,0.3f,0.3f,0.3f }; //阴影颜色

		//		temp = temp * shadow;
		//		color = &((*color) + temp);
		//		//color_scale(&temp, shadow);  //阴影浓度
		//		//color_sub(color, color, &temp); //混合阴影到当前颜色
		//	}
		//	
		//}


	}
}

//这里有三个帧缓冲  （显存）帧缓冲 -》（内存）帧缓冲 -》（设备）帧缓冲
// 设备初始化，fb为外部帧缓存，非 NULL 将引用外部帧缓存（每行 4字节对齐）
void device_init(device_t *device, int width, int height, void *fb)
{

	int lengthVoid = sizeof(void*);//sizeof(void*)是取决于这个整形量有多少个字节而不是取决于其指向了啥。x86通常4bytes.  x64通常8bytes.

	int need = lengthVoid * (height * 2 + 1024) + width * height * 8;

	//ptr是二级指针。  内存帧缓冲指向ptr，ptr再指向
	char *ptr = (char*)malloc(need + 64); //为什么是 char*

	char *nativeFramebuf, *zbuf;
	int j;
	assert(ptr); //断言


	device->framebuffer = (IUINT32**)ptr; 	//1.（显存）帧缓冲跟渲染像素完全相同 需要的是 lengthVoid * height 也就是 height这么多的指针，用于指向 （内存）帧缓冲
	ptr += lengthVoid * height; // 2. 指针后移lengthVoid * height 指向下一个区域


	device->zbuffer = (float**)ptr;  // 2.深度缓冲需要的是 lengthVoid * height  因为是扫描线，所以按高度来记录
	ptr += lengthVoid * height;

	device->texture = (IUINT32**)ptr;   //3. 转纹理空间 纹理：同样是每行索引， 1024个指针，来指向真正的纹理
	ptr += lengthVoid * 1024;

	//	nativeFramebuf = (char*)ptr; //4.（内存）帧缓冲  width * height * 4 这里是防止没有拿到windows的帧缓冲
	ptr += width * height * 4;

	zbuf = (char*)ptr;	//5.zbuffer里的 每一个点真正的Zbuf  width * height * 4;
	ptr += width * height * 4;

	// device->framebuffer 填充了ptr地址
	//同时device->framebuffer[j] 填充了 nativeFramebuf  也就是 fb这个地址
	//所以 相当于 ptr前面一段内存填充的是一个指针， 指向fb的指针
	if (fb != NULL)
		nativeFramebuf = (char*)fb; //拿到windows的帧缓冲

									//赋值 设备里的 帧缓冲 和 Z深度缓冲
	for (j = 0; j < height; j++)  // 高度这么多的扫描线
	{
		device->framebuffer[j] = (IUINT32*)(nativeFramebuf + width * 4 * j); //将本地帧缓冲赋值给指向的设备帧缓冲,每一行帧缓冲是 width * 4 宽度  framebuffer直接指向他
		device->zbuffer[j] = (float*)(zbuf + width * 4 * j);
	}

	//分配一个阴影缓存  即使是多灯光，也是在像素上依次叠加计算，现在暂时只计算单灯光阴影
	float *shadowbuffer = (float*)malloc(height * width * sizeof(float));
	device->shadowbuffer = shadowbuffer;

	device->texture[0] = (IUINT32*)ptr;  //？ 这里指针长度貌似不对
	device->texture[1] = (IUINT32*)(ptr + 16);

	memset(device->texture[0], 0, 64);
	device->tex_width = 2;
	device->tex_height = 2;
	device->max_u = 1.0f;
	device->max_v = 1.0f;
	device->width = width;
	device->height = height;
	device->background = 0xc0c0c0;  //背景颜色
	device->foreground = 0; //线框颜色

	device->render_state = RENDER_STATE_WIREFRAME;
}

// 删除设备
void device_destroy(device_t *device) {
	if (device->framebuffer)
		free(device->framebuffer);
	device->framebuffer = NULL;
	device->zbuffer = NULL;
	device->texture = NULL;
}



// 清空缓冲
void device_clear(device_t *device, int mode)
{
	int y, x, height = device->height;
	for (y = 0; y < device->height; y++)
	{
		IUINT32 *dst = device->framebuffer[y];
		IUINT32 cc = (height - 1 - y) * 230 / (height - 1);
		cc = (cc << 16) | (cc << 8) | cc;
		if (mode == 0)
			cc = device->background;
		for (x = device->width; x > 0; dst++, x--)
			dst[0] = cc;
	}
	for (y = 0; y < device->height; y++)
	{
		float *dst = device->zbuffer[y];
		for (x = device->width; x > 0; dst++, x--)
			dst[0] = 0.0f;
	}
}


void device_clear(device_t *device) {
	if (device->framebuffer != NULL) {
		for (int y = 0; y < device->height; y++)
			for (int x = 0; x < device->width; x++)
				//device->framebuffer[y * device->width + x] = device->background;
				device->framebuffer[y][x] = device->background;
	}
	// memset(device->framebuffer, 0xff, device->camera->width * device->camera->height * sizeof(IUINT32));
	if (device->zbuffer != NULL)
		memset(device->zbuffer, 0, device->width * device->height * sizeof(float));
	if (device->shadowbuffer != NULL) {
		for (int y = 0; y < device->height; y++)
			for (int x = 0; x < device->width; x++)
				device->shadowbuffer[y * device->width + x] = 0;
	}
}