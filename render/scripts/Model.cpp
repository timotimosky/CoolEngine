#include <iostream>
#include <fstream>
#include <sstream>
#include "include\model.h"

using namespace std;
Model::Model(const char* filename) : verts_(), faces_(), norms_(), uv_(), diffusemap_(), normalmap_(), specularmap_() {
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail())
	{
		cout << "no file-------" << endl;
		return;
	}

	// .obj文件中，每一行都有表明该行意义的标志符。对obj的读取中，处理以下标志符
	/*

	"v"--点的坐标，三维模型为x, y, z的顺序；程序中的结构存储：
	 typedef struct ObjVector3
	 {
			 ObjFloat x;
			 ObjFloat y;
			 ObjFloat z;
		 } ObjVector3;


	"vt"--纹理坐标，程序中结构存储：

	 typedef struct ObjVector2
	 {
		 ObjFloat x;
		 ObjFloat y;
	 } ObjVector2;




	 "vn"--法向量坐标，程序中与“v”的存储结构相同

	"f"--面所用到的点坐标/纹理坐标/法向量坐标的索引，
	  以vec3数据结构挨个存储3个索引， 分别为 点坐标/纹理坐标/法向量坐标的索引，
	*/

	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		if (line.empty())
			continue;

		std::istringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v ")) {
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++) iss >> v[i];
			verts_.push_back(v);
		}
		else if (!line.compare(0, 3, "vn ")) {
			iss >> trash >> trash;
			Vec3f n;
			for (int i = 0; i < 3; i++) iss >> n[i];
			norms_.push_back(n);
		}
		else if (!line.compare(0, 3, "vt ")) {
			iss >> trash >> trash;
			Vec2f uv;
			for (int i = 0; i < 2; i++) iss >> uv[i];
			uv_.push_back(uv);
		}
		else if (!line.compare(0, 2, "f ")) {
			std::vector<Vec3i> f;
			Vec3i tmp;
			iss >> trash;
			//istringstream  通过>>操作符 按顺序 读取字符串  以空格为分类
			while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
				for (int i = 0; i < 3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
				f.push_back(tmp);
			}
			faces_.push_back(f);
		}
	}
	std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << std::endl;
	load_texture(filename, "_diffuse.tga", diffusemap_);
	//load_texture(filename, "_nm_tangent.tga", normalmap_);
	//load_texture(filename, "_spec.tga", specularmap_);

	load_texture(filename, "_normal.tga", normalmap_);
	load_texture(filename, "_specular.tga", specularmap_);
	//D:/github/CoolEngine/obj/crab/crab.ani
	//skeleton =skeleton_load(filename);
}

Model::~Model() {}

int Model::nverts() {
	return (int)verts_.size();
}

int Model::nfaces() {
	return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
	std::vector<int> face;
	for (int i = 0; i < (int)faces_[idx].size(); i++) 
		face.push_back(faces_[idx][i][0]);
	return face;
}

Vec3f Model::vert(int i) {
	return verts_[i];
}

Vec3f Model::vert(int iface, int nthvert) {
	return verts_[faces_[iface][nthvert][0]];
}

void Model::load_texture(std::string filename, const char* suffix, TGAImage& img) {
	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");
	if (dot != std::string::npos) {
		texfile = texfile.substr(0, dot) + std::string(suffix);
		bool load_success = img.read_tga_file(texfile.c_str());
		std::cerr << "texture file " << texfile << " loading " << (load_success ? "ok" : "failed") << std::endl;
		img.flip_vertically();
	}
}

TGAColor Model::diffuse(Vec2f uvf) {
	int u = uvf[0] * diffusemap_.get_width();
	int v = uvf[1] * diffusemap_.get_height();
	
	return diffusemap_.get(u, v);
}

Vec3f Model::normal(Vec2f uvf) {
	Vec2i uv(uvf[0] * normalmap_.get_width(), uvf[1] * normalmap_.get_height());
	TGAColor c = normalmap_.get(uv[0], uv[1]);
	Vec3f res;
	for (int i = 0; i < 3; i++)
		res[2 - i] = (float)c[i] / 255.f * 2.f - 1.f;
	return res;
}

Vec2f Model::uv(int iface, int nthvert) {
	return uv_[faces_[iface][nthvert][1]];
}

float Model::specular(Vec2f uvf) {
	Vec2i uv(uvf[0] * specularmap_.get_width(), uvf[1] * specularmap_.get_height());
	return specularmap_.get(uv[0], uv[1])[0] / 1.f;
}

Vec3f Model::normal(int iface, int nthvert) {
	int idx = faces_[iface][nthvert][2];
	return norms_[idx].normalize();
}

