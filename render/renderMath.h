#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <windows.h>
#include <tchar.h>
#include <stdbool.h>




#define PI 3.141592653
#define angle_to_radian(X) ((X)/180*PI)  //角度转弧度
#define radian_to_angle(X) ((X)/PI*180)  //弧度转角度

typedef unsigned int IUINT32;
struct matrix_t {
	
	
	float m[4][4];

	matrix_t& operator* (float f) {
		int i, j;
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++)
				this->m[i][j] = this->m[i][j] * f;
		}
		return *this;
	}


	matrix_t operator* (const matrix_t& right) //引用是副本，不像指针那样要求外界传递指针
	{
		matrix_t c;
		int i, j;
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				c.m[i][j] =
					(this->m[i][0] * right.m[0][j]) +
					(this->m[i][1] * right.m[1][j]) +
					(this->m[i][2] * right.m[2][j]) +
					(this->m[i][3] * right.m[3][j]);
			}
		}
		return c;
	}

	matrix_t operator-(const matrix_t & b) {
		matrix_t c;
		int i, j;
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++)
				c.m[i][j] = this->m[i][j] - b.m[i][j];
		}
		return c;
	}

	matrix_t operator+(const matrix_t & b) {
		matrix_t c;
		int i, j;
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++)
				c.m[i][j] = this->m[i][j] + b.m[i][j];
		}
		return c;
	}


} ;
struct vector_t  { float x, y, z, w;

vector_t()
{
	x = 0;
		y = 0;
		z = 0;
		w = 1;
}


vector_t(float ix, float iy, float iz, float iw)
{
	x = ix;
	y = iy;
	z = iz;
	w = iw;
}

vector_t operator*(float value)
{
	vector_t ncolor_t;
	ncolor_t.x = x * value;
	ncolor_t.y = y * value;
	ncolor_t.z = z * value;

	return ncolor_t;
}


vector_t operator+(const vector_t& value)
{
	vector_t ncolor_t;
	ncolor_t.x = x + value.x;
	ncolor_t.y = y + value.y;
	ncolor_t.z = z + value.z;

	return ncolor_t;
}

vector_t operator-(const vector_t* value)
{
	vector_t ncolor_t;
	ncolor_t.x = x - value->x;
	ncolor_t.y = y - value->y;
	ncolor_t.z = z - value->z;

	return ncolor_t;
}


} ; //使用的3D齐次坐标  其常规坐标为 {x/w, y/w, z/w; }
typedef vector_t point_t;



//不允许超过上下限 
float CMIDFloat(float x, float min, float max);

//不允许超过上下限
int CMID(int x, int min, int max);

void vector_scale(vector_t *z, float scale);

// 计算插值：t 为 [0, 1] 之间的数值 lerp插值
float interp(float x1, float x2, float t);

// | v |
float vector_length(const vector_t *v);


//齐次坐标的第二个功效就是区分向量跟点.  w为1则为点.  w为0则为向量。  因为w作用是位移，向量位移没什么意义。
// z = x + y  w一直为1
void vector_add(vector_t *z, const vector_t *x, const vector_t *y);


// z = x - y
void vector_sub(vector_t *z, const vector_t *x, const vector_t *y);

// 矢量点乘
float vector_dotproduct(const vector_t *x, const vector_t *y);


// 矢量叉乘  如果 x y 标准化，那算出来的叉乘Z也是标准化的xy平面的法线。 否则的话，要标准化一次Z
void vector_crossproduct(vector_t *z, const vector_t *x, const vector_t *y);

// 矢量lerp插值，t取值 [0, 1]
void vector_interp(vector_t *z, const vector_t *x1, const vector_t *x2, float t);

// 矢量归一化  注意归一化，不涉及W。 只有四维向三维投影的时候，W归一化才有用
void vector_normalize(vector_t *v);

//void matrix_mul(matrix_t *c, const matrix_t *a, const matrix_t *b);

// 矩阵乘法：   y = x * m   	//向量/点，被转为矩阵后, 矩阵 乘 矩阵.  实现坐标转换
void matrix_apply(vector_t	*y, const vector_t *x, const matrix_t *m);

//标准矩阵 4X4 
void matrix_set_identity(matrix_t *m);

//零矩阵
void matrix_set_zero(matrix_t *m);

// 平移变换
void matrix_set_translate(matrix_t *m, float x, float y, float z);

// 缩放变换
void matrix_set_scale(matrix_t *m, float x, float y, float z);

void matrix_Obj2World(matrix_t *m, vector_t rot, vector_t pos,float scale);

void matrix_World2Obj(matrix_t *m, vector_t rot, vector_t pos, float scale);

//坐标系的变化 = 基坐标的位移+坐标系的旋转   坐标系的旋转 跟 这里坐标系内部向量的旋转是一样的
//这个矩阵是基于指定向量旋转的旋转矩阵。 该指定向量为 (x,y,z)
void matrix_set_rotate(matrix_t *m, float x, float y, float z, float theta, float xOffset, float yOffest, float zOffset);






