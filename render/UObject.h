#pragma once
#include <stdbool.h>
#include "shaderStruct.h"
#include "CMaterail.h"


//这里重点说下 pos的W值， 跟color_t的W值的区别
//pos在坐标系转换中为了统一转换，扩展到了四维。 pos的W值是用于坐标从4维到3维的还原，也就是CVV到光栅化的阶段
//color_t的W值，是用于记录该顶点/像素点的深度，用于深度剔除

typedef struct {
	point_t pos;//位置 摄像机
				 
	texcoord_t tc; //纹理坐标
	color_t color; //颜色       
	Vec4f normal; //给寄存器增加法线    将这个合并 float rhw 合并到;  //该顶点的之前的w的倒数， 也就是缩放大小
	point_t worldPos;  //之前在世界坐标的位置 这个在shader中才需要
	float vertexLight; //顶点灯光的强度
	float shadowPos_z; //投影摄像机的光栅化位置 这个在shader中才需要
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


void vertex_add(vertex_t *y, const vertex_t *x);


//物体
typedef struct
{
	unsigned long mesh_num;//..

	point_t pos;
	Vec4f axis;
	float scaleX;
	float scaleY;
	float scaleZ;

	//mesh[0]和mesh[]不占用空间，且地址紧跟在结构后面，而vertex_t *data作为指针，占用4个字节，地址不在结构之后
	//vertex_t mesh[0]; //起始地址 可变数组   vertex_t data[0]，vertex_t *data, vertex_t data[]都可以 
	vertex_t* mesh;

	IUINT32 texture[256][256]; //纹理
	matrix_t model;         // 物体矩阵  由三个弧度计算出来


	materal m_materal; //材质

	//TODO：以后增加

	//int* material_ids; //只存储ID引用

	//int texture_id;
	//bool shadow;  //是否开启产生阴影  TODO：以后加入是否接受阴影
}
Object_t;