#ifndef shaderBase_h
#define shaderBase_h
#include <stdbool.h>
#include<vector>
using namespace std;
#include "renderMath.h"

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


 struct color_t { float r, g, b, a;

 color_t operator*(float value)
 {
	 color_t ncolor_t;
	 ncolor_t.r = r*value;
	 ncolor_t.g = g*value;
	 ncolor_t.b = b*value;
	 ncolor_t.a = a*value; //TODO:透明值考虑这里以后不计算
	 return ncolor_t;
 }


color_t operator+(color_t value)
{
	color_t ncolor_t;
	ncolor_t.r = r + value.r;
	ncolor_t.g = g + value.g;
	ncolor_t.b = b + value.b;
	ncolor_t.a = a + value.a;
	return ncolor_t;
}

//color_t operator-(color_t* value)
//{
//	color_t ncolor_t;
//	ncolor_t.r = r - value->r;
//	ncolor_t.g = g - value->g;
//	ncolor_t.b = b - value->b;
//	ncolor_t.a = a - value->a;
//	return ncolor_t;
//}
} ; //颜色

//typedef struct {
//	Vec4f pos;
//	color_t color;
//	Vec4f normal;
//	Vec4f tangent;
//	Vec4f binormal;
//	texcoord_t texcoord;
//} a2v;
//
//
//typedef struct {
//	Vec4f pos;
//	texcoord_t texcoord;
//	color_t color;
//	Vec4f normal;
//	Vec4f storage0;
//	Vec4f storage1;
//	Vec4f storage2;
//} v2f;


typedef struct { float u, v; } texcoord_t; //纹理


typedef struct {
	Vec4f pos; //位置
	color_t color; //颜色
	Vec4f normal; //法线
	Vec4f tangent; //切线
	Vec4f binormal; //副法线
	texcoord_t texcoord; //uv坐标
} a2v;

typedef struct {
	Vec4f pos;
	texcoord_t texcoord;
	color_t color;
	Vec4f normal;
	Vec4f storage0;
	Vec4f storage1;
	Vec4f storage2;
} v2f;

//=====================================================================
// 坐标变换
//=====================================================================
typedef struct {

	float screen_width, screen_height;             // 屏幕大小

	//貌似没有模型矩阵

	matrix_t model;         // 物体-世界矩阵，每个物体都有自己的物体矩阵。TODO: 以后分离到每个物体管理
	matrix_t view;          // 摄影机坐标变换 世界到摄像机坐标变换矩阵  TODO:以后分离到每个摄像机管理
	matrix_t projection;    // 投影变换 ，就是变换到摄像机的远平面-近平面空间内  TODO:以后分离到每个摄像机管理
	//物体与相机的距离（z）也是如此！对于具有相似x和y坐标的两个顶点，具有最大z坐标的顶点将比屏幕的中心更多地位于屏幕的中心。     
	// 另外，如果是透视视图，W负责缩放物体大小，来模拟视觉。远处的物体更小



	matrix_t mvp;     // transform = world * view * projection
	matrix_t vp;           // view * projection
	matrix_t mv;           // model * view
}
transform_t;


// 矩阵更新
void transform_update(transform_t *ts);
//void transform_update(transform_t *ts, object_simple Obj);

void transform_apply(const transform_t *ts, Vec4f *posInCVV, const Vec4f *posInObj);


void transform_applyVP(const transform_t *ts, Vec4f *posInCVV, const Vec4f *posInWorld);

#endif