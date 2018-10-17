// Point.cpp: implementation of the Point class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Shadow.h"
#include "Point.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Point::Point()
{

}

Point::Point(GLfloat x,GLfloat y,GLfloat z,GLfloat w)
{
	 pt[0] = x;
	 pt[1] = y;
	 pt[2] = z;
	 pt[3] = w;

}

Point::~Point()
{

}

Point Point::rotate_aboutx(GLfloat c, GLfloat s)
{
	Point resultPt;

	resultPt.pt[0]=pt[0];
	resultPt.pt[1] = c*pt[1] - s*pt[2];
	resultPt.pt[2] = s*pt[1] + c*pt[2];
	return resultPt;

}

Point Point::rotate_aboutx(GLfloat angle)
{
	Point resultPt;
	GLfloat c=cos(angle);
	GLfloat s=sin(angle);

	resultPt.pt[0]=pt[0];
	resultPt.pt[1] = c*pt[1] - s*pt[2];
	resultPt.pt[2] = s*pt[1] + c*pt[2];
	return resultPt;

}


Point Point::rotate_abouty(GLfloat c, GLfloat s)
{
	Point resultPt;
	resultPt.pt[0] = c*pt[0] + s*pt[2];
	resultPt.pt[1] = pt[1];
	resultPt.pt[2] = -s*pt[0] + c*pt[2];
	return resultPt;
}

Point Point::rotate_abouty(GLfloat angle)
{
	Point resultPt;
	GLfloat c=cos(angle);
	GLfloat s=sin(angle);

	resultPt.pt[0] = c*pt[0] + s*pt[2];
	resultPt.pt[1] = pt[1];
	resultPt.pt[2] = -s*pt[0] + c*pt[2];
	return resultPt;

}

Point Point::rotate_aboutz(GLfloat c, GLfloat s)
{
	Point resultPt;
	resultPt.pt[0] = c*pt[0] - s*pt[1];
	resultPt.pt[1] = s*pt[0] + c*pt[1];
	resultPt.pt[2] = pt[2];
	return resultPt;
}

Point Point::rotate_aboutz(GLfloat angle)
{
	Point resultPt;
	GLfloat c=cos(angle);
	GLfloat s=sin(angle);

	resultPt.pt[0] = c*pt[0] - s*pt[1];
	resultPt.pt[1] = s*pt[0] + c*pt[1];
	resultPt.pt[2] = pt[2];
	return resultPt;

}

void Point::refract_self(Point light, Point N, GLfloat I)
{
	GLfloat t;
	Point dlight;

	dlight = refract_direction(light, N, I);
	t = -pt[2] / dlight.pt[2]; 
	pt[0] = pt[0] + dlight.pt[0]*t;
	pt[1] = pt[1] + dlight.pt[1]*t;
	pt[2] = 0;
}

Point Point::refract_direction(Point light, Point N, GLfloat I) 
{
	Point resultPt;
	GLfloat cos1, sin1, cos2, sin2, m;
	GLfloat dlight[3], dN[3], axis[3];

	/* dlight = (light - *this).unit() * -1.0; */ 
	dlight[0] = pt[0] - light.pt[0]; 
	dlight[1] = pt[1] - light.pt[1]; 
	dlight[2] = pt[2] - light.pt[2]; 
	m = sqrt(dlight[0]*dlight[0] + dlight[1]*dlight[1] + dlight[2]*dlight[2]);
	dlight[0] /= m;
	dlight[1] /= m;
	dlight[2] /= m;

	// dN = N * -1.0; 
	dN[0] = -N.pt[0]; 
	dN[1] = -N.pt[1]; 
	dN[2] = -N.pt[2]; 

	// cos1 = dN.dot(dlight); 
	cos1 = dN[0]*dlight[0] + dN[1]*dlight[1] + dN[2]*dlight[2];

	if (1.0 - cos1*cos1 < minLimit) {
	resultPt = dN;
	return resultPt;
	}

	// axis = ((dN * dlight) * dN).unit(); 
	resultPt.pt[0] = dN[1]*dlight[2] - dlight[1]*dN[2];
	resultPt.pt[1] = dN[2]*dlight[0] - dlight[2]*dN[0];
	resultPt.pt[2] = dN[0]*dlight[1] - dN[1]*dlight[0];
	axis[0] = resultPt.pt[1]*dN[2] - dN[1]*resultPt.pt[2];
	axis[1] = resultPt.pt[2]*dN[0] - dN[2]*resultPt.pt[0];
	axis[2] = resultPt.pt[0]*dN[1] - resultPt.pt[1]*dN[0];
	m = sqrt(axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2]);
	axis[0] /= m;
	axis[1] /= m;
	axis[2] /= m;

	if (axis[0]*axis[0] > minLimit)  
	sin1 = (dlight[0] - dN[0] * cos1) / axis[0]; 
	else if (axis[1]*axis[1] > minLimit)  
	sin1 = (dlight[1] - dN[1] * cos1) / axis[1]; 
	else sin1 = dlight[2] - dN[2] * cos1; 

	sin2 = sin1 / I; 
	cos2 = (sin1*sin1 < 1.0) ? sqrt(1.0 - sin2*sin2) : 0; 

	dlight[0] = dN[0]*cos2 + axis[0]*sin2;
	dlight[1] = dN[1]*cos2 + axis[1]*sin2;
	dlight[2] = dN[2]*cos2 + axis[2]*sin2;

	/* I'm not sure this is quite legal */ 
	if (dlight[2] > 0.0) dlight[2] = -dlight[2];   

	resultPt = dlight;

	return resultPt;
}

Point Point::CalcuNormal(Point point2, Point point3)
{
	Point resultPt;
	Point vector1,vector2,point1;

	point1.pt[0]=pt[0];
	point1.pt[1]=pt[1];
	point1.pt[2]=pt[2];

	vector1=point2 - point1;
	vector2=point3 - point1;

	resultPt=vector1*vector2;

	resultPt.unitize();

	return resultPt;

}

