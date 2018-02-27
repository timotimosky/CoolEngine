#include "ShadingCalculate.h"

// 计算光向量和法线向量之间角度的余弦  
// 返回0到1之间的值  
float ComputeNDotL(const point_t* vertex, const vector_t* normal, const point_t* lightPosition)
{
    vector_t lightDirection;

	//TODO:这里的如果简单计算夹角，用 灯光位置-物体顶点位置 的矢量   
	//如果是 计算反射，则要用 物体顶点位置-灯光位置 的矢量
	//vector_sub(&lightDirection, vertex , lightPosition);

	vector_sub(&lightDirection, lightPosition, vertex);

	vector_normalize(&lightDirection);

	float dot = vector_dotproduct(normal, &lightDirection);

	//float correctionDot = (dot + 1)*0.5f;
	return CMIDFloat(dot, 0, 1);
}

//计算摄像机到面中心
float ComputeCameraToVertor(point_t* vertex, const vector_t* normal, const point_t *camera_pos)
{
	vector_t  CameraToVertor;

	vector_sub(&CameraToVertor, vertex, camera_pos);

	vector_normalize(&CameraToVertor);

	float dot = vector_dotproduct(normal, &CameraToVertor);

	return dot; 
}

//计算顶点到灯光的距离,用于产生阴影
//float DisVertexToLight(const point_t *Light, vector_t* VertertPosInWorld)
//{
//
//	vector_t dis;
//	vector_sub(&dis, Light, VertertPosInWorld);
//	return vector_length(&dis);
//}


//计算顶点的灯光强度和颜色
float calculateVertexLight(const point_t *v1, vector_t* normal)
{
	vector_normalize(normal);
	//灯光参数，用于乘基本颜色     (0-1)之间 被用作颜色的亮度
	float surfaceLight = ComputeNDotL(v1, normal, &lightPosition); //表面灯光

	return surfaceLight;

}

//三角形是逆时针
//背面剔除 以摄像机为起点
float CullCalcutate(const point_t *v1, const point_t *v2, const point_t *v3, const point_t *camera_pos)
{
	vector_t cross21, cross31, surfaceNormal;
	point_t center;

	vector_sub(&cross21, v2, v3);
	vector_sub(&cross31, v2, v1);
	vector_crossproduct(&surfaceNormal, &cross21, &cross31);	//叉乘方向  在右手坐标中判断叉乘结果的方向使用右手定律，左手坐标系中使用左手。我们的mesh顶点是顺时针
																
	vector_add(&center, v1, v2);
	vector_add(&center, &center, v3);
	vector_scale(&center, 0.333f);

	vector_normalize(&surfaceNormal);

	//剔除参数 
	float cull = ComputeCameraToVertor(&center, &surfaceNormal, camera_pos); //表面灯光

	return cull;

}


//计算平面渲染所需的面法线、光照角度
float calculateGroudShader(const point_t *v1, const point_t *v2, const point_t *v3)
{
	vector_t cross1, cross2, surfaceNormal;
	point_t center;

	//对于平面渲染，才使用面法线，光向量，就是光源位置到面的中心点。

	//对于顶点渲染，没有面法线，三角面内 每个像素，都是由三个顶点法矢量插值来的。

	// 法线面上的向量是该法线面和每个顶点法线面中心点的平均值

	//计算光照效果  在顶点阶段和像素阶段计算都可以。
	//但是计算光照的亮度， 因为 灯光自身是世界坐标，所以在物体转到世界坐标系这个阶段计算是最快的
	vector_sub(&cross1, v2, v3);
	vector_sub(&cross2, v2, v1);
	vector_crossproduct(&surfaceNormal, &cross1, &cross2);	//叉乘方向  在右手坐标中判断叉乘结果的方向使用右手定律，左手坐标系中使用左手。我们的mesh顶点是顺时针

	//面的法线向量 === 3个顶点的法线向量，将他们累加后除以3。
	vector_add(&center, v1, v2);
	vector_add(&center, &center, v3);
	vector_scale(&center, 0.333f);

	vector_normalize(&surfaceNormal);

	//灯光参数，用于乘基本颜色     (0-1)之间 被用作颜色的亮度 
	float surfaceLight = ComputeNDotL(&center, &surfaceNormal, &lightPosition); //表面灯光
	//printf("surfaceLight当前%f", surfaceLight);
	//printf("surfaceLight当前");
	return surfaceLight;

}


//1.理想漫反射

float Lambert(transform_t* mainTrans,  vector_t *v_Obj, vector_t* normal, vector_t* lightPos,color_t diffuseColor, color_t ambientColor)
{

	//将顶点变换到相机透视空间 
	//Output.position = mul(Input.position, matWorldViewProjection);

	//将法线变换到视图空间
	//	Output.normalInView = mul(Input.normal, matWorldView);

	//	// 计算视图空间的灯光方向
	//	Output.lightDirInView = lightPos - mul(Input.position, matWorldView);

	//将顶点变换到视图空间  
	vector_t*  ObjInView;
	transform_apply(mainTrans, v_Obj, ObjInView);


	//将顶点变换到相机透视空间  
	vector_t*  ObjInCVV;
	transform_apply(mainTrans, v_Obj, ObjInCVV);

	vector_t* normalInView;
	//将法线变换到视图空间
	matrix_apply(normalInView, normal, &mainTrans->mv);


	//TODO:这里把灯光转到视图空间

	// 计算视图空间的灯光方向
	vector_t* lightDirInView;

	vector_sub(lightDirInView,lightPos, ObjInView);

	//-----------------像素阶段------------

	//	// 归一化
	//	In.normalInView = normalize(In.normalInView);
	//	In.lightDirInView = normalize(In.lightDirInView);
	//
	//	// 根据Lambert模型，法线点乘入射光方向计算漫反射
	//	float4 diffuse = max(0, dot(In.normalInView, In.lightDirInView));
	//	diffuse = diffuse * diffuseColor;
	//
	//	Out.color = ambientColor + diffuse;


	//根据Lambert模型，法线点乘入射光方向计算漫反射
	float diffuse = max(0,vector_dotproduct(normalInView, lightDirInView));


	//TODO:以后重载操作符
	diffuse = ambientColor + diffuseColor * diffuse;
}


//2.带镜面高光的漫反射
