#include "renderMath.h"


//不允许超过上下限 
float CMIDFloat(float x, float min, float max)
{
	return (x < min) ? min : ((x > max) ? max : x);
}


//不允许超过上下限 int
int CMID(int x, int min, int max)
{
	return (x < min) ? min : ((x > max) ? max : x);
}

// 计算插值：t 为 [0, 1] 之间的数值 lerp插值
float interp(float x1, float x2, float t)
{
	return x1 + (x2 - x1) * t;
}

// 矢量lerp插值，t取值 [0, 1]
void vector_interp(Vec4f *z, const Vec4f *x1, const Vec4f *x2, float t)
{
	z->x = interp(x1->x, x2->x, t);
	z->y = interp(x1->y, x2->y, t);
	z->z = interp(x1->z, x2->z, t);
	z->w = interp(x1->w, x2->w, t);//这是深度插值。  非线性插值，用的 1/Z
}
// c = a * b
//void matrix_mul(matrix_t *c, const matrix_t *left , const matrix_t *right) {         
//	int i, j;
//	for (i = 0; i < 4; i++)
//	{
//		for (j = 0; j < 4; j++)
//		{
//			c->m[i][j] =
//				(left->m[i][0] * right->m[0][j]) +
//				(left->m[i][1] * right->m[1][j]) +
//				(left->m[i][2] * right->m[2][j]) +
//				(left->m[i][3] * right->m[3][j]);
//		}
//	}
//}

// c = a * f
//void matrix_scale(matrix_t *c, const matrix_t *a, float f) {
//	int i, j;
//	for (i = 0; i < 4; i++) {
//		for (j = 0; j < 4; j++)
//			c->m[i][j] = a->m[i][j] * f;
//	}
//}

//向量 右乘矩阵 
void matrix_apply(Vec4f *y, const Vec4f *x, const matrix_t *m) {
	float X = x->x, Y = x->y, Z = x->z, W = x->w;
	y->x = X * m->m[0][0] + Y * m->m[1][0] + Z * m->m[2][0] + W * m->m[3][0];
	y->y = X * m->m[0][1] + Y * m->m[1][1] + Z * m->m[2][1] + W * m->m[3][1];
	y->z = X * m->m[0][2] + Y * m->m[1][2] + Z * m->m[2][2] + W * m->m[3][2];
	y->w = X * m->m[0][3] + Y * m->m[1][3] + Z * m->m[2][3] + W * m->m[3][3];
}

//标准矩阵 4X4 
void matrix_set_identity(matrix_t *m) {
	m->m[0][0] = m->m[1][1] = m->m[2][2] = m->m[3][3] = 1.0f;
	m->m[0][1] = m->m[0][2] = m->m[0][3] = 0.0f;
	m->m[1][0] = m->m[1][2] = m->m[1][3] = 0.0f;
	m->m[2][0] = m->m[2][1] = m->m[2][3] = 0.0f;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = 0.0f;
}


//零矩阵
void matrix_set_zero(matrix_t *m) {
	m->m[0][0] = m->m[0][1] = m->m[0][2] = m->m[0][3] = 0.0f;
	m->m[1][0] = m->m[1][1] = m->m[1][2] = m->m[1][3] = 0.0f;
	m->m[2][0] = m->m[2][1] = m->m[2][2] = m->m[2][3] = 0.0f;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = m->m[3][3] = 0.0f;
}



// 平移变换 左乘
void matrix_set_translate(matrix_t *m, float x, float y, float z) {
	matrix_set_identity(m);
	m->m[0][3] = x;
	m->m[1][3] = y;
	m->m[2][3] = z;
}

// 缩放变换
void matrix_set_scale(matrix_t *m, float x, float y, float z) {
	matrix_set_identity(m);
	m->m[0][0] = x;
	m->m[1][1] = y;
	m->m[2][2] = z;
}


