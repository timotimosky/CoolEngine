#pragma once
#include <windows.h>
#include "renderMath.h"

//���Դ
struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	Vec4f ambient;
	Vec4f diffuse;
	Vec4f specular;

	Vec4f position;//��Դλ��

	Vec4f att;     //˥��ϵ��

	float range;      //���շ�Χ
};

//rgb   �� ǿ�� ���ڻ�����
typedef struct {
	float r, g, b, intension;
	bool shadow;   //�Ƿ������Ӱ
} Light_t;


//����� λ������ν
typedef struct {
	Vec4f dir;
	Vec4f ambi;
	//color_t ambi;
	//color_t diff;
	//color_t spec;
	bool shadow;
} dirlight_t;
