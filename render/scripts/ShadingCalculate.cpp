#include "include\ShadingCalculate.h"

// 计算光向量和法线向量之间角度的余弦  
// 返回0到1之间的值  
float ComputeNDotL(const point_t& vertex, const Vec4f& normal, const point_t& lightPosition)
{
	//TODO:这里的如果简单计算夹角，用 灯光位置-物体顶点位置 的矢量   
	//如果是 计算反射，则要用 物体顶点位置-灯光位置 的矢量
	//vector_sub(&lightDirection, vertex , lightPosition);

	Vec4f lightDirection = lightPosition - vertex;
	lightDirection.normalize();

	float dot = normal * lightDirection;

	//float correctionDot = (dot + 1)*0.5f;
	return CMID(dot, 0.0f, 1.0f);
}

//计算面中心到摄像机
float ComputeCameraToVertor(const point_t& vertex, const Vec4f& normal, const point_t& camera_pos)
{
	Vec4f  CameraToVertor=  camera_pos- vertex;
	CameraToVertor.normalize();

	float dot = normal * CameraToVertor;

	return dot; 
}

//计算顶点到灯光的距离,用于产生阴影
//float DisVertexToLight(const point_t *Light, Vec4f* VertertPosInWorld)
//{
//
//	Vec4f dis;
//	vector_sub(&dis, Light, VertertPosInWorld);
//	return vector_length(&dis);
//}


//计算顶点的灯光强度和颜色
float calculateVertexLight(const point_t *v1, Vec4f& normal)
{
	normal.normalize();
	//灯光参数，用于乘基本颜色     (0-1)之间 被用作颜色的亮度
	float surfaceLight = ComputeNDotL(*v1, normal, dirLight.dir); //表面灯光

	return surfaceLight;

}

//背面剔除 以摄像机为起点
float CullCalcutate(const point_t *v1, const point_t *v2, const point_t *v3, const point_t *camera_pos)
{
	Vec4f cross21, cross31, surfaceNormal;

	cross21 = *v2 - *v3;
	cross31 = *v2 - *v1;
	surfaceNormal = cross(cross21, cross31);
	//叉乘方向  在右手坐标中判断叉乘结果的方向使用右手定律，左手坐标系中使用左手。我们的mesh顶点是顺时针
	
	point_t center;
	center= *v1+ *v2 + *v3;
	center = center * 0.333f;

	surfaceNormal.normalize();

	//剔除参数 
	float cull = ComputeCameraToVertor(center, surfaceNormal, *camera_pos); 

	return cull;
}


//计算平面渲染所需的面法线、光照角度
float calculateGroudShader(const point_t *v1, const point_t *v2, const point_t *v3)
{
	Vec4f cross1, cross2, surfaceNormal;
	point_t center;

	//对于平面渲染，才使用面法线，光向量，就是光源位置到面的中心点。

	//对于顶点渲染，没有面法线，三角面内 每个像素，都是由三个顶点法矢量插值来的。

	// 法线面上的向量是该法线面和每个顶点法线面中心点的平均值

	//计算光照效果  在顶点阶段和像素阶段计算都可以。
	//但是计算光照的亮度， 因为 灯光自身是世界坐标，所以在物体转到世界坐标系这个阶段计算是最快的

	cross1 = *v2 - *v3;
	cross2 = *v2 - *v1;
	surfaceNormal = cross(cross1, cross2);
	//叉乘方向  在右手坐标中判断叉乘结果的方向使用右手定律，左手坐标系中使用左手。我们的mesh顶点是顺时针

	//面的法线向量 === 3个顶点的法线向量，将他们累加后除以3。
	center = *v1 + *v2 + *v3;
	center = center * 0.333f;
	surfaceNormal.normalize();

	//灯光参数，用于乘基本颜色     (0-1)之间 被用作颜色的亮度 
	float surfaceLight = ComputeNDotL(center, surfaceNormal, dirLight.dir); //表面灯光
	//printf("surfaceLight当前%f", surfaceLight);
	return surfaceLight;

}


//1.理想漫反射:返回该顶点的颜色

color_t Lambert(transform_t* mainTrans, Vec4f *v_Obj, Vec4f* normal, Vec4f* lightPos, color_t diffuseColor, color_t ambientColor)
{
	//将顶点变换到视图空间  
	Vec4f  ObjInView;
	cross(ObjInView,(*v_Obj) , mainTrans->mv);

	//将顶点变换到相机透视空间  
	Vec4f  ObjInCVV;
		//= (*v_Obj) * mainTrans->mvp;
	cross(ObjInCVV, (*v_Obj), mainTrans->mvp);
	//Vec4f* normalInView = NULL;
	//将法线变换到视图空间
	//matrix_apply(normalInView, normal, &mainTrans->mv);

	Vec4f normalInView;
		//= (*normal) * mainTrans->mv;

	cross(normalInView, (*normal), mainTrans->mv);
	//TODO:这里把灯光转到视图空间

	// 计算视图空间的灯光方向
	Vec4f lightDirInView = *lightPos - ObjInView;

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
	float diffuse = max(0, (normalInView* lightDirInView));

	return ambientColor + diffuseColor * diffuse;
}


