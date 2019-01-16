#ifndef shaderBase_h
#define shaderBase_h

#include "renderMath.h"
//#include "objectManager.h"
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

typedef struct { float u, v; } texcoord_t; //纹理


typedef struct {
	vector_t pos; //位置
	color_t color; //颜色
	vector_t normal; //法线
	vector_t tangent; //切线
	vector_t binormal; //副法线
	texcoord_t texcoord; //uv坐标
} a2v;

typedef struct {
	vector_t pos;
	texcoord_t texcoord;
	color_t color;
	vector_t normal;
	vector_t storage0;
	vector_t storage1;
	vector_t storage2;
} v2f;

//=====================================================================
// 坐标变换
//=====================================================================
typedef struct {

	//貌似没有模型矩阵

	matrix_t model;         // 物体-世界矩阵，每个物体都有自己的物体矩阵。这里要单独拿出去
	matrix_t view;          // 摄影机坐标变换 世界到摄像机坐标变换矩阵
	matrix_t projection;    // 投影变换 ，就是变换到摄像机的远平面-近平面空间内
	matrix_t mvp;     // transform = world * view * projection
	float screen_width, screen_height;             // 屏幕大小

	matrix_t vp;           // view * projection
	matrix_t mv;           // model * view
}
transform_t;

//TODO:以后同时渲染多个物体

//=====================================================================
// 渲染设备   这个渲染设备 是怎么跟最后Windwos的gdi关联上的？
//=====================================================================
typedef struct {
	transform_t transform;      // 坐标变换器
	transform_t transform_shadow;      // 坐标变换器
	int width;                  // 窗口宽度
	int height;                 // 窗口高度

								//最后输出到屏幕的像素  800*600*32
	IUINT32 **framebuffer;      // 像素缓存：framebuffer[x] 代表第x行      framebuffer[x][y] 代表第x行第y个像素

								//每个像素的深度 800*600*4   32位系统里，char 1位  short 2位   float int long 都是4位 只有double是8位
	float **zbuffer;            // 深度缓存：zbuffer[y] 为第 y行指针      zbuffer[x][y] 代表第x行第y个像素
	float *shadowbuffer;        // 阴影缓存
	IUINT32 **texture;          // 纹理：同样是每行索引
	int tex_width;              // 纹理宽度
	int tex_height;             // 纹理高度
	float max_u;                // 纹理最大宽度：tex_width - 1
	float max_v;                // 纹理最大高度：tex_height - 1
	int render_state;           // 渲染状态
	IUINT32 background;         // 背景颜色
	IUINT32 foreground;         // 线框颜色

	int cull;   // 0:不裁剪;1:裁剪反面;2:裁剪正面
}
device_t;

// 矩阵更新
void transform_update(transform_t *ts);
//void transform_update(transform_t *ts, object_simple Obj);

void transform_apply(const transform_t *ts, vector_t *posInCVV, const vector_t *posInObj);

//void vert_shader(device_t *device, a2v *av, v2f *vf);

void frag_shader(device_t *device, v2f *vf, color_t *color);

void transform_applyVP(const transform_t *ts, vector_t *posInCVV, const vector_t *posInWorld);

void device_clear(device_t *device);
#endif