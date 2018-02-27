#pragma once
#include "objectManager.h"
#include "ShadingCalculate.h"

//在渲染中，rhw 是摄像机空间转CVV空间的时候，摄像机空间坐标w的负数的倒数。
//用于摄像机空间转cvv的时候，一个是从4维缩放到3维，另一个是同时恰好实现了近大远小的效果。

//framebuffer zbuffer 到底该怎么用？
//z - test  ： 先Z/w后，转到了-1到1取值范围的矩形体，然后 再 （Z+1)/2 转到 0-1范围。   然后再拿到 1/Z， 做非线性的深度判断。 物体越远，原本的Z越大，拿到的真正的深度越小，越早绘制

//透视纹理映射 /色彩填充 /线框图

//视锥体(frustum, 是一个梯形)变换到规则观察体(Canonical View Volume 以下简称CVV)

//unity 也是左手坐标系

//左手坐标系, 矩阵如下
//Xx Yx Zx 0
//Xy Yy Zy 0
//Xz Yz Zz 0
//Dx Dy Dz 1

//X(x, y, z), Y(x, y, z), Z(x, y, z)分别表示坐标系各轴的向量, 通常为标准化向量, 若非标准化向量, 表示在该轴放大了与该向量标准化的倍数, D(x, y, z)表示在该坐标系中的所偏移的位置

//标准矩阵
//1 0 0 0
//0 1 0 0
//0 0 1 0
//0 0 0 1
//表示 X(1, 0, 0), Y(0, 1, 0), Z(0, 0, 1) 偏移(0, 0, 0), 若X非标准化向量 X(4, 0, 0)则所有的点将放大4倍.

extern int render_state;

int transform_check_cvv(const vector_t *v);

void transform_homogenize(const transform_t *ts, point_t *y, const point_t *x);

void vertex_rhw_init(vertex_t *v);

//这里是顶点插值
void vertex_interp(vertex_t *y, const vertex_t *x1, const vertex_t *x2, float t);

//这是扫描线的像素插值  计算每个像素点的过渡的差值  包括坐标x,y,z,w 颜色rpg
void vertex_division(vertex_t *y, const vertex_t *x1, const vertex_t *x2, float width);

// 根据三角形拆分为 0-2 个三角形，并且返回合法三角形的数量
int trapezoid_init_triangle(trapezoid_t *trap, const vertex_t *p1, const vertex_t *p2, const vertex_t *p3);

//提前求出斜率，优化
//按照Y坐标计算出左右两条边纵坐标等于Y的顶点
void trapezoid_edge_interp(trapezoid_t *trap, float y);

// 根据左右两边的端点，初始化计算出扫描线的起点和步长
void trapezoid_init_scan_line(const trapezoid_t *trap, scanline_t *scanline, int y);

// 画点 1.光栅化2D点（就是在二维数组上画点，了解色彩基本原理，并解决影像输出问题）
void device_pixel(device_t *device, int x, int y, IUINT32 color);

// x y都必须是整数。。。
// 绘制线段 2.光栅化2D直线（布雷森漢姆直線演算法、吴小林直线算法等） 起点-终点，寻找线段上离二维栅格最近的像素点
void device_draw_line(device_t *device, int x1, int y1, int x2, int y2, IUINT32 c);

// 根据坐标读取纹理
IUINT32 device_texture_read(const device_t *device, float u, float v);

// 绘制扫描线
void device_draw_scanline(device_t *device, scanline_t *scanline, float surfaceLight,  point_t* wolrdPoss);

// 主渲染函数   渲染一个三角形
void device_render_trap(device_t *device, trapezoid_t *trap, float surfaceLight, point_t* wolrdPoss);

// 根据 render_state 绘制原始三角形   
void device_draw_primitive(device_t *device, vertex_t *v1, vertex_t *v2, vertex_t *v3);