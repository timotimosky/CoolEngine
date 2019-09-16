#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include <string>
//#include "geometry.h"
#include "renderMath.h"
#include "tgaimage.h"


//vector_t
//Vec3i
class Model {
private:
	std::vector<vector_t> verts_;
	std::vector<std::vector<Vec3i>> faces_; // attention, this Vec3i means vertex/uv/normal
	std::vector<vector_t> norms_;
	std::vector<Vec2f> uv_;
	TGAImage diffusemap_;
	TGAImage normalmap_;
	TGAImage specularmap_;
	void load_texture(std::string filename, const char* suffix, TGAImage& img);
public:
	Model(const char* filename);
	~Model();
	int nverts();
	int nfaces();
	vector_t normal(int iface, int nthvert);
	vector_t normal(Vec2f uv);
	vector_t vert(int i);
	vector_t vert(int iface, int nthvert);
	Vec2f uv(int iface, int nthvert);
	TGAColor diffuse(Vec2f uv);
	float specular(Vec2f uv);
	std::vector<int> face(int idx);
};
#endif //__MODEL_H__

