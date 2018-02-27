#pragma once
#include "objectManager.h"
//多种着色方式

//Phong shading
//blinn shading

//Groud Shader

//VertexLight


//计算法线余弦
float ComputeNDotL(const point_t* vertex, const vector_t* normal, const point_t* lightPosition);

//计算顶点到灯光的距离,用于产生阴影
//float DisVertexToLight(const point_t *Light, vector_t* VertertPosInWorld);

//顶点
float calculateVertexLight(const point_t *v1,  vector_t* normal);
//平面
float calculateGroudShader(const point_t *v1, const point_t *v2, const point_t *v3);

float ComputeCameraToVertor(point_t* vertex, const vector_t* normal, const point_t* BBBCameraToVertor);

float CullCalcutate(const point_t *v1, const point_t *v2, const point_t *v3, const point_t *camera_pos);