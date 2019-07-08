#pragma once

#include "CMaterail.h"

namespace Raytracer {

	typedef unsigned int Pixel;
	// -----------------------------------------------------------
	// Ray class definition
	// -----------------------------------------------------------
	class Ray
	{
	public:
		Ray() : m_Origin(vector_t(0, 0, 0,1)), m_Direction(vector_t(0, 0, 0,0)) {};
		Ray(vector_t& a_Origin, vector_t& a_Dir);
		void SetOrigin(vector_t& a_Origin) { m_Origin = a_Origin; }
		void SetDirection(vector_t& a_Direction) { m_Direction = a_Direction; }
		vector_t& GetOrigin() { return m_Origin; } //原点 
		vector_t& GetDirection() { return m_Direction; } //方向
	private:
		vector_t m_Origin;
		vector_t m_Direction;
	};

	// -----------------------------------------------------------
	// Engine class definition
	// Raytracer core
	//引擎类定义
	//光线跟踪的核心
	// -----------------------------------------------------------
	class Scene;
	class Primitive;
	class Engine
	{
	public:
		Engine();
		~Engine();
		void SetTarget(Pixel* a_Dest, int a_Width, int a_Height);
		Scene* GetScene() { return m_Scene; }
		Primitive* Raytrace(Ray& a_Ray, color_t& a_Acc, int a_Depth, float a_RIndex, float& a_Dist);
		void InitRender();
		bool Render();
	protected:
		// renderer data
		float m_WX1, m_WY1, m_WX2, m_WY2, m_DX, m_DY, m_SX, m_SY;
		Scene* m_Scene;
		Pixel* m_Dest;
		int m_Width, m_Height, m_CurrLine, m_PPos;
		Primitive** m_LastRow;
	};

}; // namespace Raytracer
