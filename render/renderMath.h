#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <windows.h>
#include <tchar.h>
#include <stdbool.h>
#include "geometry.h"
#include"geometry.h"
#define PI 3.141592653
#define angle_to_radian(X) ((X)/180*PI)  //角度转弧度
#define radian_to_angle(X) ((X)/PI*180)  //弧度转角度

typedef unsigned int IUINT32;

void matrix_Obj2World(matrix_t<4,4,float> *m, Vec4f rot, Vec4f pos);

void matrix_World2Obj(matrix_t<4, 4, float>*m, Vec4f rot, Vec4f pos, float scale);

//坐标系的变化 = 基坐标的位移+坐标系的旋转   坐标系的旋转 跟 这里坐标系内部向量的旋转是一样的
//这个矩阵是基于指定向量旋转的旋转矩阵。 该指定向量为 (x,y,z)
void matrix_set_rotate(matrix_t<4, 4, float>*m, float x, float y, float z, float theta, float xOffset, float yOffest, float zOffset);






 