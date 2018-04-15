#include "shaderStruct.h"

//TODO:需要拆分为 按帧更新、按物体更新
// 矩阵更新
void transform_update(transform_t *ts) {

	matrix_mul(&ts->mv, &ts->model, &ts->view);
	matrix_mul(&ts->vp, &ts->view, &ts->projection);
	matrix_mul(&ts->mvp, &ts->mv, &ts->projection);
}


//结论是“行向量左乘矩阵时，结果是行向量，列向量右乘矩阵时，结果是列向量，反过来是不行的”，在DirectX中使用的是行向量，在OpenGL中使用的是列向量。
//这里用的右乘

// w 也要参与到坐标变换的计算  到时候在CVV的时候， x,y,z,w要除以W，此刻 w=1,投影到屏幕坐标了
//得到在屏幕上的坐标. (假设屏幕的Z值是1)

void transform_apply(const transform_t *ts, vector_t *posInCVV, const vector_t *posInObj) {
	 
	//拿到MVP矩阵，从物体坐标转到CVV坐标
	matrix_apply(posInCVV, posInObj, &ts->mvp);
}


void transform_applyVP(const transform_t *ts, vector_t *posInCVV, const vector_t *posInObj) {

	//拿到MVP矩阵，从物体坐标转到CVV坐标
	matrix_apply(posInCVV, posInObj, &ts->vp);
}

void device_clear(device_t *device) {
	if (device->framebuffer != NULL) {
		for (int y = 0; y < device->height; y++)
			for (int x = 0; x < device->width; x++)
				//device->framebuffer[y * device->width + x] = device->background;
				device->framebuffer[y][x] = device->background;
	}
	// memset(device->framebuffer, 0xff, device->camera->width * device->camera->height * sizeof(IUINT32));
	if (device->zbuffer != NULL)
		memset(device->zbuffer, 0, device->width * device->height * sizeof(float));
	if (device->shadowbuffer != NULL) {
		for (int y = 0; y < device->height; y++)
			for (int x = 0; x < device->width; x++)
				device->shadowbuffer[y * device->width + x] = 1.0f;
	}
}
