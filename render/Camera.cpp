#include "Camera.h"





//因为最开始物体坐标系跟世界坐标系重合，物体的运动就是从世界坐标系 转到其他坐标系，符合现实的状态是，先缩放、再旋转，在平移，
//而从其他坐标系转到世界坐标系  则是相反  因为 (T*R)的逆矩阵 = （R的逆矩阵）*（T的逆矩阵） 先平移、再旋转，在缩放，
// 设置摄像机  eye自身坐标 front正前方  up是Y轴
void matrix_set_lookat(matrix_t *m, const vector_t *eye, const vector_t *eyeTarget, const vector_t *up)
{
	//获取摄像机坐标系的3个基向量 因为当前项目设计为Unity的左手坐标系，所有叉乘也遵循左手法则
	vector_t xaxis, yaxis, zaxis;

	//zaxis 摄像机Z轴 朝屏幕内
	vector_sub(&zaxis, eyeTarget, eye); //Z
	vector_normalize(&zaxis);
	vector_crossproduct(&xaxis, up, &zaxis); 
	vector_normalize(&xaxis);
	vector_crossproduct(&yaxis, &zaxis, &xaxis);

	//计算 摄像机相对于世界坐标系的旋转 
	matrix_t rotationM;
	rotationM.m[0][0] = xaxis.x;
	rotationM.m[1][0] = xaxis.y;
	rotationM.m[2][0] = xaxis.z;

	rotationM.m[0][1] = yaxis.x;
	rotationM.m[1][1] = yaxis.y;
	rotationM.m[2][1] = yaxis.z;

	rotationM.m[0][2] = zaxis.x;
	rotationM.m[1][2] = zaxis.y;
	rotationM.m[2][2] = zaxis.z;

	rotationM.m[0][3] = rotationM.m[1][3] = rotationM.m[2][3] = 0.0f;
	rotationM.m[3][0] = rotationM.m[3][1] = rotationM.m[3][2] = 0.0f;
	rotationM.m[3][3] = 1.0f;

	//计算 摄像机相对于世界坐标系的位移
	matrix_t transM;
	matrix_set_identity(&transM);
	transM.m[3][0] = -eye->x;
	transM.m[3][1] = -eye->y;
	transM.m[3][2] = -eye->z;

	*m = transM * rotationM;
	//matrix_mul(m, &transM, &rotationM); //世界-》摄像机。 先平移，再旋转
}


//假设相机坐标系中的一点 P_V, 经过透视变换后为P_H(分量分别为P_Hx, P_Hy, P_Hz, 1)
//P_Hz = -P_Vz  *  (far + near) / (far - near) - 2far*near / (far - near)
//P_Hw = -P_Vz。

//因为经过透视除法后顶点的w分量是1  那么范围是 -1 到1 的x 映射到 0zl 到 1 上 就是 x‘ = x/2 + 0.5 

//透视矩阵  将摄像机坐标系的坐标转换为真正可用的屏幕坐标
//zn 近平面Z
//zf 远平面Z
//	float aspect = (float)width / ((float)height); 分辨率比例
void matrix_set_perspective(matrix_t *m, float fovy, float aspect, float zn, float zf)
{
	float fax = 1.0f / (float)tan(fovy * 0.5f);  //cot@
	matrix_set_zero(m);
	m->m[0][0] = (float)(fax / aspect);
	m->m[1][1] = (float)(fax);
	m->m[2][2] = zf / (zf - zn);
	m->m[3][2] = -zn * zf / (zf - zn);
	m->m[2][3] = 1; //令W=1
}


void CameraInit()
{

	
}

//摄像机位置刷新后,设备的矩阵改变
void camera_update(camera* caneraMain)
{

	matrix_set_perspective(&caneraMain->projection_trans, caneraMain->fov, caneraMain->aspect, caneraMain->zn, caneraMain->zf); //设定近平面为1，这样W取值为1就好了。缩放到投影面比较方便

	//计算 view矩阵 我们计算过matrix_Obj2World,如果我们把camera当作obj， World2view 就是它的逆矩阵 TOdo:以后增加伴随矩阵求逆矩阵
	
	//matrix_World2Obj(&device->transform.view, caneraMain->rotation,caneraMain->pos, 1);

	//vector_t right, eyeTarget, up, front;
	//vector_add(&eyeTarget, &caneraMain->eye, &caneraMain->front);
	//摄像机矩阵 摄像机的位移
	matrix_set_lookat(&caneraMain->view, &(caneraMain->eye), &caneraMain->eyeTarget, &caneraMain->worldup);

}

//平行光=正交矩阵 点光源=透视矩阵
//如果是动态灯光，需要刷新

//TODO：暂时使用主摄像机的fov
void camera_updateShadow(camera * caneraShadow, camera *  camera_main)
{
	matrix_set_perspective(&caneraShadow->projection_trans, camera_main->fov, camera_main->aspect, camera_main->zn, camera_main->zf);
	//摄像机矩阵 摄像机的位移
	matrix_set_lookat(&caneraShadow->view, &(caneraShadow->eye), &caneraShadow->eyeTarget, &caneraShadow->worldup);
}

