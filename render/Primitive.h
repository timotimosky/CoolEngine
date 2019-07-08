#pragma once
#include "CMaterail.h"

class Primitive
{
public:
	enum
	{
		SPHERE = 1, //«Ú–Õ
		PLANE  //µÿ∞Â
	};
	Primitive() : m_Name(0), m_Light(false) {};
	materal* GetMaterial() { return &m_Material; }
	//void SetMaterial(materal& a_Mat) { m_Material = a_Mat; }
	virtual int GetType() = 0;
	//virtual int Intersect(Ray& a_Ray, float& a_Dist) = 0;
//	virtual vector3 GetNormal(vector3& a_Pos) = 0;
//	virtual Color GetColor(vector3&) { return m_Material.GetColor(); }
	virtual void Light(bool a_Light) { m_Light = a_Light; }
	bool IsLight() { return m_Light; }
	void SetName(const char* a_Name);
	char* GetName() { return m_Name; }
protected:
	materal m_Material;
	char* m_Name;
	bool m_Light;
};
