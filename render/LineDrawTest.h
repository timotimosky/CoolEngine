#include <iostream>
using namespace std;
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
int imageCount = 0;
bool TestimageCount(TGAImage& image, int x, int y, const TGAColor& color)
{
	//cout << x << "----" << y<<endl;
	imageCount++;
	return image.set(x, y, color);
}

//错误：绘制次数太多，性能低下。另外因为是按固定的间隔增加的 0.1 可能会错过一些格子
void  line1(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
	for (float t = 0.; t < 1.; t += .1) {
		int x = x0 + (x1 - x0) * t;
		int y = y0 + (y1 - y0) * t;
		TestimageCount(image, x, y, color);
	}
}

//错误：只能用于x1 》x0 y1》y0
void  line2(int x0, int y0,int x1,int y1,TGAImage&image,TGAColor color){
	 for(int x = x0; x <= x1; x ++){
		 float t = (x - x0) / (float)(x1 - x0);
		int y = y0  + (y1-y0) * t;
		cout << x << "----" << y <<"--t---"<<t<< endl;
		TestimageCount(image, x, y, color);
	}
}
//最终
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
	bool steep = false;

	//设想极端情况,X0 到X1 只相差1，但Y0-Y1相差100，以X++来循环，Y无法插值。所以要以最长的轴来++循环
	//由于高度大于宽度会造成空洞，需要保持不陡
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) { // if the line is steep, we transpose the image 
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) { // make it left−to−right 
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	float derror = std::abs(dy / float(dx)); //斜率，X轴上每移动一格，Y轴增加的误差
	float error = 0;
	int y = y0;


	for (int x = x0; x <= x1; x++) {
		//float t = (x - x0) / (float)(x1 - x0);
		//int y = y0 * (1. - t) + y1 * t;
		if (steep) {
			TestimageCount(image, y, x, color); // if transposed, de−transpose 
		}
		else {
			TestimageCount(image, x, y, color);
		}
		error += derror;
		if (error > .5) {
			y += (y1 > y0 ? 1 : -1);
			error -= 1.;
		}
	}
}

