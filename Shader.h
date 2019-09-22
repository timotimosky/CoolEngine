#pragma once
#include "render/geometry.h"
#include "render/Camera.h"
#include "render/Model.h"

class Shader
{
public:
	//当前哪个摄像机渲染它
	camera curCamera;

	Model *mModel; //暂时保存一份引用

	//屏幕分辨率

	int tex_width;              // 纹理宽度
	int tex_height;             // 纹理高度
	IUINT32** texture; //[256][256]; //纹理

	Vec2f _uv;  //UV  // 纹理最大宽度：tex_width - 1  // 纹理最大高度：tex_height - 1
	Vec3f _verter; //顶点

	int cull;   // 0:不裁剪;1:裁剪反面;2:裁剪正面

};


//先每个物体使用每个shader渲染 都写入buffer（公用）中（这里处理屏幕分辨率）
//最后的devece 用这个buffer写入屏幕（这里处理）