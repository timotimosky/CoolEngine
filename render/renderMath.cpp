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

// | v |
float vector_length(const vector_t *v) {
	float sq = v->x * v->x + v->y * v->y + v->z * v->z;
	return (float)sqrt(sq);
}

//齐次坐标的第二个功效就是区分向量跟点.  w为1则为点.  w为0则为向量。  因为w作用是位移，向量位移没什么意义。
// z = x + y  w一直为1
void vector_add(vector_t *z, const vector_t *x, const vector_t *y)
{
	z->x = x->x + y->x;
	z->y = x->y + y->y;
	z->z = x->z + y->z;
	z->w = 1.0;
}

// z = x - y
void vector_sub(vector_t *z, const vector_t *x, const vector_t *y)
{
	z->x = x->x - y->x;
	z->y = x->y - y->y;
	z->z = x->z - y->z;
	z->w = 1.0;
}


void vector_scale(vector_t *z, float scale)
{
	z->x = z->x * scale;
	z->y = z->y * scale;
	z->z = z->z * scale;
}

// 矢量点乘
float vector_dotproduct(const vector_t *x, const vector_t *y)
{
	return x->x * y->x + x->y * y->y + x->z * y->z;
}



// 矢量叉乘  如果 x y 标准化，那算出来的叉乘Z也是标准化的xy平面的法线。 否则的话，要标准化一次Z
void vector_crossproduct(vector_t *z, const vector_t *x, const vector_t *y)
{

	z->x = x->y * y->z - x->z * y->y;
	z->y = x->z * y->x - x->x * y->z;
	z->z = x->x * y->y - x->y * y->x;
	z->w = 1.0f;
}

// 矢量lerp插值，t取值 [0, 1]
void vector_interp(vector_t *z, const vector_t *x1, const vector_t *x2, float t)
{
	z->x = interp(x1->x, x2->x, t);
	z->y = interp(x1->y, x2->y, t);
	z->z = interp(x1->z, x2->z, t);
	z->w = interp(x1->w, x2->w, t);//这是深度插值。  非线性插值，用的 1/Z
}

// 矢量归一化  注意归一化，不涉及W。 只有四维向三维投影的时候，W归一化才有用
void vector_normalize(vector_t *v)
{
	float length = vector_length(v);
	if (length != 0.0f) {
		float inv = 1.0f / length;
		v->x *= inv;
		v->y *= inv;
		v->z *= inv;
	}
}

// c = a + b
void matrix_add(matrix_t *c, const matrix_t *a, const matrix_t *b)
{
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			c->m[i][j] = a->m[i][j] + b->m[i][j];
	}
}

// c = a - b
void matrix_sub(matrix_t *c, const matrix_t *a, const matrix_t *b) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			c->m[i][j] = a->m[i][j] - b->m[i][j];
	}
}

//矩阵乘法：   z.m[j][i] = (a->m[j][0] * b->m[0][i]) +(a->m[j][1] * b->m[1][i]) +(a->m[j][2] * b->m[2][i]) +(a->m[j][3] * b->m[3][i]);
// c = a * b
void matrix_mul(matrix_t *c, const matrix_t *a, const matrix_t *b) {
	matrix_t z;
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			z.m[j][i] = (a->m[j][0] * b->m[0][i]) +
				(a->m[j][1] * b->m[1][i]) +
				(a->m[j][2] * b->m[2][i]) +
				(a->m[j][3] * b->m[3][i]);
		}
	}

	//c++赋值, 只需要拿0索引，则拿到他的指针
	c[0] = z;
}

// c = a * f
void matrix_scale(matrix_t *c, const matrix_t *a, float f) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			c->m[i][j] = a->m[i][j] * f;
	}
}

