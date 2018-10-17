// Point.h: interface for the Point class.
//
//////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "math.h"

#if !defined(AFX_POINT_H__B7EFCF01_82AF_11D2_83B4_0080C832FCF3__INCLUDED_)
#define AFX_POINT_H__B7EFCF01_82AF_11D2_83B4_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define   minLimit (float)0.000001

class Point  
{
public:

	Point();
	Point(GLfloat x,GLfloat y,GLfloat z,GLfloat w);
	virtual ~Point();

public:

	inline Point operator=(Point a);
	inline Point operator=(GLfloat *a);
	inline Point operator+(Point a);
	inline Point operator+=(Point a);
	inline Point operator-(Point a);

	inline Point operator*(Point b);
	inline Point operator*(GLfloat b);
	inline Point operator*=(GLfloat b);
	inline Point operator/(GLfloat b);
	inline Point operator/=(GLfloat b);
	inline GLfloat& operator[](int index);

	inline void set(GLfloat x, GLfloat y, GLfloat z, GLfloat w = 1.0);

	inline GLfloat dist(Point b);
	inline GLfloat dot(Point b);
	inline GLfloat dot(GLfloat x, GLfloat y, GLfloat z);
	inline GLfloat mag();
	inline GLfloat magsquared();
	inline Point unit();
	inline void unitize();

	inline Point scale(Point p);

	// 角度为弧度
	inline Point rotate(Point axis, GLfloat angle);
	inline Point rotate(Point axis, GLfloat c, GLfloat s);
	inline void rotate_self(Point axis, GLfloat c, GLfloat s);

	Point rotate_aboutx(GLfloat c, GLfloat s);
	Point rotate_aboutx(GLfloat angle);
	Point rotate_abouty(GLfloat c, GLfloat s);
	Point rotate_abouty(GLfloat angle);
	Point rotate_aboutz(GLfloat c, GLfloat s);
	Point rotate_aboutz(GLfloat angle);
	
	// 透视到o_xy平面
	inline Point project(Point proj_pt);
	inline void project_self(Point proj_pt);
	inline void project_self(GLfloat px, GLfloat py, GLfloat pz);
	inline Point project_direction(Point direction);
	inline Point project_direction(GLfloat x, GLfloat y, GLfloat z);
	
	inline void compute_projected(GLfloat px, GLfloat py, GLfloat pz,
					 GLfloat x, GLfloat y, GLfloat z);

	inline Point refract(Point light, Point N, GLfloat I);
	void refract_self(Point light, Point N, GLfloat I);

	inline void glvertex();
	inline void glnormal();

	Point CalcuNormal(Point point2,Point point3);
	Point refract_direction(Point light, Point N, GLfloat I);
	GLfloat pt[4];

};

#define DOT(a, b) (a.pt[0]*b.pt[0] + a.pt[1]*b.pt[1] + a.pt[2]*b.pt[2])
#define THIS_DOT(b) (pt[0]*b.pt[0] + pt[1]*b.pt[1] + pt[2]*b.pt[2])

inline Point Point::operator=(Point a)
{
	pt[0] = a.pt[0];
	pt[1] = a.pt[1];
	pt[2] = a.pt[2];
	pt[3] = a.pt[3];
	return *this;
}

inline Point Point::operator=(GLfloat *a)
{
	pt[0] = a[0]; 
	pt[1] = a[1]; 
	pt[2] = a[2]; 
	pt[3] = 1;
	return *this;
}

inline Point Point::operator+(Point a)
{ 
	Point resultPt;
	resultPt.pt[0] = pt[0] + a.pt[0];
	resultPt.pt[1] = pt[1] + a.pt[1];
	resultPt.pt[2] = pt[2] + a.pt[2]; 
	return resultPt; 
}

inline Point Point::operator+=(Point a)
{
	pt[0] += a.pt[0];
	pt[1] += a.pt[1];
	pt[2] += a.pt[2];
	return *this;
}

inline Point Point::operator-(Point a)
{
	Point resultPt;
	resultPt.pt[0] = pt[0] - a.pt[0];
	resultPt.pt[1] = pt[1] - a.pt[1];
	resultPt.pt[2] = pt[2] - a.pt[2];
	return resultPt;
}
  
