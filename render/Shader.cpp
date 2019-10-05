#include "Shader.h"
#include "UObject.h"
#include "ShadingCalculate.h"
#include "Pipline.h"



//IShader::~IShader() {}

//为了使用GPU的并行运算

//对顶点执行 
//1.坐标变换（透视投影）  
//2.逐顶点光照计算、计算动画角色的蒙皮、纹理计算、变形
//1.物体坐标系到世界坐标系的转换
//2.CVV空间裁剪(视锥裁剪)
//2.初始化： uv的赋值


//输出：完成变换以及光照的顶点，其位置和法向量是以齐次坐标空间表示的

//返回 Vec4f ：齐次坐标
Vec3f Shader::vertex(Vec3f iface, int nthvert) {
	Vec3f gl_Vertex = iface;

	return gl_Vertex;
}

//中间光栅化

//像素阶段，输入顶点
bool Shader::fragment(Vec3f bar, TGAColor& color) {




	return false;
}