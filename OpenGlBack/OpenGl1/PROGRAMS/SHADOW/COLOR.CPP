// Color.cpp: implementation of the Color class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Shadow.h"
#include "Color.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Color::Color()
{
	c[0] = 1.0;
	c[1] = 1.0;
	c[2] = 1.0;
	c[3] = 1.0;

}

Color::~Color()
{

}

void Color::set(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	c[0] = r;
	c[1] = g;
	c[2] = b;
	c[3] = a;

}
