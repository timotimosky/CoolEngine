#pragma once
#include <stdbool.h>
#include "shaderStruct.h"
#include "renderMath.h"

extern void matrix_Obj2World(matrix_t *m, Vec4f rot, Vec4f pos);

typedef enum {
	perspective, //透视
	orthographic //正视
} PROJECTION;

 struct  camera
{
	Vec4f eye; //当前坐标   //C的结构体里的基本元素可以 用 =赋值。但复合结构不行
	Vec4f eyeTarget; //看向的焦点

	matrix_t view_matrix_r;

	matrix_t view;          // 摄影机坐标变换 世界到摄像机坐标变换矩阵  TODO:以后分离到每个摄像机管理
	matrix_t projection_trans;    // 投影变换 ，就是变换到摄像机的远平面-近平面空间内  TODO:以后分离到每个摄像机管理
	//物体与相机的距离（z）也是如此！对于具有相似x和y坐标的两个顶点，具有最大z坐标的顶点将比屏幕的中心更多地位于屏幕的中心。     
	// 另外，如果是透视视图，W负责缩放物体大小，来模拟视觉。远处的物体更小


	//仿Unity，还是使用Transform组件的Rotation
	Vec4f rotation;


	Vec4f front; //朝前向量
	Vec4f worldup;  //朝上



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
};


void CameraInit();

void matrix_set_lookat(matrix_t *m, const Vec4f *eye, const Vec4f *at, const Vec4f *up);

void matrix_set_perspective(matrix_t *m, float fovy, float aspect, float zn, float zf);

void camera_update(camera * caneraMain);
void camera_updateShadow(camera * caneraShadow,camera* caneraMain);