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
		Ray() : m_Origin(vector3(0, 0, 0)), m_Direction(vector3(0, 0, 0)) {};
		Ray(vector3& a_Origin, vector3& a_Dir);
		void SetOrigin(vector3& a_Origin) { m_Origin = a_Origin; }
		void SetDirection(vector3& a_Direction) { m_Direction = a_Direction; }
		vector3& GetOrigin() { return m_Origin; } //原点 
		vector3& GetDirection() { return m_Direction; } //方向
	private:
		vector3 m_Origin;
		vector3 m_Direction;
	};

	// -----------------------------------------------------------
	// Engine class definition
	// Raytracer core
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
		Primitive* Raytrace(Ray& a_Ray, Color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist);
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
