#pragma once
#include "Shader.h"


typedef struct {
	char *name;

	color_t ambient;  //��������ܶ�
	color_t diffuse; //������
	color_t specular; //���淴��
	color_t transmittance; //͸����
	color_t emission; //ɢ��

	float shininess; //�⻬��

	char *ambient_texname;            /* map_Ka */
	int ambient_tex_id;
	char *diffuse_texname;            /* map_Kd */
	int diffuse_tex_id;
	char *specular_texname;           /* map_Ks */
	int specular_tex_id;
	char *specular_highlight_texname; /* map_Ns */
	int specular_highlight_tex_id;
	char *bump_texname;               /* map_bump, bump */
	int bump_tex_id;
	char *displacement_texname;       /* disp */
	int displacement_tex_id;
	char *alpha_texname;              /* map_d */
	int alpha_tex_id;
} materal;
