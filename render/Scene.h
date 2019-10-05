#ifndef objectManager_h
#define objectManager_h
#include <vector>
#include "Shader.h"
#include "UObject.h"
#include "Lights.h"
using namespace std;
//用于集成物体的模型、网格、动画、材质、贴图


//储存所有渲染物体
extern vector<Object_t> Scene_render_Objs;


//=====================================================================
// 几何计算：顶点、扫描线、边缘、矩形、步长计算
//=====================================================================

//灯光  点光源  现在计算的是静态光源
extern dirlight_t dirLight;

//环境光，暂时只给一个环境光的强度和颜色
extern Light_t AmbientLight;


//以后给的地面加上缩放
//地面
extern vertex_t ground_mesh[6];


//如何拆面 颜色
extern vertex_t mesh[8];

//rhw  也就是  reciprocal homogeneous w    实际是视图空间的坐标点的的-w的倒数，用于还原Z轴，做深度排序和近大远小



//因为颜色是点，所以w= 1；   normals是向量，所以w =0
extern vertex_t box_mesh[36];


#endif