//2.带镜面高光的漫反射 也就是Phong反射模型
color_t Phong(transform_t* mainTrans, Vec4f *posInObj, Vec4f *normal, Vec4f* lightPos, Vec4f* cameraPos, color_t diffuseColor, color_t ambientColor, color_t specularColor)
{

	//	Output.position = mul(Input.position, matWorldViewProjection);
	//	Output.normalInView = normalize(mul(Input.normal, matWorldView));
	//	Output.lightDirInView = normalize(lightPos - mul(Input.position, matWorldView));
	//	float3 position = mul(Input.position, matWorldView);
	//	Output.viewDirInView = normalize(matView[3].xyz - position);
	//	return Output;

	//将顶点变换到视图空间  
	Vec4f  posInView;
		//= (*posInObj) * mainTrans->mv;

	cross(posInView, (*posInObj), mainTrans->mv);
	//将顶点变换到相机透视空间  
	Vec4f  posInCvv;
		//= (*posInObj) * mainTrans->mvp;
	cross(posInCvv, (*posInObj), mainTrans->mvp);

	//将法线变换到视图空间
	//matrix_apply(normalInView, normal, &mainTrans->mv);
	Vec4f normalInView;
		//=  (*normal)  * mainTrans->mv;
	cross(normalInView, (*normal), mainTrans->mv);

	// 计算视图空间的灯光方向
	Vec4f lightDirInView =  *lightPos- posInView;

	//计算视图控件里摄像机到顶点的方向
	Vec4f cameraDirInView =  *cameraPos - posInView;
	cameraDirInView.normalize();

	//----------------pix shader-----------------------------------//
	//	float4 diffuse = max(0, dot(In.normalInView, In.lightDirInView));
	//	diffuse = diffuse * diffuseColor;
	//
	//	float3 vReflect = normalize(2 * dot(In.normalInView, In.lightDirInView) * In.normalInView - In.lightDirInView);
	//
	//	float4 specular = specularColor * pow(max(0, dot(vReflect, In.viewDirInView)), 2);
	//
	//	Out.color = ambientColor + diffuse + specular;

	float diffuse = max(0, (normalInView* lightDirInView));
	diffuseColor = diffuseColor* diffuse;

	//反射向量
	Vec4f  vReflect = (normalInView) * (2 * (normalInView*lightDirInView)) - lightDirInView;
	vReflect.normalize();

	//再通过反射计算高光

	return  ambientColor + diffuseColor + specularColor * pow(max(0, (vReflect* cameraDirInView)), 2);
}

//TODO: 插值以后进入
float PixShader_Phong()
{
	return 0;
}

//Lambert模型能很好的表示粗糙表面的光照，但不能表现出镜面反射高光。1975年Phong Bui Tong发明的Phong模型，提出了计算镜面高光的经验模型，镜面反射光强与反射光线和视线的夹角a相关：
//
//Ispecular = Ks * Is*(cos a) n
//
//其中Ks为物体表面的高光系数，Is为光强，a是反射光与视线的夹角，n为高光指数，n越大，则表面越光滑，反射光越集中，高光范围越小。如果V表示顶点到视点的单位向量，R表示反射光反向，则cos a可表示为V和R的点积。模型可表示为：
//
//Ispecular = Ks * Is*(V●R)n
//
//反射光放向R可由入射光放向L（顶点指向光源）和物体法向量N求出。
//
//R = (2N●L)N – L
//
//1) VertexShader
//float4x4 matWorldViewProjection;
//float4x4 matWorldView;
//float4x4 matView;
//float3 lightPos;
//
//struct VS_INPUT
//{
//	float4 position : POSITION0;
//	float3 normal : NORMAL;
//};
//
//struct VS_OUTPUT
//{
//	float4 position : POSITION0;
//	float3 normalInView : TEXCOORD0;
//	float3 lightDirInView : TEXCOORD1;
//	float3 viewDirInView : TEXCOORD2;
//};
//
//VS_OUTPUT vs_main(VS_INPUT Input)
//{
//	VS_OUTPUT Output;
//
//	Output.position = mul(Input.position, matWorldViewProjection);
//	Output.normalInView = normalize(mul(Input.normal, matWorldView));
//	Output.lightDirInView = normalize(lightPos - mul(Input.position, matWorldView));
//	float3 position = mul(Input.position, matWorldView);
//	Output.viewDirInView = normalize(matView[3].xyz - position);
//	return Output;
//}
//
//
//
//2)	PixelShader
//
//float4 ambientColor;
//float4 diffuseColor;
//float4 specularColor;
//
//struct PS_INPUT
//{
//	float3 normalInView : TEXCOORD0;
//	float3 lightDirInView : TEXCOORD1;
//	float3 viewDirInView : TEXCOORD2;
//};
//
//struct PS_OUTPUT
//{
//	float4 color : COLOR0;
//};
//
//
//PS_OUTPUT ps_main(PS_INPUT In)
//{
//	PS_OUTPUT Out;
//
//	float4 diffuse = max(0, dot(In.normalInView, In.lightDirInView));
//	diffuse = diffuse * diffuseColor;
//
//	float3 vReflect = normalize(2 * dot(In.normalInView, In.lightDirInView) * In.normalInView - In.lightDirInView);
//
//	float4 specular = specularColor * pow(max(0, dot(vReflect, In.viewDirInView)), 2);
//
//	Out.color = ambientColor + diffuse + specular;
//
//	return Out;
//}

