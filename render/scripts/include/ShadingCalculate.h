#pragma once
#include "Scene.h"

//导入模型解析

//多种着色方式

//Phong shading
//blinn shading

//Groud Shader

//VertexLight


//计算法线余弦
float ComputeNDotL(const point_t& vertex, const Vec4f& normal, const point_t& lightPosition);

//计算顶点到灯光的距离,用于产生阴影
//float DisVertexToLight(const point_t *Light, Vec4f* VertertPosInWorld);

//顶点
float calculateVertexLight(const point_t *v1,  Vec4f& normal);
//平面
float calculateGroudShader(const point_t *v1, const point_t *v2, const point_t *v3);

float ComputeCameraToVertor(const point_t& vertex, const Vec4f& normal, const point_t& BBBCameraToVertor);

float CullCalcutate(const point_t *v1, const point_t *v2, const point_t *v3, const point_t *camera_pos);

//TODO:增加下列三种光照模型： BRDF模型下的漫反射 、
//1.Lambert模型（理想漫反射模型：各向同性） Idiffuse = Kd*Id*cosθ

//1.顶点Shader
float calculateVertexLight(const point_t *v1, Vec4f& normal);
//2.平面Shader
float calculateGroudShader(const point_t *v1, const point_t *v2, const point_t *v3);

//3.理想漫反射
color_t Lambert(transform_t* mainTrans, Vec4f *v_Obj, Vec4f* normal, Vec4f* lightPos, color_t diffuseColor, color_t ambientColor);

//4.带镜面高光的漫反射 也就是Phong反射模型
color_t Phong(transform_t* mainTrans, Vec4f *posInObj, Vec4f *normal, Vec4f* lightPos, Vec4f* cameraPos, color_t diffuseColor, color_t ambientColor, color_t specularColor);


//5. blinn shading