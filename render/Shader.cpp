#include "Shader.h"
#include "ShadingCalculate.h"
#include "Pipline.h"

Shader::Shader()
{
	v1 = new vertex_t();
	v2 = new vertex_t();
	v3 = new vertex_t();
}

Shader::~Shader()
{
	delete(v1);
	delete(v2);
	delete(v3);
}


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

////像素阶段，输入顶点
//bool Shader::fragment(Vec3f bar, TGAColor& color) {
//
//
//
//
//	return false;
//}

void Shader::vertex_shader(vertex_t* v1,int nfaces_index,int vertex_index)
{
	Vec3f v11 = mModel->vert(nfaces_index,vertex_index);

	v1->pos = point_t(v11.x, v11.y, v11.z, 1);

	v1->tc = mModel->uv(nfaces_index, vertex_index);

	Vec3f vNormal = mModel->normal(nfaces_index, vertex_index);

	v1->normal = Vec4f(vNormal.x, vNormal.y, vNormal.z,0);

	//v1->worldPos = v1->pos * transform.model;
	cross(v1->worldPos ,v1->pos , transform.model);
	//投影坐标 cvv空间
//	v1->project_pos = v1->pos * transform.mvp;
	cross(v1->project_pos, v1->pos, transform.mvp);
	//法线转换到世界空间
	//v1->world_normal = v1->normal * transform.model;
	cross(v1->world_normal, v1->normal, transform.model);
}

void Shader::frag_shader(vertex_t& frag_Vertex_t, IUINT32& color)
{

	float surfaceLight = ComputeNDotL(frag_Vertex_t.worldPos, frag_Vertex_t.world_normal, dirLight.dir);

	//前期用于坐标转换，后期用于存储深度
	float rhw = frag_Vertex_t.pos.w; 


	if (render_state & RENDER_STATE_COLOR) //使用位运算判断  //颜色模式是通过 顶点的W值来插值，但平面着色模式不是
	{
		//转为255
		int R = (int)(frag_Vertex_t.color.x * 255.0f);
		int G = (int)(frag_Vertex_t.color.y * 255.0f);
		int B = (int)(frag_Vertex_t.color.z * 255.0f);

		//保证在0-255之间
		R = CMID(R, 0, 255);
		G = CMID(G, 0, 255);
		B = CMID(B, 0, 255);

		////在像素阶段 根据深度图，计算深度，是否要产生阴影
		//if (device->shadowbuffer != NULL)
		//{
		//	float z = scanline->v.shadowPos_z;

		//	//	if (pcfDepth >0 && z > pcfDepth && pcfDepth !=0)
		//		//{
		//		//	//printf("nowZ========%f------oldZ==========%f\n", z, device->shadowbuffer[scanlineY*width + scanlineX]);
		//		//	R = R * 0.7;
		//		//	G = G * 0.7 ;
		//		//	B = B * 0.7 ;
		//		//}
		//}

		color = (R << 16) | (G << 8) | (B);  //<<是左移运算符    RGB 每个占8位.  或运算，恰好保留有1的位。 就合并RGB为一个int了

	}

	else if (render_state & RENDER_STATE_TEXTURE)
	{
		//if (mShader->mModel != nullptr)
		{
			TGAColor mTgacolor = mModel->diffuse(frag_Vertex_t.tc);
			int R = mTgacolor.bgra[2] - '0' + 48;
			int G = mTgacolor.bgra[1] - '0' + 48;
			int B = mTgacolor.bgra[0] - '0' + 48;
			//R *= surfaceLight;
			//G *= surfaceLight;
			//B *= surfaceLight;
			color = (R << 16) | (G << 8) | (B);
		}

		//在像素阶段 根据深度图，计算深度，是否要产生阴影
		//if (device->shadowbuffer != NULL)
		//{
		//	float z = scanline->v.shadowPos_z;

		//	/*if (pcfDepth > 0 && z > pcfDepth && pcfDepth != 0)
		//	{
		//		int R = (cc & (0xff0000)) >> 16;
		//		int G = (cc & (0x00ff00)) >> 8;
		//		int B = (cc & (0x0000ff));
		//		R *= 0.7;
		//		G *= 0.7;
		//		B *= 0.7;
		//		cc = (R << 16) | (G << 8) | (B);
		//	}*/
		//	//color = cc;
		//}
	}

	//顶点插值
	else if (render_state & (RENDER_STATE_verterNormal_color))
	{
		//asset(v.color.r<0.2f);
		//获取颜色 0-1
		float r = frag_Vertex_t.color.x + AmbientLight.r; //* pixRhw; 
		float g = frag_Vertex_t.color.y + AmbientLight.g;//* pixRhw;
		float b = frag_Vertex_t.color.z + AmbientLight.b;//* pixRhw;


		float verterLight = frag_Vertex_t.vertexLight;
		//+ scanline->step.vertexLight;

	//转为255
		int R = (int)(r * 255.0f * verterLight);
		int G = (int)(g * 255.0f * verterLight);
		int B = (int)(b * 255.0f * verterLight);

		//保证在0-255之间
		R = CMID(R, 0, 255);
		G = CMID(G, 0, 255);
		B = CMID(B, 0, 255);

		//位运算 
		color = (R << 16) | (G << 8) | (B);  //<<是左移运算符    RGB 每个占8位.  或运算，恰好保留有1的位。 就合并RGB为一个int了

	}

	else if (render_state & (RENDER_STATE_surfaceNormal_color))
	{
		//获取颜色 0-1
		float r = frag_Vertex_t.color.x;// *pixRhw; 
		float g = frag_Vertex_t.color.y;// *pixRhw;
		float b = frag_Vertex_t.color.z;// *pixRhw;

		//转为255
		//int R = (int)(r * 255.0f * surfaceLight);
		//int G = (int)(g * 255.0f * surfaceLight);
		//int B = (int)(b * 255.0f * surfaceLight);

		int R = (int)(r * 255.0f);
		int G = (int)(g * 255.0f);
		int B = (int)(b * 255.0f);

		//保证在0-255之间
		R = CMID(R, 0, 255);
		G = CMID(G, 0, 255);
		B = CMID(B, 0, 255);

		//位运算 
		color = (R << 16) | (G << 8) | (B);  //<<是左移运算符    RGB 每个占8位.  或运算，恰好保留有1的位。 就合并RGB为一个int了

	}
}
