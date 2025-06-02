#pragma once
#include <windows.h>
#include "renderMath.h"

//点光源
struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	Vec4f ambient;
	Vec4f diffuse;
	Vec4f specular;

	Vec4f position;//光源位置

	Vec4f att;     //衰减系数

	float range;      //光照范围
};

//rgb   和 强度 用于环境光
typedef struct {
	float r, g, b, intension;
	bool shadow;   //是否产生阴影
} Light_t;


//方向光 位置无所谓
typedef struct {
	Vec4f dir;
	Vec4f ambi;
	//color_t ambi;
	//color_t diff;
	//color_t spec;
	bool shadow;
} dirlight_t;
