#pragma once
#include <stdbool.h>
#include "shaderStruct.h"
#include "CMaterail.h"


//�����ص�˵�� pos��Wֵ�� ��color_t��Wֵ������
//pos������ϵת����Ϊ��ͳһת������չ������ά�� pos��Wֵ�����������4ά��3ά�Ļ�ԭ��Ҳ����CVV����դ���Ľ׶�
//color_t��Wֵ�������ڼ�¼�ö���/���ص����ȣ���������޳�

typedef struct {
	point_t pos;//λ��

	texcoord_t tc; //��������
	color_t color; //��ɫ       
	vector_t normal; //���Ĵ������ӷ���    ������ϲ� float rhw �ϲ���;  //�ö����֮ǰ��w�ĵ����� Ҳ�������Ŵ�С
	point_t worldPos;  //֮ǰ�����������λ�� �����shader�в���Ҫ
	float vertexLight; //����ƹ��ǿ��
	point_t shadowPos; //ͶӰ������Ĺ�դ��λ�� �����shader�в���Ҫ
} vertex_t; //����

			//���������εı�Ե
typedef struct {
	vertex_t v,  //ɨ���� �� б�ߵĽ���
		v1, //������б�ߵ�ԭʼ���
		v2; //������б�ߵ�ԭʼ�յ�
} edge_t; //��Ե

		  //������
typedef struct {
	float top, bottom; //�����ε�����
	edge_t left, right;  //�����ε�����б��  ��ʵ �����εĸ߶ȣ��Ѿ�ȷ����

} trapezoid_t; //����

			   //��Ⱦ���ε�ʱ���ɨ����
typedef struct {
	vertex_t v, step; //���
	int x,
		y,
		width;  //���͵�ɨ���߿���
} scanline_t; //ɨ����


void vertex_add(vertex_t *y, const vertex_t *x);


//����
typedef struct {
	unsigned long mesh_num;
	int* material_ids; //ֻ�洢ID����
	int texture_id;
	bool shadow;  //�Ƿ���������Ӱ  TODO���Ժ�����Ƿ������Ӱ

	bool dirty;
	point_t pos; //λ��
	vector_t scale; //����
	vector_t axis;
	matrix_t model2View;

	//ҲҲ����������
	//struct
	//{
		//int data;   // ������
		//struct node *next;   // ָ����
	//} *node;

	//mesh[0]��mesh[]��ռ�ÿռ䣬�ҵ�ַ�����ڽṹ���棬��vertex_t *data��Ϊָ�룬ռ��4���ֽڣ���ַ���ڽṹ֮��
	vertex_t mesh[0]; //��ʼ��ַ �ɱ�����   vertex_t data[0]��vertex_t *data, vertex_t data[]������ 
} object_t;


typedef struct
{
	unsigned long mesh_num;//..

	point_t pos;
	vector_t axis;
	float scale;
	vertex_t* mesh;

	IUINT32 texture[256][256]; //����
	matrix_t model;         // �������  ���������ȼ������
	materal m_materal; //����
}
Object_t;