inline Point Point::operator*(Point b)
{
	Point resultPt;
	resultPt.pt[0] = pt[1]*b.pt[2] - b.pt[1]*pt[2];
	resultPt.pt[1] = pt[2]*b.pt[0] - b.pt[2]*pt[0];
	resultPt.pt[2] = pt[0]*b.pt[1] - pt[1]*b.pt[0];
	return resultPt;
}

inline Point Point::operator*(GLfloat b)
{
	Point resultPt;
	resultPt.pt[0] = pt[0] * b;
	resultPt.pt[1] = pt[1] * b;
	resultPt.pt[2] = pt[2] * b;
	return resultPt;
}

inline Point Point::operator*=(GLfloat b)
{
	pt[0] *= b;
	pt[1] *= b;
	pt[2] *= b;
	return *this;
}

inline Point Point::operator/(GLfloat b)
{
	Point resultPt;
	resultPt.pt[0] = pt[0] / b;
	resultPt.pt[1] = pt[1] / b;
	resultPt.pt[2] = pt[2] / b;
	return resultPt;
}

inline Point Point::operator/=(GLfloat b)
{
	pt[0] /= b;
	pt[1] /= b;
	pt[2] /= b;
	return *this;
}

inline GLfloat& Point::operator[](int index)
{
	return pt[index];
}

inline void Point::set(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	pt[0] = x;
	pt[1] = y;
	pt[2] = z;
	pt[3] = w;
}

inline GLfloat Point::dist(Point b)
{
	return (*this - b).mag();
}

inline GLfloat Point::dot(Point b)
{
	return pt[0]*b.pt[0] + pt[1]*b.pt[1] + pt[2]*b.pt[2];
}

inline GLfloat Point::dot(GLfloat x, GLfloat y, GLfloat z)
{ 
	return pt[0]*x + pt[1]*y + pt[2]*z;
}

inline GLfloat Point::mag()
{
	return sqrt(pt[0]*pt[0] + pt[1]*pt[1] + 
              pt[2]*pt[2]);
}

inline GLfloat Point::magsquared()
{
	return pt[0]*pt[0] + pt[1]*pt[1] + pt[2]*pt[2];
}

inline Point Point::unit()
{
	Point resultPt;
	GLfloat m = sqrt(pt[0]*pt[0] + pt[1]*pt[1] + pt[2]*pt[2]);
	resultPt.pt[0] = pt[0] / m;
	resultPt.pt[1] = pt[1] / m;
	resultPt.pt[2] = pt[2] / m;
	return resultPt;
}

inline void Point::unitize()
{
	GLfloat m = sqrt(pt[0]*pt[0] + pt[1]*pt[1] + pt[2]*pt[2]);
	pt[0] /= m;
	pt[1] /= m;
	pt[2] /= m;
}

inline Point Point::scale(Point p) 
{
	Point resultPt;
	resultPt.pt[0] = pt[0] * p.pt[0];
	resultPt.pt[1] = pt[1] * p.pt[1];
	resultPt.pt[2] = pt[2] * p.pt[2];
	return resultPt;
}

inline Point Point::rotate(Point axis, GLfloat angle)
{
	return rotate(axis, cos(angle), sin(angle));
}

inline Point Point::rotate(Point axis, GLfloat c, GLfloat s)
{
	Point resultPt;
	
	axis=axis.unit();	//单位化
	
	float x = axis.pt[0], y = axis.pt[1], z = axis.pt[2], t = 1.0 - c;
	float tx, ty;

	tx = t*x;

	//	通用情形
	if (y == 0.0) {
	resultPt.pt[0] = pt[0]*(tx*x + c) + pt[1]*(-s*z) + pt[2]*(tx*z);
	resultPt.pt[1] = pt[0]*(s*z) + pt[1]*c + pt[2]*(-s*x);
	resultPt.pt[2] = pt[0]*(tx*z) + pt[1]*s*x + pt[2]*(t*z*z + c);
	} else {
	ty = t*y;
	resultPt.pt[0] = pt[0]*(tx*x + c) + pt[1]*(tx*y - s*z) +
	  pt[2]*(tx*z + s*y);
	resultPt.pt[1] = pt[0]*(tx*y + s*z) + pt[1]*(ty*y + c) +
	  pt[2]*(ty*z - s*x);
	resultPt.pt[2] = pt[0]*(tx*z - s*y) + pt[1]*(ty*z + s*x) +
	  pt[2]*(t*z*z + c);
	}
	return resultPt;
}