//TODO：以后改左乘
// 矩阵乘法：   y = x * m   	//向量/点，被转为矩阵后, 矩阵 乘 矩阵.  实现坐标转换
// 用的右乘 4-4* 4-1矩阵
void matrix_apply(vector_t *y, const vector_t *x, const matrix_t *m) {
	float X = x->x, Y = x->y, Z = x->z, W = x->w;
	//A的第一行  乘 B第一列的相加
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



// 平移变换
void matrix_set_translate(matrix_t *m, float x, float y, float z) {
	matrix_set_identity(m);
	m->m[3][0] = x;
	m->m[3][1] = y;
	m->m[3][2] = z;
}

// 缩放变换
void matrix_set_scale(matrix_t *m, float x, float y, float z) {
	matrix_set_identity(m);
	m->m[0][0] = x;
	m->m[1][1] = y;
	m->m[2][2] = z;
}

	//欧拉角转矩阵 采用Y-X-Z轴的顺序 根据矩阵乘法可结合来叠加成一个矩阵
	//默认的物体空间是跟世界空间一致的。所以物体到世界矩阵，就是根据物体当下的旋转和位移来反推。
	void matrix_Obj2World(matrix_t *m, float rot_x, float rot_y, float rot_z, float xOffset, float yOffest, float zOffset)
	{
		float x;
		float y;
		float z;
		float a;
		float b;
		float c;
		x = (float)sin(rot_x); //(x,y,z)是一个向量 
		y = (float)sin(rot_y);
		z = (float)sin(rot_z);

		a = (float)cos(rot_x); //(x,y,z)是一个向量
		b = (float)cos(rot_y);
		c = (float)cos(rot_z);

		//因为其他地方用的行向量 * 列矩阵 所以这里矩阵也按列写

		//旋转顺序为Z-Y-X
		//m->m[0][0] = b * c;
		//m->m[0][1] = -z*b;
		//m->m[0][2] = y;

		//m->m[1][0] = z*a-x*y*c;
		//m->m[1][1] = a*c+x*y*z;
		//m->m[1][2] = -x*b;

		//m->m[2][0] = x*z + y*c*b;
		//m->m[2][1] = x*c - a*y*z;
		//m->m[2][2] = a*b;

		//旋转顺序为Y-Z-X
		m->m[0][0] = b * c + x*y*z;
		m->m[0][1] = -z * b +x*y*c;
		m->m[0][2] = y*a;

		m->m[1][0] = z * a;
		m->m[1][1] = a*c;
		m->m[1][2] = -x;

		m->m[2][0] = -y * c + z * x*b;
		m->m[2][1] = z * y + b*x*c;
		m->m[2][2] = a * b;


		m->m[0][3] = m->m[1][3] = m->m[2][3] = 0.0f;
		//m->m[3][0] = m->m[3][1] = m->m[3][2] = 0.0f;
		m->m[3][0] = xOffset;
		m->m[3][1] = yOffest;
		m->m[3][2] = zOffset;

		m->m[3][3] = 1.0f;


		//m->m[0][0] = b * c;
		//m->m[1][0] = -z*b;
		//m->m[2][0] = y;

		//m->m[0][1] = z*a-x*y*c;
		//m->m[1][1] = a*c+x*y*z;
		//m->m[2][1] = -x*b;

		//m->m[0][2] = x*z + y*c*b;
		//m->m[1][2] = x*c - a*y*z;
		//m->m[2][2] = a*b;





		//m->m[0][0] = b * c + x * y*z;
		//m->m[1][0] = -z*b;
		//m->m[2][0] = y;

		//m->m[0][1] = z*a-x*y*c;
		//m->m[1][1] = a*c+x*y*z;
		//m->m[2][1] = -x*b;

		//m->m[0][2] = x*z + y*c*b;
		//m->m[1][2] = x*c - a*y*z;
		//m->m[2][2] = a*b;
	
	}


	//四元数转矩阵
	// 这是指定轴旋转
//TODO：加上物体坐标

//坐标系的变化 = 基坐标的位移+坐标系的旋转   坐标系的旋转 跟 这里坐标系内部向量的旋转是一样的
//这个矩阵是基于指定向量旋转的旋转矩阵。 该指定向量为 (x,y,z)
void matrix_set_rotate(matrix_t *m, float x, float y, float z, float theta, float xOffset,float yOffest, float zOffset)
{
	//三角函数用的是弧度，不是角度
	vector_t vec = { x, y, z, 1.0f }; //转换为齐次坐标  w=1代表是一个点.   w=0代表是向量.  因为w用于位移,向量位移无意义

									  //计算四元数的四个参数之前，先要把旋转轴标准化为单位向量
	vector_normalize(&vec);

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
	//m->m[3][0] = m->m[3][1] = m->m[3][2] = 0.0f;
	m->m[3][0] = xOffset;
	m->m[3][1] = yOffest;
	m->m[3][2] = zOffset;

	m->m[3][3] = 1.0f;
}