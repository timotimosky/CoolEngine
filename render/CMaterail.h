#pragma once
#include "objectManager.h"


typedef struct {
	char *name;

	color_t ambient;  //环境光接受度
	color_t diffuse; //漫反射
	color_t specular; //镜面反射
	color_t transmittance; //透明度
	color_t emission; //散射

	float shininess; //光滑度

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


	int cull;  //剔除
} materal;

typedef struct
{

	point_t pos;
	float theta; //暂时只给定一个旋转角度

				 //分别在x,y,z轴上的弧度
	float thetaX;
	float thetaY;
	float thetaZ;

	vertex_t* mesh;
	IUINT32 texture[256][256]; //纹理
	matrix_t model;         // 物体矩阵  由三个弧度计算出来

	materal m_materal; //材质
}
Object_t;