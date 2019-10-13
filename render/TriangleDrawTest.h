//#pragma once
//
//#include "LineDrawTest.h"
//
//
//const TGAColor white = TGAColor(255, 255, 255, 255);
//const TGAColor red = TGAColor(255, 0, 0, 255);
//
//void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
//	line(t0, t1, image, color);
//	line(t1, t2, image, color);
//	line(t2, t0, image, color);
//}
//
//// ...
//void Testtriangle()
//{
//	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
//	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
//	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
//	triangle(t0[0], t0[1], t0[2], image, red);
//	triangle(t1[0], t1[1], t1[2], image, white);
//	triangle(t2[0], t2[1], t2[2], image, green);
//}