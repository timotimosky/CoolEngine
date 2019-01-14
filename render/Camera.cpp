#include "Camera.h"


float Forwardoffset = 0.01f;
camera cameras[MAX_NUM_CAMERA]; 
camera camera_main =  camera();

//TODO：这里拿到的 貌似不是真正的世界坐标转摄像机矩阵，拿到的是它的转置矩阵
// 设置摄像机  eye是目光看向的焦点  at是摄像机自身坐标  up是Y轴
void matrix_set_lookat(matrix_t *m, const vector_t *eye, const vector_t *at, const vector_t *up)
{
	//测试回滚3333333
	//测试回滚1

	vector_t xaxis, yaxis, zaxis;

	//根据摄像机的自身坐标和朝向，算出摄像机的X Y Z

	//zaxis 是 目光看向焦点的矢量   这里是跟opengl一样， Z轴正轴朝向屏幕外面
	vector_sub(&zaxis, at, eye); //Z

								 //叉乘之前要归一化。 
	vector_normalize(&zaxis);
	vector_crossproduct(&xaxis, up, &zaxis);  //叉乘得到 X轴

	vector_normalize(&xaxis);
	vector_crossproduct(&yaxis, &zaxis, &xaxis);  //up只是单纯的设置为 朝上。与目光看向焦点 构成一个平面，来运算出X轴， 然后这里还要算出一次真正的Y轴

//点积不仅能表示两个向量的角度范围 , 而且可以表示在标准化坐标系内的X , Y ,Z 轴上的对应位置 

//TODO：这里没有标准化基坐标

//摄像机坐标系的原点不一定与世界坐标系重合，同时由于自身的旋转，坐标轴也一定不与世界坐标系的坐标轴平行。为完成工作任务，需要分为两步走1.整体平移，将摄像机平移至世界坐标系原点，2.将坐标点从世界坐标系转换至摄像机坐标系。
//使用单位向量U, V, W分别代表摄像机坐标系X, Y, Z轴正向的单位向量在世界坐标系中的表示，则在摄像相机坐标系与世界坐标系原点重合的情况下，物体顶点坐标代表的向量（即从世界原点指向物体顶点的向量）
//在U, V, W上的投影大小即是物体顶点在摄像机坐标系下的坐标值。因为U, V, W是单位向量，使用二者的点乘即可以得到顶点的投影大小。
//P  =  (ax,by,cz)  a b c是已知数
//转换到另一个坐标系，为  PN = (mU, nV,qP)

//世界转视图空间矩阵

//float m[4][4] = { { xaxis.x,xaxis.y,xaxis.z,0 },{ V.x,V.y,V.z,0 },{ W.x,W.y,W.z,0 },{ 0,0,0,1 } };


//矩阵乘法 =  被变换的向量  点积   矩阵的对应的那一列标识的向量（如果是行变化，也就是左乘，就是每一列）

 //其实到时候矩阵效果就是  Px * xaxis = newPx;  原本在世界坐标系的X坐标，乘以xaxis，获得在摄像机基坐标x轴上的投影， 也就是在摄像机坐标系里的新X坐标了
 //右乘。考虑矩阵的意义，如果是斜线才有值的矩阵，从行变换来考虑。  下面这种，用列变换考虑比较好。
	m->m[0][0] = xaxis.x;
	m->m[1][0] = xaxis.y;
	m->m[2][0] = xaxis.z;
	m->m[3][0] = -vector_dotproduct(&xaxis, eye);  //这里是填写的 平移变化的基坐标

												   //这三行是 摄像机坐标系的Y轴的基向量
												   //到时候矩阵运算会跟世界坐标的Y坐标点乘,  拿到的也就是世界坐标系里的点，在 摄像机坐标系 X轴上的投影
	m->m[0][1] = yaxis.x;
	m->m[1][1] = yaxis.y;
	m->m[2][1] = yaxis.z;

	//因为摄像机的基坐标平移了，所以点也要跟着平移。
	m->m[3][1] = -vector_dotproduct(&yaxis, eye);

	m->m[0][2] = zaxis.x;
	m->m[1][2] = zaxis.y;
	m->m[2][2] = zaxis.z;
	m->m[3][2] = -vector_dotproduct(&zaxis, eye);//这里是填写的 平移变化的基坐标

	m->m[0][3] = m->m[1][3] = m->m[2][3] = 0.0f;
	m->m[3][3] = 1.0f;
}


//假设相机坐标系中的一点 P_V, 经过透视变换后为P_H(分量分别为P_Hx, P_Hy, P_Hz, 1)
//P_Hz = -P_Vz  *  (far + near) / (far - near) - 2far*near / (far - near)
//P_Hw = -P_Vz。

//因为经过透视除法后顶点的w分量是1  那么范围是 -1 到1 的x 映射到 0zl 到 1 上 就是 x‘ = x/2 + 0.5 

//透视矩阵  将摄像机坐标系的坐标转换为真正可用的屏幕坐标
//zn 近平面Z
//zf 远平面Z
//	float aspect = (float)width / ((float)height); 分辨率比例
void matrix_set_perspective(matrix_t *m, float fovy, float aspect, float zn, float zf)
{
	float fax = 1.0f / (float)tan(fovy * 0.5f);  //cot@
	matrix_set_zero(m);
	m->m[0][0] = (float)(fax / aspect);
	m->m[1][1] = (float)(fax);
	m->m[2][2] = zf / (zf - zn);
	m->m[3][2] = -zn * zf / (zf - zn);
	m->m[2][3] = 1; //令W=1
}

// 初始化，设置屏幕长宽
void transform_init(transform_t *ts, int width, int height) {
	float aspect = (float)width / ((float)height);
	matrix_set_identity(&ts->model);
	matrix_set_identity(&ts->view);

	//第二个方向是摄像机上下的夹角&  y方向的视角   cot(@/2) 也就是 near/投影面的高度的一半 当然也等于     cot(@/2)  = far/远截面高度的一半

	//这里设定上下夹角为 90度  2PI=360度  PI=180度
	matrix_set_perspective(&ts->projection, PI * 0.5f, aspect, 1.0f, 500.0f); //设定近平面为1，这样W取值为1就好了。缩放到投影面比较方便
	ts->screen_width = (float)width;
	ts->screen_height = (float)height;
	transform_update(ts);
}

void CameraInit()
{

	
}

//摄像机位置刷新后,设备的矩阵改变
void camera_update(device_t *device, camera * caneraMain) 
{

	//摄像机矩阵 摄像机的位移
	matrix_set_lookat(&device->transform.view, &(caneraMain->pos), &caneraMain->front, &caneraMain->worldup);

	//更新矩阵，获得MVP矩阵
	transform_update(&device->transform);
}


//如果是动态灯光，需要刷新
void camera_updateShadow(device_t *device, camera * caneraShadow)
{

	//摄像机矩阵 摄像机的位移
	matrix_set_lookat(&device->transform_shadow.view, &(caneraShadow->pos), &caneraShadow->front, &caneraShadow->worldup);

	//更新矩阵，获得MVP矩阵
	transform_update(&device->transform_shadow);

}

void Forward()
{
	
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
	transform_init(&device->transform, width, height);
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

// 清空 framebuffer 和 zbuffer
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

	//清空shaderbuffer,默认深度是CVV盒子最远离摄像机的边的深度，也就是1
	if (device->shadowbuffer != NULL) {
		for (int y = 0; y < device->height; y++)
			for (int x = 0; x < device->width; x++)
				device->shadowbuffer[y * device->width + x] = 1.0f;
	}
}