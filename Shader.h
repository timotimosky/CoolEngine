#pragma once
#include "render/geometry.h"
#include "render/Camera.h"
#include "render/Model.h"

//class IShader {
//	virtual ~IShader();
//	virtual Vec4f vertex(int iface, int nthvert) = 0;
//	virtual bool fragment(Vec3f bar, TGAColor& color) = 0;
//};

class Shader//:public IShader
{
public:
	//当前哪个摄像机渲染它
	camera curCamera;

	Model *mModel; //暂时保存一份引用

	transform_t transform;

	int render_state;

	//屏幕分辨率

	int tex_width;              // 纹理宽度
	int tex_height;             // 纹理高度
	IUINT32** texture; //[256][256]; //纹理

	Vec2f _uv;  //UV  // 纹理最大宽度：tex_width - 1  // 纹理最大高度：tex_height - 1
	Vec3f _verter; //顶点

		//每个摄像机都走单独的渲染管线，下面用于缓存摄像机影响渲染管线的参数
	int cull;   // 0:不裁剪;1:裁剪反面;2:裁剪正面

	virtual Vec4f vertex(int iface, int nthvert);

	virtual bool fragment(Vec3f bar, TGAColor& color);
};
//在.obj文件中，我们具有以“ vt u v”开头的行，它们给出了纹理坐标数组。
//刻面线“ fx / x / xx / x / xx / x / x”中中间（斜线之间）的数字是此三角形顶点的纹理坐标。
//将其插入三角形内，乘以纹理图像的宽度 - 高度，您将获得要放入渲染中的颜色。

//先每个物体使用每个shader渲染 都写入buffer（公用）中（这里处理屏幕分辨率）
//最后的devece 用这个buffer写入屏幕（这里处理）