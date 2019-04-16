#pragma once
#include <stdbool.h>
#include "shaderStruct.h"
#include "renderMath.h"

extern void matrix_Obj2World(matrix_t *m, vector_t rot, vector_t pos, float scale);

typedef enum {
	perspective, //透视
	orthographic //正视
} PROJECTION;

typedef struct  camera_temp
{
	vector_t eye; //当前坐标
	vector_t eyeTarget; //看向的焦点
	int cull;   // 0:不裁剪;1:裁剪反面;2:裁剪正面

	transform_t transform;

	//仿Unity，还是使用Transform组件的Rotation
	vector_t rotation;


	vector_t front; //朝前向量
	vector_t worldup;  //朝上


	matrix_t view_matrix;
	matrix_t projection_matrix;
	matrix_t view_matrix_r;

	//宽高
	int width;
	int height;

	//Maya Camera的Angle of View是Horizontal FOV的角度
	//Unity Camera的FOV是Vertical FOV的角度
	float fov;  //fov缩放时，分辨率缩放，但比例不变

	float zn; //近截面
	float zf; //远截面

			  //上下左右
	float left;
	float right;
	float bottom;
	float top;

	bool dirty; //是否清除
	PROJECTION projection;  //摄像机取景类型
	bool main;  //是否是主摄像机

				// private
	float aspect; //纵横比；如果是主摄像头，则全屏 = 屏幕高宽比
}camera;


void CameraInit();

void matrix_set_lookat(matrix_t *m, const vector_t *eye, const vector_t *at, const vector_t *up);

void matrix_set_perspective(matrix_t *m, float fovy, float aspect, float zn, float zf);

void camera_update(camera * caneraMain);
void camera_updateShadow(camera * caneraMain);