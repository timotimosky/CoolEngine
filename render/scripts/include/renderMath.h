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

//=====================================================================
// 坐标变换
//=====================================================================
typedef struct {

	float screen_width, screen_height;             // 屏幕大小

	Vec4f worldPos;
	Vec4f worldRot;
	Vec4f worldScale;
	//貌似没有模型矩阵

	Matrix44f model;         // 物体-世界矩阵，每个物体都有自己的物体矩阵。TODO: 以后分离到每个物体管理
	Matrix44f view;          // 摄影机坐标变换 世界到摄像机坐标变换矩阵  TODO:以后分离到每个摄像机管理
	Matrix44f projection;    // 投影变换 ，就是变换到摄像机的远平面-近平面空间内  TODO:以后分离到每个摄像机管理
	//物体与相机的距离（z）也是如此！对于具有相似x和y坐标的两个顶点，具有最大z坐标的顶点将比屏幕的中心更多地位于屏幕的中心。     
	// 另外，如果是透视视图

	Matrix44f mvp;     // transform = world * view * projection
	Matrix44f vp;           // view * projection
	Matrix44f mv;           // model * view
}
transform_t;

// 矩阵更新
void transform_update(transform_t* ts);

void Init_Model_matrix(Matrix44f& srt, const point_t& pos, const Vec4f& axis, const Vec4f& scale);

void matrix_Obj2World(matrix_t<4,4,float> &m, Vec4f rot, Vec4f pos);

void matrix_World2Obj(matrix_t<4, 4, float> &m, Vec4f rot, Vec4f pos, float scale);

//坐标系的变化 = 基坐标的位移+坐标系的旋转   坐标系的旋转 跟 这里坐标系内部向量的旋转是一样的
//这个矩阵是基于指定向量旋转的旋转矩阵。 该指定向量为 (x,y,z)
void matrix_set_rotate(matrix_t<4, 4, float>*m, Vec4f vec, float theta, Vec3f offset);






 