inline void Point::rotate_self(Point axis, GLfloat c, GLfloat s)
{
	float Px, Py, Pz;
	float x = axis.pt[0], y = axis.pt[1], z = axis.pt[2], t = 1.0 - c;
	float tx, ty;

	tx = t*x;
	Px = pt[0];
	Py = pt[1];
	Pz = pt[2];

	if (!y) {
	pt[0] = Px*(tx*x + c) +	Py*(-s*z) + 	Pz*(tx*z);
	pt[1] = Px*(s*z) + 		Py*c + 		Pz*(-s*x);
	pt[2] = Px*(tx*z) + 	Py*s*x + 	Pz*(t*z*z + c);
	} else {
	ty = t*y;
	pt[0] = Px*(tx*x + c) + 	Py*(tx*y - s*z) +
	  Pz*(tx*z + s*y);
	pt[1] = Px*(tx*y + s*z) + 	Py*(ty*y + c) +
	  Pz*(ty*z - s*x);
	pt[2] = Px*(tx*z - s*y) + 	Py*(ty*z + s*x) +
	  Pz*(t*z*z + c);
	}
}  

inline void Point::glvertex() 
{
	glVertex3fv(pt);
}

inline void Point::glnormal()
{
	glNormal3fv(pt);
}

inline Point Point::project(Point proj_pt)
{
	Point resultPt;
	GLfloat dirx = pt[0] - proj_pt.pt[0], 
	  diry = pt[1] - proj_pt.pt[1],
	  dirz = pt[2] - proj_pt.pt[2];
	GLfloat t;

	if (fabs(dirz) < minLimit) resultPt = *this;
	else {
	t = -proj_pt.pt[2] / dirz;
	resultPt.pt[0] = proj_pt.pt[0] + dirx*t;
	resultPt.pt[1] = proj_pt.pt[1] + diry*t;
	resultPt.pt[2] = 0.0;
	}
	return resultPt;
}


inline void Point::project_self(Point proj_pt)
{
	GLfloat dirx = pt[0] - proj_pt.pt[0], 
	  diry = pt[1] - proj_pt.pt[1],
	  dirz = pt[2] - proj_pt.pt[2];
	GLfloat t;

	t = -proj_pt.pt[2] / dirz;
	pt[0] = proj_pt.pt[0] + dirx*t;
	pt[1] = proj_pt.pt[1] + diry*t;
	pt[2] = 0.0;
}

inline void Point::project_self(GLfloat px, GLfloat py, GLfloat pz)
{
	GLfloat dirx = pt[0] - px, 
	diry = pt[1] - py,
	dirz = pt[2] - pz, t;

	t = -pz / dirz;
	pt[0] = px + dirx*t;
	pt[1] = py + diry*t;
	pt[2] = 0.0;
}

inline Point Point::project_direction(Point direction) 
{
	Point resultPt;
	GLfloat t;

	t = -pt[2] / direction.pt[2];
	resultPt.pt[0] = pt[0] + direction.pt[0]*t;
	resultPt.pt[1] = pt[1] + direction.pt[1]*t;
	resultPt.pt[2] = 0;
	return resultPt;
}

inline Point Point::project_direction(GLfloat x, GLfloat y, GLfloat z) 
{
	Point resultPt;
	GLfloat t;

	t = -pt[2] / z;
	resultPt.pt[0] = pt[0] + x*t;
	resultPt.pt[1] = pt[1] + y*t;
	resultPt.pt[2] = 0;
	return resultPt;
}

inline void Point::compute_projected(GLfloat px, 
				GLfloat py, GLfloat pz,
				GLfloat dx, GLfloat dy, GLfloat dz)
{
	GLfloat t = -pz / dz;
	pt[0] = px + dx*t;
	pt[1] = py + dy*t;
	pt[2] = 0;
}

#endif // !defined(AFX_POINT_H__B7EFCF01_82AF_11D2_83B4_0080C832FCF3__INCLUDED_)
