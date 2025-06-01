#pragma once
#include <stdbool.h>
#include "renderMath.h"

//extern void matrix_Obj2World(Matrix44f*m, Vec4f rot, Vec4f pos);

typedef enum {
	perspective, //͸��
	orthographic //����
} PROJECTION;

 struct  camera
{
	Vec4f eye; //��ǰ����   //C�Ľṹ����Ļ���Ԫ�ؿ��� �� =��ֵ�������Ͻṹ����
	Vec4f eyeTarget; //����Ľ���

	Matrix44f view_matrix_r;

	Matrix44f view;          // ��Ӱ������任 ���絽���������任����  TODO:�Ժ���뵽ÿ�����������
	Matrix44f projection_trans;    // ͶӰ�任 �����Ǳ任���������Զƽ��-��ƽ��ռ���  TODO:�Ժ���뵽ÿ�����������
	//����������ľ��루z��Ҳ����ˣ����ھ�������x��y������������㣬�������z����Ķ��㽫����Ļ�����ĸ����λ����Ļ�����ġ�     
	// ���⣬�����͸����ͼ��W�������������С����ģ���Ӿ���Զ���������С

	//��Unity������ʹ��Transform�����Rotation
	Vec4f rotation;

	Vec4f front; //��ǰ����
	Vec4f worldup;  //����

	//���
	int width;
	int height;

	//Maya Camera��Angle of View��Horizontal FOV�ĽǶ�
	//Unity Camera��FOV��Vertical FOV�ĽǶ�
	float fov;  //fov����ʱ���ֱ������ţ�����������
	float zn; //������
	float zf; //Զ����

	//��������
	float left;
	float right;
	float bottom;
	float top;

	bool dirty; //�Ƿ����
	PROJECTION projection;  //�����ȡ������
	bool main;  //�Ƿ����������

	// private
	float aspect; //�ݺ�ȣ������������ͷ����ȫ�� = ��Ļ�߿��
};


void CameraInit();

void matrix_set_lookat(Matrix44f*m, const Vec4f *eye, const Vec4f *at, const Vec4f *up);

void matrix_set_perspective(Matrix44f*m, float fovy, float aspect, float zn, float zf);

void camera_update(camera * caneraMain);
void camera_updateShadow(camera * caneraShadow,camera* caneraMain);