#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <windows.h>
#include <tchar.h>
#include <stdbool.h>
#include "geometry.h"

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

//void matrix_mul(matrix_t *c, const matrix_t *a, const matrix_t *b);

// 矩阵乘法：   y = x * m   	//向量/点，被转为矩阵后, 矩阵 乘 矩阵.  实现坐标转换
void matrix_apply(Vec4f	*y, const Vec4f *x, const matrix_t *m);

//标准矩阵 4X4 
void matrix_set_identity(matrix_t *m);

//零矩阵
void matrix_set_zero(matrix_t *m);

// 平移变换
void matrix_set_translate(matrix_t *m, float x, float y, float z);

// 缩放变换
void matrix_set_scale(matrix_t *m, float x, float y, float z);

void matrix_Obj2World(matrix_t *m, Vec4f rot, Vec4f pos);

void matrix_World2Obj(matrix_t *m, Vec4f rot, Vec4f pos, float scale);

//坐标系的变化 = 基坐标的位移+坐标系的旋转   坐标系的旋转 跟 这里坐标系内部向量的旋转是一样的
//这个矩阵是基于指定向量旋转的旋转矩阵。 该指定向量为 (x,y,z)
void matrix_set_rotate(matrix_t *m, float x, float y, float z, float theta, float xOffset, float yOffest, float zOffset);






 