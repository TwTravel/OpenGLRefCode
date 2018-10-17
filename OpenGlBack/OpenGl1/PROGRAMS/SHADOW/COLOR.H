// Color.h: interface for the Color class.
//
//////////////////////////////////////////////////////////////////////

#include "gl\gl.h"

#if !defined(AFX_COLOR_H__27EED6A2_8BAE_11D2_83B4_0080C832FCF3__INCLUDED_)
#define AFX_COLOR_H__27EED6A2_8BAE_11D2_83B4_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class Color  
{
public:
	Color();
	virtual ~Color();
public:
	inline	Color(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0);
	inline	Color operator=(GLfloat *a);
	inline	Color operator=(GLfloat a);
	inline	Color operator=(Color color);
	inline	Color operator+(Color a);
	inline	Color operator+=(Color a);
	inline	Color operator*(Color a);
	inline	Color operator*(GLfloat a);
	inline	Color operator*=(Color a);
	inline	Color operator*=(GLfloat *a);
	inline	Color operator*=(GLfloat a);

	inline	GLfloat& operator[](int index);

	inline	Color	clamp();

	inline	void	glcolor();

	void	set(GLfloat r,GLfloat g,GLfloat b,GLfloat a=1.0);

public:
	
    GLfloat	c[4];
};

inline Color::Color(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	c[0] = r;
	c[1] = g;
	c[2] = b;
	c[3] = a;
}

inline Color Color::operator=(GLfloat a)
{
	c[0] = c[1] = c[2] = c[3] = a;
	return *this;
}

inline Color Color::operator=(GLfloat *a)
{
	c[0] = a[0];
	c[1] = a[1];
	c[2] = a[2];
	c[3] = a[3];
	return *this;
}

inline Color Color:: operator=(Color color)
{
	c[0] = color.c[0];
	c[1] = color.c[1];
	c[2] = color.c[2];
	c[3] = color.c[3];
	return *this;

}


inline Color Color::operator+(Color a) 
{
	Color resultCl;
	resultCl.c[0] = c[0] + a.c[0];
	resultCl.c[1] = c[1] + a.c[1];
	resultCl.c[2] = c[2] + a.c[2];
	resultCl.c[3] = c[3] + a.c[3];
	return resultCl;
}

inline Color Color::operator+=(Color a)
{
	c[0] += a.c[0];
	c[1] += a.c[1];
	c[2] += a.c[2];
	c[3] += a.c[3];
	return *this;
}

inline Color Color::operator*(Color a)
{
	Color resultCl;
	resultCl.c[0] = c[0] * a.c[0];
	resultCl.c[1] = c[1] * a.c[1];
	resultCl.c[2] = c[2] * a.c[2];
	resultCl.c[3] = c[3] * a.c[3];
	return resultCl;
}

inline Color Color::operator*(GLfloat a)
{
	Color resultCl;
	resultCl.c[0] = c[0] * a;
	resultCl.c[1] = c[1] * a;
	resultCl.c[2] = c[2] * a;
	resultCl.c[3] = c[3] * a;
	return resultCl;
}

inline Color Color::operator*=(Color a)
{
	c[0] *= a.c[0];
	c[1] *= a.c[1];
	c[2] *= a.c[2];
	return *this;
}

inline Color Color::operator*=(GLfloat *a) 
{
	c[0] *= a[0];
	c[1] *= a[1];
	c[2] *= a[2];
	return *this;
}

inline Color Color::operator*=(GLfloat a)
{
	c[0] *= a;
	c[1] *= a;
	c[2] *= a;
	c[3] *= a;

	return *this;
}

inline GLfloat& Color::operator[](int index)
{
	return c[index];
}

inline Color Color::clamp()
{
	Color resultCl;
	resultCl.c[0] = c[0] < 0.0 ? 0.0 : (c[0] > 1.0 ? 1.0 : c[0]);
	resultCl.c[1] = c[1] < 0.0 ? 0.0 : (c[1] > 1.0 ? 1.0 : c[1]);
	resultCl.c[2] = c[2] < 0.0 ? 0.0 : (c[2] > 1.0 ? 1.0 : c[2]);
	resultCl.c[3] = c[3] < 0.0 ? 0.0 : (c[3] > 1.0 ? 1.0 : c[3]);
	return resultCl;
}

inline void Color::glcolor()
{
	glColor4fv(c);
}

#endif // !defined(AFX_COLOR_H__27EED6A2_8BAE_11D2_83B4_0080C832FCF3__INCLUDED_)
