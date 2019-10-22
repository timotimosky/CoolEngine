#pragma once
#include <vector> 
#include <iostream> 
#include "geometry.h"
#include "LineDrawTest.h"
#include "Pipline.h"
#include <iostream>
#include <time.h>
#include <windows.h>

#include <map>
using namespace std;
#define maxA(a,b)            (((a) > (b)) ? (a) : (b))
#define minA(a,b)            (((a) < (b)) ? (a) : (b))
//const TGAColor white = TGAColor(255, 255, 255, 255);
//const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(255, 255, 0, 255);
map<int, int> Bdic;
//渲染三角形的边
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, const TGAColor& color) {
	// sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!) 
	//if (t0.y > t1.y) std::swap(t0, t1);
	//if (t0.y > t2.y) std::swap(t0, t2);
	//if (t1.y > t2.y) std::swap(t1, t2);
	line(t0.x,t0.y, t1.x,t1.y, image, green);
	line(t1.x, t1.y, t2.x, t2.y, image, white);
	line(t2.x, t2.y, t0.x, t0.y,  image, red);
}
int renderNum;
int copyNum; //重复
map<int, int> dic;
//渲染半个三角形:它应该是对称的：图片不应取决于传递给绘图函数的顶点的顺序。
void triangle1(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, const TGAColor& color) {
	// sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!) 
	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);
	int total_height = t2.y - t0.y;//110

	Vec2i T2_T0 = t2 - t0;
	Vec2i T1_T0 = t1 - t0;
	Vec2i T2_T1 = (t2 - t1);
	int segment_height = t1.y - t0.y + 1;
	dic = map<int, int>();
	copyNum = renderNum = 0;
	for (int y = t0.y; y <= t1.y; y++) {

		float alpha = (float)(y - t0.y) / total_height;
		float beta = (float)(y - t0.y) / segment_height; // be careful with divisions by zero 
		Vec2i A = t0 + T2_T0 * alpha;
		Vec2i B = t0 + T1_T0 * beta;
		if (A.x > B.x) std::swap(A, B);
		for (int j = A.x; j <= B.x; j++) {

			if (!Bdic[j * 400 + y])
			{
				printf("-triangle1--漏渲染--%i---%i-\n", j,y);
			}
			renderNum++;
			if (dic[j * 400 + y])
			{
				copyNum++;
			}
			else 
				dic[j * 400 + y] = 1;
			image.set(j, y, color); // attention, due to int casts t0.y+i != A.y 
		}
	}

	segment_height = t2.y - t1.y + 1;
	for (int y = t1.y; y <= t2.y; y++) {

		float alpha = (float)(y - t0.y) / total_height;
		float beta = (float)(y - t1.y) / segment_height; // be careful with divisions by zero 
		Vec2i A = t0 + T2_T0 * alpha;
		Vec2i B = t1 + T2_T1 * beta;
		if (A.x > B.x) std::swap(A, B);
		for (int j = A.x; j <= B.x; j++) {
			renderNum++;
			if (dic[j * 400 + y])
			{
				copyNum++;
			}
			else
				dic[j * 400 + y] = 1;
			image.set(j, y, color); // attention, due to int casts t0.y+i != A.y 
		}
	}
	printf("-triangle1--重复--%i--\n", copyNum);
	printf("-triangle1-总渲染数量-----%i---\n", renderNum);
}


