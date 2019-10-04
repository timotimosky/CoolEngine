#include "Shader.h"
#include "render/UObject.h"
#include "render/ShadingCalculate.h"
#include "render/Pipline.h"



//IShader::~IShader() {}

//为了使用GPU的并行运算

//对顶点执行 
//1.坐标变换（透视投影）  
//2.逐顶点光照计算、计算动画角色的蒙皮、纹理计算、变形
//1.物体坐标系到世界坐标系的转换
//2.CVV空间裁剪(视锥裁剪)
//2.初始化： uv的赋值
Model* model = NULL;

//输出：完成变换以及光照的顶点，其位置和法向量是以齐次坐标空间表示的

//返回 Vec4f ：齐次坐标
Vec3f Shader::vertex(Vec3f iface, int nthvert) {
	Vec3f gl_Vertex = iface;
		//model->vert(iface, nthvert);
	vertex_t* v1 = new vertex_t();
	v1->pos =  point_t(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z,1);  //这个由上一层几何应用器 传入

	//1--------物体空间------------------------------

	point_t world_pos1,
		//world_pos2, world_pos3, //世界坐标
	// 光栅化坐标，在一个2D平面上，对应到屏幕上真正的像素了（本应该通过当前的采样方式来对应，这里直接是按屏幕像素大小缩放）	
	raster_pos1,
	//raster_pos2, raster_pos3,
	project_pos1 ;
		//project_pos2, project_pos3; //投影坐标 cvv空间

	//2--------世界空间----------计算光照---------------------------------如果是烘焙 没法考虑摄像机遮挡，所以在这里直接计算---//

	//坐标转到世界空间
	world_pos1 = v1->pos * transform.model;


	//// 背面剔除
	//if (cull > 0)
	//{
	//	float cullValue = CullCalcutate(&world_pos1, &world_pos2, &world_pos3, &curCamera.eye);
	//	if (cull == 1)
	//	{
	//		if (cullValue <= 0)
	//			return Vec3f();
	//	}
	//	else if (cull == 2) {
	//		if (cullValue > 0)
	//			return Vec3f();
	//	}
	//}

	// 这里的裁剪不准确，只要有顶点不满足，则剔除，可以完善为具体判断几个点在 cvv内以及同cvv相交平面的坐标比例
	// 进行进一步精细裁剪，将一个分解为几个完全处在 cvv内的三角形

	//--------------------------------4.CVV空间裁剪(视锥裁剪)-----------------------------
	project_pos1 = v1->pos * transform.mvp;

	if (transform_check_cvv(&project_pos1) != 0) return Vec3f();


	//法线转换到世界空间
	Vec4f world_normal1, world_normal2, world_normal3;
	world_normal1 = v1->normal * transform.model;
	//阴影预备
	//DisVertexToLight(lightPosition, &world_pos1);

	//转换到灯光空间


	//--------如果是动态物体-------在视锥裁剪之后计算，比较节约性能---------另外,部分shader特效需要在摄像机空间计算法线、摄像机、灯光三者的角度------//


	//灯光参数，用于乘基本颜色     (0-1)之间 被用作颜色的亮度 
	float surfaceLight = 0; //表面灯光
	float v1Light = 0;

	if (render_state & RENDER_STATE_surfaceNormal_color)
	{
		//surfaceLight = calculateGroudShader(&world_pos1, &world_pos2, &world_pos3); //表面灯光
																					//printf("surfaceLight出来%f", surfaceLight);
	}
	else if (render_state & RENDER_STATE_verterNormal_color)
	{
		v1->vertexLight = calculateVertexLight(&world_pos1, &world_normal1);
	}


	//避免重复计算rhw. 在光栅化初期计算完了就保存
	//2.------------------光栅化空间--------------5.光栅化过程：先归一化，从CVV空间到矩形空间，然后从 -1到1变换到0到1的取值范围，然后 拿到屏幕的长宽，取得屏幕坐标-------------
	transform_homogenize(&transform, &raster_pos1, &project_pos1);


	//保存光栅化坐标 到shader上下文缓存
	//varying_tri

	Vec3f _verter = Vec3f(raster_pos1.x, raster_pos1.y, raster_pos1.z);
	varying_tri.set_col(nthvert,_verter);

	//---------TODO：生成阴影摄像机的深度图
	//如果阴影,插值
	point_t shadow_view_pos1, shadow_view_pos2, shadow_view_pos3;

	//切换到光源视点，渲染一张图（平行光是正交投影,点光源是透视投影）

	transform_t shadow_transform = transform;

	shadow_view_pos1 =  v1->pos * shadow_transform.mv;


	//TODO：这里以后要改为投影摄像机的成像大小 不用屏幕大小
   //transform_homogenize(&shadow_transform, &shadow_raster_pos1, &shadow_project_pos1);
   //transform_homogenize(&shadow_transform, &shadow_raster_pos2, &shadow_project_pos2);
   //transform_homogenize(&shadow_transform, &shadow_raster_pos3, &shadow_project_pos3);
   //point_t shadow_points[3] = point_t{ shadow_raster_pos1, shadow_raster_pos2, shadow_raster_pos3 };

	return gl_Vertex;
}

//中间光栅化

//像素阶段，输入顶点
bool Shader::fragment(Vec3f bar, TGAColor& color) {




	return false;
}