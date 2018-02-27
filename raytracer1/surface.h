// -----------------------------------------------------------
// surface.h
// 2004 - Jacco Bikker - jacco@bik5.com - www.bik5.com -   <><
// -----------------------------------------------------------

#ifndef I_SURFACE_H
#define I_SURFACE_H

#include "string.h"
#include "common.h"

namespace Raytracer {

class Surface
{
	enum
	{
		OWNER = 1
	};

public:
	// constructor / destructors
	Surface( int a_Width, int a_Height );
	Surface( char* a_File );
	~Surface();

	// member data access
    Pixel* GetBuffer() { return m_Buffer; }
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }

	// Special operations
	void InitCharset();
	void SetChar( int c, char* c1, char* c2, char* c3, char* c4, char* c5 );
	void Print( char* a_String, int x1, int y1, Pixel color );
	void Clear( Pixel a_Color );

private:
	// Attributes
	Pixel* m_Buffer;	
	int m_Width, m_Height;	
	
	// Static attributes for the buildin font
	char s_Font[51][5][5];	
	int s_Transl[256];		
};

}; // namespace Raytracer

#endif