//渲染三角形: 跟上面一样，但代码更短
void triangle2(Vec2f t0, Vec2f t1, Vec2f t2, TGAImage& image, const TGAColor& color) {
	// sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!) 
	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);
	int total_height = t2.y - t0.y;//110

	Vec2f T2_T0 = t2 - t0;
	Vec2f T1_T0 = t1 - t0;
	Vec2f T2_T1 = (t2 - t1);

	dic = map<int, int>();
	copyNum = renderNum = 0;
	for (int i = 0; i < total_height; i++) {
		bool second_half = i > T1_T0.y || t1.y == t0.y; //是否属于上半三角形
		int segment_height = second_half ? T2_T1.y : T1_T0.y;
		float alpha = (float)i / total_height;
		float beta = (float)(i - (second_half ? T1_T0.y : 0)) / segment_height; // be careful: with above conditions no division by zero here 
		Vec2f A = t0 + T2_T0 * alpha;
		Vec2f B = second_half ? t1 + (T2_T1) * beta : t0 + (T1_T0) * beta;
		if (A.x > B.x) std::swap(A, B);
		for (int j = A.x; j <= B.x; j++) {
			renderNum++;
			if (dic[j * 400 + i])
			{
				copyNum++;
			}
			else
				dic[j * 400 + i] = 1;
			image.set(j, t0.y + i, color); // attention, due to int casts t0.y+i != A.y 
		}
	}
	printf("-triangle2-----重复--%i--\n", copyNum);
	printf("-triangle2-总渲染数量-----%i---\n", renderNum);
}


void trianglebarycentric(Vec2f v1, Vec2f v2, Vec2f v3, TGAImage& image, const TGAColor& color) {
	Vec4f MaxBox = TriangleMaxMinXY(embed<4>(v1), embed<4>(v2), embed<4>(v3));
	MaxBox.x = MaxBox.x > 400 ? 400 : MaxBox.x;
	MaxBox.z = MaxBox.z > 400 ? 400 : MaxBox.z;
	Vec2i P;
	Vec3f uv;
	vertex_t ret;
	vertex_t ver1 = vertex_t();
	vertex_t ver2 = vertex_t();
	vertex_t ver3 = vertex_t();

	ver1.pos = embed<4>(v1);
	ver2.pos = embed<4>(v2);
	ver3.pos = embed<4>(v3);

	Bdic = map<int, int>();
	copyNum = renderNum = 0;

	for (P.y = MaxBox.w; P.y <= MaxBox.z; P.y++) {
		for (P.x = MaxBox.y; P.x <= MaxBox.x; P.x++)
		{
			if (P.x == 179 && P.y == 151)
				printf("%i----- %i\n", P.x, P.y);
			
			PointinTriangleUV(v1, v2, v3, P, uv);
			//uv=	barycentric(v1, v2, v3, P);
			if (!PointinTriangle(uv))
				continue;
			vertex_interpUV(ret, ver1, ver2, ver3, uv);
			//mShader->frag_shader(ret, color);

			TGAColor nTGAColor = TGAColor((int)(ret.pos.x), (int)(ret.pos.y),
				(int)(ret.pos.z));
			
			renderNum++;
			if (Bdic[P.x * 400 + P.y])
			{
				copyNum++;
			}
			else
				Bdic[P.x * 400 + P.y] = 1;
			image.set(P.x, P.y, nTGAColor);
		}
	}
	//MaxBox = TriangleMaxMinXY(v1->raster_pos, v2->raster_pos, v3->raster_pos);
	//MaxBox.x = MaxBox.x > MaxScreenWidth ? MaxScreenWidth : MaxBox.x;
	//MaxBox.z = MaxBox.z > MaxScreenHeight ? MaxScreenHeight : MaxBox.z;

	printf("trianglebarycentric-------重复--%i--\n", copyNum);
	printf("trianglebarycentric----总渲染数量-----%i---\n", renderNum);
}

//重心坐标
Vec3f barycentric(Vec2i* pts, Vec2i P) {
	Vec3f u = cross(Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]), Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]));
	/* `pts` and `P` has integer value as coordinates
	   so `abs(u[2])` < 1 means `u[2]` is 0, that means
	   triangle is degenerate, in this case return something with negative coordinates */
	if (std::abs(u[2]) < 1) return Vec3f(-1, 1, 1);
	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}
const int width = 200;
const int height = 200;

