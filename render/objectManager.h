#ifndef objectManager_h
#define objectManager_h
#include "shaderStruct.h"
#include <stdbool.h>

//用于集成物体的模型、网格、动画、材质、贴图


//=====================================================================
// 几何计算：顶点、扫描线、边缘、矩形、步长计算
//=====================================================================


//这里重点说下 pos的W值， 跟color_t的W值的区别
//pos在坐标系转换中为了统一转换，扩展到了四维。 pos的W值是用于坐标从4维到3维的还原，也就是CVV到光栅化的阶段
//color_t的W值，是用于记录该顶点/像素点的深度，用于深度剔除

typedef struct {
	point_t pos;//位置
	texcoord_t tc; //纹理坐标
	color_t color; //颜色       
	vector_t normal; //给寄存器增加法线    将这个合并 float rhw 合并到;  //该顶点的之前的w的倒数， 也就是缩放大小
	point_t worldPos;  //之前在世界坐标的位置
	point_t shadowPos; //投影摄像机的光栅化位置
	float vertexLight; //顶点灯光的强度
} vertex_t; //顶点


//typedef struct {
//	vector_t pos;
//	color_t color;
//	vector_t normal;
//	vector_t tangent;
//	vector_t binormal;
//	texcoord_t texcoord;
//} a2v;
//
//
//typedef struct {
//	vector_t pos;
//	texcoord_t texcoord;
//	color_t color;
//	vector_t normal;
//	vector_t storage0;
//	vector_t storage1;
//	vector_t storage2;
//} v2f;



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


//物体
typedef struct {
	unsigned long mesh_num;
	int* material_ids;
	int texture_id;  
	bool shadow;  //是否开启产生阴影  TODO：以后加入是否接受阴影

	bool dirty;
	point_t pos; //位置
	vector_t scale; //缩放

	vector_t axis;
	float theta;
	matrix_t matrix;

	//也也可以用链表
	//struct
	//{
		//int data;   // 数据域
		//struct node *next;   // 指针域
	//} *node;

	//mesh[0]和mesh[]不占用空间，且地址紧跟在结构后面，而vertex_t *data作为指针，占用4个字节，地址不在结构之后
	vertex_t mesh[0]; //起始地址 可变数组   vertex_t data[0]，vertex_t *data, vertex_t data[]都可以 
} object_t;

//物体
typedef struct {
	unsigned long mesh_num;
	point_t pos; //位置
	vector_t axis;
	float theta;
	float scale;
	vertex_t* mesh;
} object_simple;


void vertex_add(vertex_t *y, const vertex_t *x);

//灯光  点光源  现在计算的是静态光源
extern vector_t lightPosition;

//环境光，暂时只给一个环境光的强度和颜色
extern Light_t AmbientLight;


//以后给的地面加上缩放
//地面
extern vertex_t ground_mesh[6];


//如何拆面 颜色
extern vertex_t mesh[8];

//rhw  也就是  reciprocal homogeneous w    实际是视图空间的坐标点的的-w的倒数，用于还原Z轴，做深度排序和近大远小



//因为颜色是点，所以w= 1；   normals是向量，所以w =0
extern vertex_t box_mesh[36];
#endif