//A坐标系转到B坐标系   任何两个矩阵之间的交换都可以
//欧拉角 根据基向量 转矩阵 采用Y-X-Z轴的顺序 根据矩阵乘法可结合来叠加成一个矩阵
void matrix_World2Obj(matrix_t *m, Vec4f rot, Vec4f pos, float scale)
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

	m->m[0][0] = cosX * cosZ;
	m->m[0][1] = -sinZ;
	m->m[0][2] = -sinY * cosZ;

	m->m[1][0] = cosX * cosY* sinZ - sinX * sinY;
	m->m[1][1] = cosX * cosZ;
	m->m[1][2] = -cosX * sinY*sinZ - sinX * cosY;

	m->m[2][0] = sinX * cosY* sinZ + cosX * sinY;
	m->m[2][1] = sinX * cosZ;
	m->m[2][2] = -sinX * sinY * sinZ + cosX * cosY;

	//平移
	m->m[0][3] = 0;
	m->m[1][3] = 0;
	m->m[2][3] = 0;


	m->m[3][0] = 0;
	m->m[3][1] = 0;
	m->m[3][2] = 0;
	m->m[3][3] = 1.0f;
}

	//物体旋转，以自身坐标系旋转。最好在3DMAX里做的物体坐标系跟引擎保持一致
	//A坐标系转到B坐标系   任何两个矩阵之间的交换都可以
	//欧拉角 根据基向量 转矩阵 
	//采用正旋 X-Y-Z轴的顺序， 根据矩阵乘法可结合来叠加成一个矩阵
	//右乘
	void matrix_Obj2World(matrix_t *m, Vec4f rot, Vec4f pos)
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

		m->m[0][0] = cosY * cosZ;
		m->m[0][1] = cosY * sinZ;
		m->m[0][2] = -sinY;

		m->m[1][0] = sinX * sinY* cosZ - cosX * sinZ;
		m->m[1][1] = (sinX * sinY*sinZ + cosX * cosZ);
		m->m[1][2] = sinX * cosY;

		m->m[2][0] = cosX * sinY* cosZ + sinX * sinZ;
		m->m[2][1] = cosX * sinY* sinZ - sinX * cosZ;
		m->m[2][2] = (-sinX  + cosX * cosY);

		//平移
		m->m[0][3] = 0;
		m->m[1][3] = 0;
		m->m[2][3] = 0;

		m->m[3][0] = 0;
		m->m[3][1] = 0;
		m->m[3][2] = 0;
		//m->m[3][0] = xOffset;
		//m->m[3][1] = yOffest;
		//m->m[3][2] = zOffset;
		m->m[3][3] = 1.0f;	
	}


	//四元数转矩阵
	// 这是指定轴旋转
//TODO：加上物体坐标

//坐标系的变化 = 基坐标的位移+坐标系的旋转   坐标系的旋转 跟 这里坐标系内部向量的旋转是一样的
//这个矩阵是基于指定向量旋转的旋转矩阵。 该指定向量为 (x,y,z)


//轴角
void matrix_set_rotate(matrix_t *m, float x, float y, float z, float theta, float xOffset,float yOffest, float zOffset)
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
	m->m[0][0] = 1 - 2 * y * y - 2 * z * z;
	m->m[1][0] = 2 * x * y - 2 * w * z;
	m->m[2][0] = 2 * x * z + 2 * w * y;
	m->m[0][1] = 2 * x * y + 2 * w * z;
	m->m[1][1] = 1 - 2 * x * x - 2 * z * z;
	m->m[2][1] = 2 * y * z - 2 * w * x;
	m->m[0][2] = 2 * x * z - 2 * w * y;
	m->m[1][2] = 2 * y * z + 2 * w * x;
	m->m[2][2] = 1 - 2 * x * x - 2 * y * y;

	m->m[0][3] = m->m[1][3] = m->m[2][3] = 0.0f;

	m->m[3][0] = xOffset;
	m->m[3][1] = yOffest;
	m->m[3][2] = zOffset;


	m->m[3][3] = 1.0f;
}