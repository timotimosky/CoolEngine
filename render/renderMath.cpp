#include "renderMath.h"


//结论是“行向量左乘矩阵时，结果是行向量，列向量右乘矩阵时，结果是列向量，反过来是不行的”，在DirectX中使用的是行向量，在OpenGL中使用的是列向量。
//这里用的右乘

// w 也要参与到坐标变换的计算  到时候在CVV的时候， x,y,z,w要除以W，此刻 w=1,投影到屏幕坐标了
//得到在屏幕上的坐标. (假设屏幕的Z值是1)

//TODO:需要拆分为 按帧更新、按物体更新
// 矩阵更新
void transform_update(transform_t* ts) {

	ts->mv = ts->model * ts->view;
	ts->vp = ts->view * ts->projection;
	ts->mvp = ts->mv * ts->projection;

}

Matrix44f& Init_Model_matrix(point_t& pos, Vec4f axis, Vec4f scale)
{
	//输入 当前物体原点在世界坐标系中的位置和旋转，  来反推世界矩阵
	//axis.x 绕X轴的旋转角度

	//物体转世界坐标系  	//平移-> 旋转-》缩放 
	Matrix44f s = matrix_set_scale(scale);
	Matrix44f r;
	Matrix44f t = Matrix44f().identity();

	matrix_Obj2World(&r, axis, pos);

	t[3][0] = pos.x;
	t[3][1] = pos.y;
	t[3][2] = pos.z;

	return s * r * t;
}


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
void matrix_set_rotate(matrix_t<4, 4, float>*m, Vec4f vec, float theta, Vec3f offset)
{
	//三角函数用的是弧度，不是角度
	//转换为齐次坐标  w=1代表是一个点.   w=0代表是向量.  因为w用于位移,向量位移无意义
	vec.w = 1.0f;
	//计算四元数的四个参数之前，先要把旋转轴标准化为单位向量
	vec.normalize();

	//这里是计算出四元数的四个参数
	//设四元数Q（x, y, z, w）表示向量a（xa, ya, za）经过theta角旋转后的结果，则x、y、z和w分别为：
	float x = vec.x * (float)sin(theta * 0.5f); //(x,y,z)是一个向量
	float y = vec.y * (float)sin(theta * 0.5f);
	float z = vec.z * (float)sin(theta * 0.5f);

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

	(*m)[3][0] = offset.x;
	(*m)[3][1] = offset.y;
	(*m)[3][2] = offset.z;


	(*m)[3][3] = 1.0f;
}