#include "renderMath.h"




//A坐标系转到B坐标系   任何两个矩阵之间的交换都可以
//欧拉角 根据基向量 转矩阵 采用Y-X-Z轴的顺序 根据矩阵乘法可结合来叠加成一个矩阵
void matrix_World2Obj(matrix_t<4, 4, float>*m, Vec4f rot, Vec4f pos, float scale)
{

	float rot_x = rot.x;
	float rot_y = rot.y;
	float rot_z = rot.z;

	float xOffset = pos.x;
	float yOffest = pos.y;
	float zOffset = pos.z;

	float sinX = (float)sin(-rot_x);
	float sinY = (float)sin(-rot_y);
	float sinZ = (float)sin(-rot_z);

	float cosX = (float)cos(-rot_x);
	float cosY = (float)cos(-rot_y);
	float cosZ = (float)cos(-rot_z);

	(*m)[0][0] = cosX * cosZ;
	(*m)[0][1] = -sinZ;
	(*m)[0][2] = -sinY * cosZ;

	(*m)[1][0] = cosX * cosY* sinZ - sinX * sinY;
	(*m)[1][1] = cosX * cosZ;
	(*m)[1][2] = -cosX * sinY*sinZ - sinX * cosY;

	(*m)[2][0] = sinX * cosY* sinZ + cosX * sinY;
	(*m)[2][1] = sinX * cosZ;
	(*m)[2][2] = -sinX * sinY * sinZ + cosX * cosY;

	//平移
	(*m)[0][3] = 0;
	(*m)[1][3] = 0;
	(*m)[2][3] = 0;


	(*m)[3][0] = 0;
	(*m)[3][1] = 0;
	(*m)[3][2] = 0;
	(*m)[3][3] = 1.0f;
}

	//物体旋转，以自身坐标系旋转。最好在3DMAX里做的物体坐标系跟引擎保持一致
	//A坐标系转到B坐标系   任何两个矩阵之间的交换都可以
	//欧拉角 根据基向量 转矩阵 
	//采用正旋 X-Y-Z轴的顺序， 根据矩阵乘法可结合来叠加成一个矩阵
	//右乘
	void matrix_Obj2World(matrix_t<4, 4, float>*m, Vec4f rot, Vec4f pos)
	{
		float rot_x = rot.x;
		float rot_y = rot.y;
		float rot_z = rot.z;

		float xOffset = pos.x;
		float yOffest = pos.y;
		float zOffset = pos.z;

		float sinX = (float)sin(rot_x);
		float sinY = (float)sin(rot_y);
		float sinZ = (float)sin(rot_z);

		float cosX = (float)cos(rot_x);
		float cosY = (float)cos(rot_y);
		float cosZ = (float)cos(rot_z);

		(*m)[0][0] = cosY * cosZ;
		(*m)[0][1] = cosY * sinZ;
		(*m)[0][2] = -sinY;

		(*m)[1][0] = sinX * sinY* cosZ - cosX * sinZ;
		(*m)[1][1] = (sinX * sinY*sinZ + cosX * cosZ);
		(*m)[1][2] = sinX * cosY;

		(*m)[2][0] = cosX * sinY* cosZ + sinX * sinZ;
		(*m)[2][1] = cosX * sinY* sinZ - sinX * cosZ;
		(*m)[2][2] = (-sinX  + cosX * cosY);

		//平移
		(*m)[0][3] = 0;
		(*m)[1][3] = 0;
		(*m)[2][3] = 0;

		(*m)[3][0] = 0;
		(*m)[3][1] = 0;
		(*m)[3][2] = 0;
		//(*m)[3][0] = xOffset;
		//(*m)[3][1] = yOffest;
		//(*m)[3][2] = zOffset;
		(*m)[3][3] = 1.0f;	
	}


	//四元数转矩阵
	// 这是指定轴旋转
//TODO：加上物体坐标

//坐标系的变化 = 基坐标的位移+坐标系的旋转   坐标系的旋转 跟 这里坐标系内部向量的旋转是一样的
//这个矩阵是基于指定向量旋转的旋转矩阵。 该指定向量为 (x,y,z)


//轴角
void matrix_set_rotate(matrix_t<4, 4, float>*m, float x, float y, float z, float theta, float xOffset,float yOffest, float zOffset)
{
	//三角函数用的是弧度，不是角度
	Vec4f vec = { x, y, z, 1.0f }; //转换为齐次坐标  w=1代表是一个点.   w=0代表是向量.  因为w用于位移,向量位移无意义

	//计算四元数的四个参数之前，先要把旋转轴标准化为单位向量
	vec.normalize();
	//这里是计算出四元数的四个参数
	//设四元数Q（x, y, z, w）表示向量a（xa, ya, za）经过theta角旋转后的结果，则x、y、z和w分别为：
	x = vec.x * (float)sin(theta * 0.5f); //(x,y,z)是一个向量
	y = vec.y * (float)sin(theta * 0.5f);
	z = vec.z * (float)sin(theta * 0.5f);

	// w 是标量
	float w = (float)cos(theta * 0.5f);

	//这里是用四元数来转化出来的矩阵
	(*m)[0][0] = 1 - 2 * y * y - 2 * z * z;
	(*m)[1][0] = 2 * x * y - 2 * w * z;
	(*m)[2][0] = 2 * x * z + 2 * w * y;
	(*m)[0][1] = 2 * x * y + 2 * w * z;
	(*m)[1][1] = 1 - 2 * x * x - 2 * z * z;
	(*m)[2][1] = 2 * y * z - 2 * w * x;
	(*m)[0][2] = 2 * x * z - 2 * w * y;
	(*m)[1][2] = 2 * y * z + 2 * w * x;
	(*m)[2][2] = 1 - 2 * x * x - 2 * y * y;

	(*m)[0][3] = (*m)[1][3] = (*m)[2][3] = 0.0f;

	(*m)[3][0] = xOffset;
	(*m)[3][1] = yOffest;
	(*m)[3][2] = zOffset;


	(*m)[3][3] = 1.0f;
}