int TestLinemain() {
	TGAImage image(100, 100, TGAImage::RGB);
	//device_draw_line(3, 3, 67, 83,image, red);
	//line(67, 83, 3, 3, image, red);
	//lineSimple(3, 3, 8, 16, image, red);
	line(3, 3, 67, 83, image, red);
	//lineSimple(20, 13, 40, 80, image, red);
	//lineSimple(80, 40, 13, 20, image, red);
	//line(3, 3, 67, 83, image, red);
	//image.set(52, 41, red);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	cout << imageCount << "----" << endl;
	return 0;
}
//// 根据左右两边的端点，初始化计算出扫描线的起点和步长
//void trapezoid_init_scan_line(int  renderState, const trapezoid_t* trap, scanline_t* scanline, int y) {
//
//	float width = trap->right.v.pos.x - trap->left.v.pos.x; //扫描线宽度
//	scanline->x = (int)(trap->left.v.pos.x + 0.5f); //扫描线起点   +0.5 四舍五入
//	scanline->width = (int)(trap->right.v.pos.x + 0.5f) - scanline->x; //获得整型的扫描线宽度
//	scanline->y = y; //扫描线Y值
//
//	scanline->v = trap->left.v; //扫描线的左边起点
//
//	if (trap->left.v.pos.x >= trap->right.v.pos.x) //如果左边X轴大于等于右边，则说明 这条扫描线不存在
//		scanline->width = 0;
//
//	//计算扫描线的步长  也就是扫描线内部，每一个像素到下一个像素，所需要的插值
//	vertex_division(renderState, &scanline->step, &trap->left.v, &trap->right.v, width);
//}
//
////这里是顶点插值
//void vertex_interp(int render_state, vertex_t* y, const vertex_t* x1, const vertex_t* x2, float t)
//{
//	//顶点位置插值，通过相似三角形 获得目前扫描线左右的起始点的结束点。
//	//vector_interp(&y->pos, &x1->pos, &x2->pos, t);
//	y->pos = interp(x1->pos, x2->pos, t);
//
//	y->world_normal = interp(x1->world_normal, x2->world_normal, t);
//
//	//纹理坐标
//	y->tc = interp(x1->tc, x2->tc, t); //uv插值  根据相似三角形原理  根据Y轴上的比例，插值
//
//	y->color = interp(x1->color, x2->color, t);  //颜色插值
//
//	y->shadowPos_z = interp(x1->shadowPos_z, x2->shadowPos_z, t);
//
//	y->worldPos = interp(x1->worldPos, x2->worldPos, t);
//
//	if (render_state & (RENDER_STATE_verterNormal_color))
//		y->vertexLight = interp(x1->vertexLight, x2->vertexLight, t);
//}
//
////这是扫描线的像素插值  计算每个像素点的过渡的差值  包括坐标x,y,z,w 颜色rpg
//void vertex_division(int render_state, vertex_t* y, const vertex_t* begin, const vertex_t* end, float width) {
//
//	float inv = 1.0f / width; //使用扫描线的宽度来插值， 扫描线长度/它的宽度，就等于这个扫描线拥有的像素数量
//	y->pos = (end->pos - begin->pos) * inv;
//	//注意，这里的W值也是插值的  w倒数用于作为深度。  非线性插值，用的 1/w
//
//	//TODO： 阴影 以后决定是否插值
//	y->shadowPos_z = (end->shadowPos_z - begin->shadowPos_z) * inv;
//
//	y->tc = (end->tc - begin->tc) * inv;
//	y->color = (end->color - begin->color) * inv;
//
//	if (render_state & RENDER_STATE_verterNormal_color)//计算扫描线上 起始点的灯光插值的步长
//		y->vertexLight = (end->vertexLight - begin->vertexLight) * inv;
//}
//
//
//// 绘制扫描线
//void device_draw_scanline(device_t* device, Shader* mShader, scanline_t* scanline, float surfaceLight)
//{
//	int scanlineY = scanline->y;
//
//	IUINT32* framebuffer = device->framebuffer[scanlineY]; //拿到当前这一行的扫描线起点属性，再通过步进插值 帧缓冲
//	float* zbuffer = device->zbuffer[scanlineY]; //拿到当前这一行的扫描线  Zbuffer
//	float* shadowbuffer = device->shadowbuffer; //拿到当前这一行的扫描线  Zbuffer
//	int scanlineX = scanline->x;
//	int lineWidth = scanline->width;
//	int width = device->width; //显示器范围  TODO：以后要加上摄像机范围
//	mShader->render_state = device->render_state;
//
//	IUINT32 color;
//
//	//逐个拿到当前扫描线的像素
//	for (; lineWidth > 0; scanlineX++, lineWidth--)
//	{
//		// 从0-w递增的x ，只要小于设备宽度，就渲染。
//		if (scanlineX >= 0 && scanlineX < width)
//		{
//			//每次for循环后要后移，注意不要缓存他
//			float rhw = scanline->v.pos.w; //前期用于坐标转换，后期用于存储深度
//
//			//表示没有被遮挡 或者表示不进行深度测试
//			if (zbuffer == NULL || rhw >= zbuffer[scanlineX]) //判断深度是否大于缓存中Zbuffer  默认深度为0  //这里深度测试，是判断的扫描线的起点的深度，按理说，应该在后面，判断像素深度
//			{
//				if (zbuffer != NULL)
//					zbuffer[scanlineX] = rhw;//最前面的深度
//
//				//计算shadow 或者是 rendertextrue
//
//				//point_t interpos = scanline->v.pos;
//
//				////从CVV空间回到主摄像机空间
//				//transform_homogenize_reverse(&device->transform,&interpos, &interpos);
//
//				//使用转置矩阵 从主摄像机空间回到世界空间
//				//point_t worldpos = scanline->v.worldPos;
//				//worldpos.w = 1;
//
//
//				////再回到阴影相机的深度空间
//				//camera *shadow_camera = &cameras[0];
//				//worldpos = worldpos * shadow_camera->view;
//				//worldpos= worldpos * shadow_camera->projection_trans;
//
//				//transform_homogenize(&mShader->transform, &worldpos, &worldpos);
//
//				//int shadowY  =(int)(worldpos.y + 0.5);
//
//				//int shadowX  = (int)(worldpos.x + 0.5);
//
//				//float pcfDepth = device->shadowbuffer[shadowY*shadow_camera->width + shadowX];
//
//				//point_t pos;//位置
//
//				//texcoord_t tc; //纹理坐标
//				//color_t color; //颜色       
//				//Vec4f normal; //给寄存器增加法线    将这个合并 float rhw 合并到;  //该顶点的之前的w的倒数， 也就是缩放大小
//				//point_t worldPos;  //之前在世界坐标的位置
//				//float vertexLight; //顶点灯光的强度
//				//point_t shadowPos; //投影摄像机的光栅化位置
//
//				mShader->frag_shader(scanline->v, color);
//				framebuffer[scanlineX] = color;
//			}
//		}
//
//		if (scanlineX >= width)
//			break;
//		vertex_add(&scanline->v, &scanline->step); //根据之前计算出来的插值，增加插值生成下一个点的数据,用于下一个点
//	}
//}




