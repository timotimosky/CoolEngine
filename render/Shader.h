#pragma once
#include "geometry.h"
#include "Camera.h"
#include "Model.h"
#include <stdbool.h>
#include "CMaterail.h"

//class IShader {
//	virtual ~IShader();
//	virtual Vec4f vertex(int iface, int nthvert) = 0;
//	virtual bool fragment(Vec3f bar, TGAColor& color) = 0;
//};

// 前三个模式，没有着色过程，也不需要着色器


#define RENDER_STATE_WIREFRAME_TEXT     "渲染线框"
#define RENDER_STATE_TEXTURE_TEXT        "渲染纹理"
#define RENDER_STATE_COLOR_TEXT          "顶点颜色" //这里是根据顶点深度(由Z轴生成)过渡   使用的是根据顶点索引的奇偶性随机颜色，然后顶点间随机插值。

#define RENDER_STATE_surfaceNormal_color_TEXT   "面法线的颜色显示"   //也就是所谓的平面着色：每个图元的像素都被一致的赋予该图元的第一个顶点的颜色。
#define RENDER_STATE_verterNormal_color_TEXT     "顶点法线的颜色显示"   //也就是所谓的顶点着色（高氏着色法）：：Gouraud着色：图元中的各像素的颜色值由顶点的颜色经线性插值得到。

enum Render_State
{
	RENDER_STATE_WIREFRAME = 1,
	RENDER_STATE_TEXTURE = 2,
	RENDER_STATE_COLOR = 4,
	RENDER_STATE_surfaceNormal_color = 8,
	RENDER_STATE_verterNormal_color = 16
};

typedef struct {
	Vec4f pos; //位置
	color_t color; //颜色
	Vec4f normal; //法线
	Vec4f tangent; //切线
	Vec4f binormal; //副法线
	Vec2f texcoord; //uv坐标
} a2v;

typedef struct {
	Vec4f pos;
	Vec2f texcoord;
	color_t color;
	Vec4f normal;
	Vec4f storage0;
	Vec4f storage1;
	Vec4f storage2;
} v2f;

//我们的结构模型：UObject = Model +Tramsform +materal +shader
//渲染时的结构  vertex_t edge_t +步长 
//这里重点说下 pos的W值， 跟color_t的W值的区别
//pos在坐标系转换中为了统一转换，扩展到了四维。 pos的W值是用于坐标从4维到3维的还原，也就是CVV到光栅化的阶段
//color_t的W值，是用于记录该顶点/像素点的深度，用于深度剔除

typedef class {
public:
	point_t pos;//位置 摄像机

	Vec2f tc; //纹理坐标
	color_t color; //颜色       
	Vec4f normal; //给寄存器增加法线    将这个合并 float rhw 合并到;  //该顶点的之前的w的倒数， 也就是缩放大小
	point_t worldPos;  //之前在世界坐标的位置 这个在shader中才需要
	float vertexLight; //顶点灯光的强度
	float shadowPos_z; //投影摄像机的光栅化位置 这个在shader中才需要
	Vec4f world_normal; 
	point_t project_pos;
} vertex_t; //顶点

			//描述三角形的边缘
typedef struct {
	vertex_t v,  //扫描线 跟 斜线的交点
		v1, //三角形斜线的原始起点
		v2; //三角形斜线的原始终点
} edge_t; //边缘

		  //三角形
typedef struct {
	float top, bottom; //三角形的上下
	edge_t left, right;  //三角形的两条斜边  其实 三角形的高度，已经确定了

} trapezoid_t; //步长

			   //渲染梯形的时候的扫描线
typedef struct {
	vertex_t v, step; //起点
	int x,
		y,
		width;  //整型的扫描线宽度
} scanline_t; //扫描线


void vertex_add(vertex_t* y, const vertex_t* x);


//物体
typedef class
{
public:
	unsigned long mesh_num;//..

	point_t pos;
	Vec4f axis;
	Vec4f scale;
	//mesh[0]和mesh[]不占用空间，且地址紧跟在结构后面，而vertex_t *data作为指针，占用4个字节，地址不在结构之后
	//vertex_t mesh[0]; //起始地址 可变数组   vertex_t data[0]，vertex_t *data, vertex_t data[]都可以 
	vertex_t* mesh;

	//IUINT32 texture[256][256]; //纹理
	Matrix44f model;         // 物体矩阵  由三个弧度计算出来


	//materal m_materal; //材质

	//TODO：以后增加

	//int* material_ids; //只存储ID引用

	//int texture_id;
	//bool shadow;  //是否开启产生阴影  TODO：以后加入是否接受阴影
}
Object_t;


//为了并发处理，需要分离为顶点和图元处理器，分别并发
class Shader//:public IShader
{
public:
	//当前哪个摄像机渲染它
	camera curCamera;

	vertex_t* v1;
	vertex_t* v2;
	vertex_t* v3;
	Shader();
	~Shader();

	Model *mModel; //暂时保存一份引用

	transform_t transform;

	//Vec3f varying_tri;
	matrix_t<3, 3, float> varying_tri; //图元(三角形)缓冲 triangle coordinates (clip coordinates), written by VS, read by FS

	int render_state;

	//屏幕分辨率

	//int tex_width;              // 纹理宽度
	//int tex_height;             // 纹理高度
	//IUINT32** texture; //[256][256]; //纹理

	Vec2f _uv;  //UV  // 纹理最大宽度：tex_width - 1  // 纹理最大高度：tex_height - 1

		//每个摄像机都走单独的渲染管线，下面用于缓存摄像机影响渲染管线的参数
	int cull;   // 0:不裁剪;1:裁剪反面;2:裁剪正面

	virtual Vec3f vertex(Vec3f iface, int nthvert);

	//顶点处理器处理好顶点后，每完成完一个图元(我们用的三角形，则大部分情况是三个顶点)，则传递到图元渲染器渲染
	//virtual bool fragment(Vec3f bar, TGAColor& color);

	void vertex_shader(vertex_t* v1,  int nfaces_index, int vertex_index);
	void frag_shader(vertex_t& frag_Vertex_t, IUINT32& color);
};
//在.obj文件中，我们具有以“ vt u v”开头的行，它们给出了纹理坐标数组
//刻面线“ fx / x / xx / x / xx / x / x”中中间（斜线之间）的数字是此三角形顶点的纹理坐标
//将其插入三角形内，乘以纹理图像的宽度 - 高度，您将获得要放入渲染中的颜色

//先每个物体使用每个shader渲染 都写入buffer（公用）中（这里处理屏幕分辨率）
//最后的devece 用这个buffer写入屏幕（这里处理）