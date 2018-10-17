// Pencil.h: interface for the Pencil class.
//
//////////////////////////////////////////////////////////////////////

#include "Point.h"
#include "Color.h"

#if !defined(AFX_PENCIL_H__CF4EB3C1_8956_11D2_83B4_0080C832FCF3__INCLUDED_)
#define AFX_PENCIL_H__CF4EB3C1_8956_11D2_83B4_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class Pencil  
{
public:
	Pencil();
	virtual		~Pencil();
	Pencil		operator=(Pencil pencil);
public:

	void	set_shadow_penu_mat(GLfloat* shad_clr, GLfloat* penu_clr);
	void	set_neb_cap_mat(GLfloat* a, GLfloat* d, GLfloat* s);
	void	set_holder_mat(GLfloat* a, GLfloat* d, GLfloat* s);
	void	draw_shadow(Point sun_direction, GLfloat blur);
	void	draw_shadow(Point sun_direction);
	void	draw();

	void	rotate_angle_z(GLfloat anglez);
	void	rotate_angle_y(GLfloat angley);
	void	rotate_angle_x(GLfloat anglex);
	void	translate_parameter(Point transPt);
	void	translate_parameter(GLfloat x,GLfloat y,GLfloat z);
	float	get_erosion();
	void	erode(float eroding);

	Point	get_dimensions();
	void	set_dimensions(Point p);
	void	set_dimensions(GLfloat x, GLfloat y, GLfloat z);
	void	ResetPencil();
	void	SetPencil(GLfloat fRadius, GLfloat zHeight1, GLfloat zHeight2);

private:

	void	material_penumbra();
	void	material_shadow();
	void	material_cap_neb();
	void	material_holder();

	void	after_transform_Ns(Point* resultNs);
	void	GeneratePoints();
	void	GenerateNormals();
	void	draw_holder_cap_neb(Point* p,int flat=0);

	void	transform_points(Point* p);
	void	transform_pencilPts();
	inline	Point transform_point(Point point);	

	void	draw_holder_cap_neb(int flat=0);
	void	draw_triangle(Point p1,Point p2,Point p3);
	void	draw_quads(Point p1, Point p2, Point p3,
				Point p4,int shadow_penumbra);
	void	draw_quads(Point p1,Point p2,Point p3,Point p4);

private:

	Point	PencilPs[13];
	Point	PencilNs[13];

	Point	translation;
	Point	dimensions;
	GLfloat rotation_x,rotation_y,rotation_z;

	GLfloat radius;
	GLfloat zHeightB;	//Bottom
	GLfloat zHeightT;	//Top

	GLfloat erosion;
	int		points_valid;
	int		transforms_valid;

	Color	holder_a,	holder_d,	holder_s;
	Color	neb_cap_a,	neb_cap_d,	neb_cap_s;
	Color	shadow_c,	penu_c;

};

#endif // !defined(AFX_PENCIL_H__CF4EB3C1_8956_11D2_83B4_0080C832FCF3__INCLUDED_)
