#include "Camera.h"

//TODO：这里拿到的 貌似不是真正的世界坐标转摄像机矩阵，拿到的是 此刻 转置矩阵 =逆矩阵
// 设置摄像机  eye自身坐标 front正前方  up是Y轴
void matrix_set_lookat(matrix_t *m, const vector_t *eye, const vector_t *at, const vector_t *up)
{
	vector_t xaxis, yaxis, zaxis;

	//zaxis 摄像机Z轴
	vector_sub(&zaxis, at, eye); //Z

	//叉乘之前要归一化。 
	vector_normalize(&zaxis);
	vector_crossproduct(&xaxis, up, &zaxis);  //叉乘得到 X轴

	vector_normalize(&xaxis);
	vector_crossproduct(&yaxis, &zaxis, &xaxis);  //up只是单纯的设置为 朝上。与目光看向焦点 构成一个平面，来运算出X轴， 然后这里还要算出一次真正的Y轴

	//计算 摄像机相对于世界坐标系的旋转 

	//由于是要从世界转摄像机，所以是-号
	float cos_rot_x  = -vector_dotproduct(&xaxis, eye);
	float cos_rot_y = -vector_dotproduct(&yaxis, eye);
	float cos_rot_z = -vector_dotproduct(&zaxis, eye);

	//matrix_Obj2World(m,  vector_t(cos_rot_x, cos_rot_y, cos_rot_z, 0), *eye, 1);

	//return;
//点积不仅能表示两个向量的角度范围 , 而且可以表示在标准化坐标系内的X , Y ,Z 轴上的对应位置 

//摄像机坐标系 1.整体平移，将摄像机平移至世界坐标系原点，2.将坐标点从世界坐标系转换至摄像机坐标系。
//使用单位向量U, V, W分别代表摄像机坐标系X, Y, Z轴正向的单位向量在世界坐标系中的表示，则在摄像相机坐标系与世界坐标系原点重合的情况下，物体顶点坐标代表的向量（即从世界原点指向物体顶点的向量）
//在U, V, W上的投影大小即是物体顶点在摄像机坐标系下的坐标值。因为U, V, W是单位向量，使用二者的点乘即可以得到顶点的投影大小。
//P  =  (ax,by,cz)  a b c是已知数
//转换到另一个坐标系，为  PN = (mU, nV,qP)

//世界转视图空间矩阵

//float m[4][4] = { { xaxis.x,xaxis.y,xaxis.z,0 },{ V.x,V.y,V.z,0 },{ W.x,W.y,W.z,0 },{ 0,0,0,1 } };


//矩阵乘法 =  被变换的向量  点积   矩阵的对应的那一列标识的向量（如果是行变化，也就是左乘，就是每一列）

 //其实到时候矩阵效果就是  Px * xaxis = newPx;  原本在世界坐标系的X坐标，乘以xaxis，获得在摄像机基坐标x轴上的投影， 也就是在摄像机坐标系里的新X坐标了
 //右乘。考虑矩阵的意义，如果是斜线才有值的矩阵，从行变换来考虑。  下面这种，用列变换考虑比较好。
	m->m[0][0] = xaxis.x;
	m->m[1][0] = xaxis.y;
	m->m[2][0] = xaxis.z;
	m->m[3][0] = -vector_dotproduct(&xaxis, eye);  //cos

												   //这三行是 摄像机坐标系的Y轴的基向量
												   //到时候矩阵运算会跟世界坐标的Y坐标点乘,  拿到的也就是世界坐标系里的点，在 摄像机坐标系 X轴上的投影
	m->m[0][1] = yaxis.x;
	m->m[1][1] = yaxis.y;
	m->m[2][1] = yaxis.z;

	//因为摄像机的基坐标平移了，所以点也要跟着平移。
	m->m[3][1] = -vector_dotproduct(&yaxis, eye);

	m->m[0][2] = zaxis.x;
	m->m[1][2] = zaxis.y;
	m->m[2][2] = zaxis.z;
	m->m[3][2] = -vector_dotproduct(&zaxis, eye);//这里是填写的 平移变化的基坐标

	m->m[0][3] = m->m[1][3] = m->m[2][3] = 0.0f;
	m->m[3][3] = 1.0f;
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

	matrix_set_perspective(&(&caneraMain->transform)->projection, caneraMain->fov, caneraMain->aspect, caneraMain->zn, caneraMain->zf); //设定近平面为1，这样W取值为1就好了。缩放到投影面比较方便

	//计算 view矩阵 我们计算过matrix_Obj2World,如果我们把camera当作obj， World2view 就是它的逆矩阵 TOdo:以后增加伴随矩阵求逆矩阵
	
	//matrix_World2Obj(&device->transform.view, caneraMain->rotation,caneraMain->pos, 1);

	vector_t right, at, up, front;
	vector_add(&at, &caneraMain->pos, &caneraMain->front);
	//摄像机矩阵 摄像机的位移
	matrix_set_lookat(&(&caneraMain->transform)->view, &(caneraMain->pos), &at, &caneraMain->worldup);

}


//如果是动态灯光，需要刷新
void camera_updateShadow(camera * caneraShadow)
{
	//matrix_set_perspective(&(&device->transform)->projection, camera_main.fov, camera_main.aspect, camera_main.zn, camera_main.zf); //设定近平面为1，这样W取值为1就好了。缩放到投影面比较方便

	//摄像机矩阵 摄像机的位移
	matrix_set_lookat(&caneraShadow->transform.view, &(caneraShadow->eye), &caneraShadow->front, &caneraShadow->worldup);
}