//barycentric（）函数计算给定三角形中点P的坐标，我们已经看到了细节。现在，让我们看看triangle（）函数的工作方式。
//首先，它计算一个边界框，它由两点描述：左下角和右上角。为了找到这些角，我们遍历三角形的顶点并选择最小/最大坐标。
//我还添加了带有屏幕矩形的边界框的剪辑，以节省屏幕外部三角形的CPU时间。
void triangle3(Vec2i* pts, TGAImage& image, TGAColor color) {
	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
	Vec2i bboxmax(0, 0);
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
	dic = map<int, int>();
	copyNum = renderNum = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin[j] = max(0, min(bboxmin[j], pts[i][j]));
			bboxmax[j] = min(clamp[j], max(bboxmax[j], pts[i][j]));
		}
	}
	Vec2i P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(pts, P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;

			 renderNum++;
			 if (dic[P.x * 400 + P.y])
			 {
				 copyNum++;
			 }
			 else
				 dic[P.x * 400 + P.y] = 1;
			image.set(P.x, P.y, color);
		}
	}
	printf("triangle3---------重复--%i--\n", copyNum);
	printf("-triangle3-总渲染数量-----%i---\n", renderNum);
}

void Testtriangle(TGAImage& image)
{
	renderNum = 0;
	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
	//triangle1(t0[0], t0[1], t0[2], image, red);
	//triangle1(t1[0], t1[1], t1[2], image, white);
	triangle1(t2[0], t2[1], t2[2], image, green);
}

void Testtriangle2(TGAImage& image) {

	Vec2f t0[3] = { Vec2f(10, 70),   Vec2f(50, 160),  Vec2f(70, 80) };
	Vec2f t1[3] = { Vec2f(180, 50),  Vec2f(150, 1),   Vec2f(70, 180) };
	Vec2f t2[3] = { Vec2f(180, 150), Vec2f(120, 160), Vec2f(130, 180) };
	//triangle2(t0[0], t0[1], t0[2], image, red);
	//triangle2(t1[0], t1[1], t1[2], image, white);
	triangle2(t2[0], t2[1], t2[2], image, green);

	//trianglebarycentric(t0[0], t0[1], t0[2], image, red);
	//trianglebarycentric(t1[0], t1[1], t1[2], image, white);
	//trianglebarycentric(t2[0], t2[1], t2[2], image, green);
}

void Testtriangle3(TGAImage& image) {

	//Vec2i pts[3] = { Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160) };
///	triangle3(pts, frame, TGAColor(255, 0, 0));

	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
	//triangle3(t0, image, red);
	//triangle3(t1, image, white);
	triangle3(t2, image, green);
}

void TestTrianglebarycentric(TGAImage& image) {

	Vec2f t0[3] = { Vec2f(10, 70),   Vec2f(50, 160),  Vec2f(70, 80) };
	Vec2f t1[3] = { Vec2f(180, 50),  Vec2f(150, 1),   Vec2f(70, 180) };
	Vec2f t2[3] = { Vec2f(180, 150), Vec2f(120, 160), Vec2f(130, 180) };
	//triangle2(t0[0], t0[1], t0[2], image, red);
	//triangle2(t1[0], t1[1], t1[2], image, white);
	//triangle2(t2[0], t2[1], t2[2], image, green);

	//trianglebarycentric(t0[0], t0[1], t0[2], image, red);
	//trianglebarycentric(t1[0], t1[1], t1[2], image, white);
	trianglebarycentric(t2[0], t2[1], t2[2], image, green);
}

clock_t  start;
clock_t  finish;
float duration;

int TestTrianglemain() {


	TGAImage image(400, 400, TGAImage::RGB);

	start = clock();
	///for(int i=0; i<10000;i++)
	TestTrianglebarycentric(image);
	//Testtriangle(image);
	//Testtriangle2(image);
	//Testtriangle3(image);

	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;


	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	cout << imageCount << "----" << endl;
	printf("pos tramsform----- %f seconds\n", duration);

	return 0;
}