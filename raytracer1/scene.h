// -----------------------------------------------------------
// scene.h
// 2004 - Jacco Bikker - jacco@bik5.com - www.bik5.com -   <><
// -----------------------------------------------------------

#ifndef I_SCENE_H
#define I_SCENE_H

#include "raytracer.h"

namespace Raytracer {

// Intersection method return values
#define HIT		 1		// Ray hit primitive
#define MISS	 0		// Ray missed primitive
#define INPRIM	-1		// Ray started inside primitive

// -----------------------------------------------------------
// Material class definition
// -----------------------------------------------------------

class Material
{
public:
	Material();
	void SetColor( Color& a_Color ) { m_Color = a_Color; }
	Color GetColor() { return m_Color; }
	void SetDiffuse( float a_Diff ) { m_Diff = a_Diff; }
	void SetReflection( float a_Refl ) { m_Refl = a_Refl; }
	float GetSpecular() { return 1.0f - m_Diff; }
	float GetDiffuse() { return m_Diff; }
	float GetReflection() { return m_Refl; }
private:
	Color m_Color;
	float m_Refl;
	float m_Diff;
};

// -----------------------------------------------------------
// Primitive class definition
// -----------------------------------------------------------

class Primitive
{
public:
	enum
	{
		SPHERE = 1,
		PLANE
	};
	Primitive() : m_Name( 0 ), m_Light( false ) {};
	Material* GetMaterial() { return &m_Material; }
	void SetMaterial( Material& a_Mat ) { m_Material = a_Mat; }
	virtual int GetType() = 0;
	virtual int Intersect( Ray& a_Ray, float& a_Dist ) = 0;
	virtual vector3 GetNormal( vector3& a_Pos ) = 0;
	virtual Color GetColor( vector3& ) { return m_Material.GetColor(); }
	virtual void Light( bool a_Light ) { m_Light = a_Light; }
	bool IsLight() { return m_Light; }
	void SetName( char* a_Name );
	char* GetName() { return m_Name; }
protected:
	Material m_Material;
	char* m_Name;
	bool m_Light;
};

// -----------------------------------------------------------
// Sphere primitive class definition
// -----------------------------------------------------------

class Sphere : public Primitive
{
public:
	int GetType() { return SPHERE; }
	Sphere( vector3& a_Centre, float a_Radius ) : 
		m_Centre( a_Centre ), m_SqRadius( a_Radius * a_Radius ), 
		m_Radius( a_Radius ), m_RRadius( 1.0f / a_Radius ) {};
	vector3& GetCentre() { return m_Centre; }
	float GetSqRadius() { return m_SqRadius; }
	int Intersect( Ray& a_Ray, float& a_Dist );
	vector3 GetNormal( vector3& a_Pos ) { return (a_Pos - m_Centre) * m_RRadius; }
private:
	vector3 m_Centre;
	float m_SqRadius, m_Radius, m_RRadius;
};

// -----------------------------------------------------------
// PlanePrim primitive class definition
// -----------------------------------------------------------

class PlanePrim : public Primitive
{
public:
	int GetType() { return PLANE; }
	PlanePrim( vector3& a_Normal, float a_D ) : m_Plane( plane( a_Normal, a_D ) ) {};
	vector3& GetNormal() { return m_Plane.N; }
	float GetD() { return m_Plane.D; }
	int Intersect( Ray& a_Ray, float& a_Dist );
	vector3 GetNormal( vector3& a_Pos );
private:
	plane m_Plane;
};

// -----------------------------------------------------------
// Scene class definition
// -----------------------------------------------------------

class Scene
{
public:
	Scene() : m_Primitives( 0 ), m_Primitive( 0 ) {};
	~Scene();
	void InitScene();
	int GetNrPrimitives() { return m_Primitives; }
	Primitive* GetPrimitive( int a_Idx ) { return m_Primitive[a_Idx]; }
private:
	int m_Primitives;
	Primitive** m_Primitive;
};

}; // namespace Raytracer

#endif