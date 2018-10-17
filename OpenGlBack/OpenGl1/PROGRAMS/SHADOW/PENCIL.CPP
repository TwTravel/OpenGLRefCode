// Pencil.cpp: implementation of the Pencil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Shadow.h"
#include "Pencil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define PI	3.14159265358979323846

inline float radians(float a) {return PI * a / 180.0; }

inline float sign(float a) {return (a > 0. ? 1 : (a < 0. ? -1 : 0)); }


Pencil::Pencil()
{
	radius=0.1;
	zHeightB=0.1;	 
	zHeightT=0.5; 

	translation.set(0, 0, 0);
	rotation_x = 0;
	rotation_y = 0;
	rotation_z = 0;
	dimensions.set(1, 1, 1);

	erosion = 0;

	points_valid = 0;
	transforms_valid = 0;

	holder_a.set(0.3,0.3,0.3);
	holder_d.set(1.0,0.15,0.0);
	holder_s.set(0.9,0.9,0.9);
	neb_cap_a.set(0.3,0.3,0.3);
	neb_cap_d.set(0.55,0.3,0.25);
	neb_cap_s.set(0.7,0.7,0.7);
	shadow_c.set(0.05,0.05,0.05,0.95);
	penu_c.set(0.15,0.15,0.15,0.75);

}

Pencil::~Pencil()
{

}

Pencil Pencil::operator=(Pencil pencil)
{
	int i;

	translation = pencil.translation;
	rotation_x = pencil.rotation_x;
	rotation_y = pencil.rotation_y;
	rotation_z = pencil.rotation_z;
	dimensions = pencil.dimensions;

	radius=pencil.radius;
	zHeightB=pencil.zHeightB;	
	zHeightT=pencil.zHeightT;	

	erosion = pencil.erosion;

	if (pencil.points_valid && pencil.transforms_valid) 
	{
		for (i = 0; i < 13; i++) 
		{
			PencilPs[i]=pencil.PencilPs[i];
			PencilNs[i]=pencil.PencilNs[i];
		}
	} 
	else 
	{
		points_valid = 0;
		transforms_valid = 0;
	}

	return *this;
}

void Pencil::GeneratePoints()
{
	float c, s, x, y;
	float fAngle=-30.0;

	int i=0;

	PencilPs[0].set(0,0,0);

	for(i=1;i<7;i++)
	{
		c = cos(radians(fAngle));
		s = sin(radians(fAngle));
		x=radius*c;
		y=radius*s;
		PencilPs[i].set(x,y,zHeightB);
		PencilPs[i+6].set(x,y,zHeightT);
		fAngle+=60.0;
	}
}

void Pencil::GenerateNormals()
{
	float x, y;
	float fAngle=0.0;

	int i=0;

	for(i=0;i<6;i++)
	{
		x = cos(radians(fAngle));
		y = sin(radians(fAngle));
		PencilNs[i+6].set(x,y,0.0);
		fAngle+=60.0;
	}
	
	PencilNs[0]=PencilPs[1].CalcuNormal(PencilPs[0],PencilPs[2]);
	PencilNs[1]=PencilPs[2].CalcuNormal(PencilPs[0],PencilPs[3]);
	PencilNs[2]=PencilPs[3].CalcuNormal(PencilPs[0],PencilPs[4]);
	PencilNs[3]=PencilPs[4].CalcuNormal(PencilPs[0],PencilPs[5]);
	PencilNs[4]=PencilPs[5].CalcuNormal(PencilPs[0],PencilPs[6]);
	PencilNs[5]=PencilPs[6].CalcuNormal(PencilPs[0],PencilPs[1]);

	PencilNs[12].set(0.0,0.0,1.0);
}

void Pencil::after_transform_Ns(Point* resultNs)
{
	resultNs[0]=PencilPs[1].CalcuNormal(PencilPs[0],PencilPs[2]);
	resultNs[1]=PencilPs[2].CalcuNormal(PencilPs[0],PencilPs[3]);
	resultNs[2]=PencilPs[3].CalcuNormal(PencilPs[0],PencilPs[4]);
	resultNs[3]=PencilPs[4].CalcuNormal(PencilPs[0],PencilPs[5]);
	resultNs[4]=PencilPs[5].CalcuNormal(PencilPs[0],PencilPs[6]);
	resultNs[5]=PencilPs[6].CalcuNormal(PencilPs[0],PencilPs[1]);
	
	resultNs[6]=PencilPs[1].CalcuNormal(PencilPs[2],PencilPs[8]);
	resultNs[7]=PencilPs[2].CalcuNormal(PencilPs[3],PencilPs[9]);
	resultNs[8]=PencilPs[3].CalcuNormal(PencilPs[4],PencilPs[10]);
	resultNs[9]=PencilPs[4].CalcuNormal(PencilPs[5],PencilPs[11]);
	resultNs[10]=PencilPs[5].CalcuNormal(PencilPs[6],PencilPs[12]);
	resultNs[11]=PencilPs[6].CalcuNormal(PencilPs[1],PencilPs[7]);

	resultNs[12]=PencilPs[7].CalcuNormal(PencilPs[9],PencilPs[11]);

}

void Pencil::SetPencil(GLfloat fRadius, GLfloat zHeight1, GLfloat zHeight2)
{
	GLfloat fTemp;
	if(zHeight1>zHeight2)
	{
		fTemp=zHeight1;
		zHeight1=zHeight2;
		zHeight2=fTemp;
	}
	radius=fRadius;
	zHeightB=zHeight1;	 
	zHeightT=zHeight2;
	
	GeneratePoints();	
	GenerateNormals();
}

void Pencil::ResetPencil()
{
	translation.set(0, 0, 0);
	rotation_x = 0;
	rotation_y = 0;
	rotation_z = 0;
	dimensions.set(1, 1, 1);

	erosion = 0;

	points_valid = 0;
	transforms_valid = 0;

	GeneratePoints();	
	GenerateNormals();
}

void Pencil::set_dimensions(GLfloat x, GLfloat y, GLfloat z)
{
	dimensions.set(x, y, z);
	points_valid = 0;
}

void Pencil::set_dimensions(Point p)
{
	dimensions = p;
	points_valid = 0;
}

Point Pencil::get_dimensions()
{
	return dimensions;
}

void Pencil::erode(float eroding)
{
	erosion += eroding;
	points_valid = 0;

}

float Pencil::get_erosion()
{
	return erosion;
}

void Pencil::translate_parameter(GLfloat x, GLfloat y, GLfloat z)
{
	translation.pt[0]+=x;
	translation.pt[1]+=y;
	translation.pt[2]+=z;
	points_valid=0;
}

void Pencil::translate_parameter(Point transPt)
{
	translation+=transPt;
	points_valid=0;
}

void Pencil::rotate_angle_x(GLfloat anglex)
{
	rotation_x += anglex;
	points_valid=0;
}

void Pencil::rotate_angle_y(GLfloat angley)
{
	rotation_y += angley;
	points_valid=0;
}

void Pencil::rotate_angle_z(GLfloat anglez)
{
	rotation_z += anglez;
	points_valid=0;
}

Point Pencil::transform_point(Point point)
{
	Point resultPt,tempPt;
	tempPt = point.scale(dimensions);
	tempPt += translation;
	resultPt = tempPt.rotate_aboutz(rotation_z);
	tempPt = resultPt.rotate_abouty(rotation_y);
	resultPt = tempPt.rotate_aboutx(rotation_x);
	return resultPt;
}

void Pencil::transform_pencilPts()
{
	int i;

	if (points_valid && transforms_valid) return;
	points_valid = 0;

	GeneratePoints();

	for (i = 0; i < 13; i++) 
	  PencilPs[i]=transform_point(PencilPs[i]);

//	after_transform_Ns();//此时法线可以不必变换

	points_valid = 0;
	transforms_valid = 1;
}

void Pencil::transform_points(Point * p)
{
	for (int i = 0; i < 13; i++) 
	  p[i]=transform_point(p[i]);

}

void Pencil::draw()
{
	GeneratePoints();	

	//一旦调用该函数，顶点值和法线向量值将是变换后的值。
	//先缩放，后平移，再绕Ｚ，Ｙ，Ｘ轴旋转。
	
	transform_pencilPts();
	after_transform_Ns(PencilNs);
	draw_holder_cap_neb();
}

void Pencil::draw_quads(Point p1, Point p2, Point p3, Point p4)
{
	glBegin(GL_QUADS);
		glVertex3fv(p1.pt);
		glVertex3fv(p2.pt);
		glVertex3fv(p3.pt);
		glVertex3fv(p4.pt);
	glEnd();
}

void Pencil::draw_quads(Point p1, Point p2, Point p3, Point p4, 
			int shadow_penumbra)
{
	shadow_penumbra=1;

	glBegin(GL_QUADS);
		material_shadow();
		glVertex3fv(p1.pt);
		glVertex3fv(p2.pt);
		
		material_penumbra();
		glVertex3fv(p3.pt);
		glVertex3fv(p4.pt);
	glEnd();

}


void Pencil::draw_triangle(Point p1,Point p2,Point p3)
{
	glBegin(GL_TRIANGLES);
		glVertex3fv(p1.pt);
		glVertex3fv(p2.pt);
		glVertex3fv(p3.pt);
	glEnd();

}

void Pencil::draw_shadow(Point sun_direction)
{
	Point p[13];

	int i;

	//	transform_pencilPts();
	//由于先绘制绘制铅笔，所以，不必进行变换。
	for(i=0;i<13;i++)
	{
		p[i]=PencilPs[i];
		if(p[i].pt[2] < 0.0)p[i].pt[2]=0.0;
		p[i]=p[i].project_direction(sun_direction);
		
	}

	draw_holder_cap_neb(p,1);

}

void Pencil::draw_holder_cap_neb(Point * p, int flat)
{
	GLfloat fN[]={0.0,0.0,1.0};

	//draw holder
	material_holder();
	if(flat)material_shadow();

	glNormal3fv(PencilNs[6].pt);
	if(flat)glNormal3fv(fN);
	draw_quads(p[1],p[2],p[8],p[7]);

	glNormal3fv(PencilNs[7].pt);
	if(flat)glNormal3fv(fN);
	draw_quads(p[2],p[3],p[9],p[8]);

	glNormal3fv(PencilNs[8].pt);
	if(flat)glNormal3fv(fN);
	draw_quads(p[3],p[4],p[10],p[9]);

	glNormal3fv(PencilNs[9].pt);
	if(flat)glNormal3fv(fN);
	draw_quads(p[4],p[5],p[11],p[10]);

	glNormal3fv(PencilNs[10].pt);
	if(flat)glNormal3fv(fN);
	draw_quads(p[5],p[6],p[12],p[11]);

	glNormal3fv(PencilNs[11].pt);
	if(flat)glNormal3fv(fN);
	draw_quads(p[6],p[1],p[7],p[12]);

	//draw cap
	material_cap_neb();
	if(flat)material_shadow();

	glBegin(GL_POLYGON);
		glNormal3fv(PencilNs[12].pt);
		if(flat)glNormal3f(0.0f,0.0f,1.0f);
		glVertex3fv(p[7].pt);
		glVertex3fv(p[8].pt);
		glVertex3fv(p[9].pt);
		glVertex3fv(p[10].pt);
		glVertex3fv(p[11].pt);
		glVertex3fv(p[12].pt);
	glEnd();

	//draw neb
	glNormal3fv(PencilNs[0].pt);
	if(flat)glNormal3fv(fN);
	draw_triangle(p[2],p[1],p[0]);

	glNormal3fv(PencilNs[1].pt);
	if(flat)glNormal3fv(fN);
	draw_triangle(p[3],p[2],p[0]);

	glNormal3fv(PencilNs[2].pt);
	if(flat)glNormal3fv(fN);
	draw_triangle(p[4],p[3],p[0]);

	glNormal3fv(PencilNs[3].pt);
	if(flat)glNormal3fv(fN);
	draw_triangle(p[5],p[4],p[0]);

	glNormal3fv(PencilNs[4].pt);
	if(flat)glNormal3fv(fN);
	draw_triangle(p[6],p[5],p[0]);

	glNormal3fv(PencilNs[5].pt);
	if(flat)glNormal3fv(fN);
	draw_triangle(p[1],p[6],p[0]);

}

void Pencil::draw_holder_cap_neb(int flat)
{
	draw_holder_cap_neb(PencilPs,flat);
}

void Pencil::draw_shadow(Point sun_direction, GLfloat blur)
{

	Point p[13];
	Point ptBlur[13];
	Point n[13];
	Point scale_blur;

	int yes_or_no[13];

	int i;

	if(blur==0)
	{
		draw_shadow(sun_direction);
		return;
	}


	//生成原始顶点,（等作工作：管理原始顶点可减少重复计算）
	GeneratePoints();		
	
	for(i=0;i<13;i++)	ptBlur[i]=PencilPs[i];

	//	注意，重载函数draw_shadow（）函数不能同时使用
	//	假定对铅笔的几何点还没有执行几何变换
	transform_pencilPts();

	//将点透视到平面
	for(i=0;i<13;i++)
	{
		p[i]=PencilPs[i];
		if(p[i].pt[2] < 0.0)p[i].pt[2]=0.0;
		p[i]=p[i].project_direction(sun_direction);
	
		scale_blur=dimensions;
		scale_blur.pt[0] += blur * (ptBlur[i].pt[2] + translation.pt[2]);
		scale_blur.pt[1] += blur * (ptBlur[i].pt[2] + translation.pt[2]);
		scale_blur.pt[2] += blur * (ptBlur[i].pt[2] + translation.pt[2]);
		ptBlur[i] = ptBlur[i].scale(scale_blur);
		ptBlur[i] = transform_point(ptBlur[i]);
		if(ptBlur[i].pt[2] < 0.0)ptBlur[i].pt[2]=0.0;
		ptBlur[i]=ptBlur[i].project_direction(sun_direction);
	}

	//	几何变换后的法线：
	after_transform_Ns(n);

	//	决定画与不画阴影（背光的一面不画）
	if(sun_direction.pt[2] > 0.0)sun_direction *= -1.0;
	for(i=0;i<13;i++)
		yes_or_no[i]=(sun_direction.dot(n[i])<0.001);

	//绘制真实阴影

	GLfloat fN[]={0.0,0.0,1.0};
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glColor3f(0.25f,0.25f,0.25f);
	glNormal3fv(fN);


	//	neb
	if(yes_or_no[0])
		draw_triangle(p[2],p[1],p[0]);
	if(yes_or_no[1])
		draw_triangle(p[3],p[2],p[0]);
	if(yes_or_no[2])
		draw_triangle(p[4],p[3],p[0]);
	if(yes_or_no[3])
		draw_triangle(p[5],p[4],p[0]);
	if(yes_or_no[4])
		draw_triangle(p[6],p[5],p[0]);
	if(yes_or_no[5])
		draw_triangle(p[1],p[6],p[0]);

	//holder
	if(yes_or_no[6])
		draw_quads(p[1],p[2],p[8],p[7]);
	if(yes_or_no[7])
		draw_quads(p[2],p[3],p[9],p[8]);
	if(yes_or_no[8])
		draw_quads(p[3],p[4],p[10],p[9]);
	if(yes_or_no[9])
		draw_quads(p[4],p[5],p[11],p[10]);
	if(yes_or_no[10])
		draw_quads(p[5],p[6],p[12],p[11]);
	if(yes_or_no[11])
		draw_quads(p[6],p[1],p[7],p[12]);

	//cap
	if(yes_or_no[12])
	{
		glBegin(GL_POLYGON);
			glVertex3fv(p[7].pt);
			glVertex3fv(p[8].pt);
			glVertex3fv(p[9].pt);
			glVertex3fv(p[10].pt);
			glVertex3fv(p[11].pt);
			glVertex3fv(p[12].pt);
		glEnd();
	}

	//绘制半影（penumbra）
	//绘制半影的技术较复杂，此处作一些简化
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//	cap
	if(yes_or_no[12])
	{
		if(!yes_or_no[6])
			draw_quads(p[7],p[8],ptBlur[8],ptBlur[7],1);
		if(!yes_or_no[7])
			draw_quads(p[8],p[9],ptBlur[9],ptBlur[8],1);
		if(!yes_or_no[8])
			draw_quads(p[9],p[10],ptBlur[10],ptBlur[9],1);
		if(!yes_or_no[9])
			draw_quads(p[10],p[11],ptBlur[11],ptBlur[10],1);
		if(!yes_or_no[10])
			draw_quads(p[11],p[12],ptBlur[12],ptBlur[11],1);
		if(!yes_or_no[11])
			draw_quads(p[12],p[7],ptBlur[7],ptBlur[12],1);
	}
	
	//holder
	if ((yes_or_no[6] || yes_or_no[7]) 
			&& !(yes_or_no[6] && yes_or_no[7])) 
		draw_quads(p[2],p[8],ptBlur[8],ptBlur[2],1);

	if ((yes_or_no[7] || yes_or_no[8]) 
			&& !(yes_or_no[7] && yes_or_no[8])) 
		draw_quads(p[3],p[9],ptBlur[9],ptBlur[3],1);

	if ((yes_or_no[8] || yes_or_no[9]) 
			&& !(yes_or_no[8] && yes_or_no[9])) 
		draw_quads(p[4],p[10],ptBlur[10],ptBlur[4],1);

	if ((yes_or_no[9] || yes_or_no[10]) 
			&& !(yes_or_no[9] && yes_or_no[10])) 
		draw_quads(p[5],p[11],ptBlur[11],ptBlur[5],1);

	if ((yes_or_no[10] || yes_or_no[11]) 
			&& !(yes_or_no[10] && yes_or_no[11])) 
		draw_quads(p[6],p[12],ptBlur[12],ptBlur[6],1);

	if ((yes_or_no[11] || yes_or_no[6]) 
			&& !(yes_or_no[11] && yes_or_no[6])) 
		draw_quads(p[1],p[7],ptBlur[7],ptBlur[1],1);

	//neb
	if ((yes_or_no[0] || yes_or_no[1]) 
			&& !(yes_or_no[0] && yes_or_no[1])) 
		draw_quads(p[0],p[2],ptBlur[2],ptBlur[0],1);

	if ((yes_or_no[1] || yes_or_no[2]) 
			&& !(yes_or_no[1] && yes_or_no[2])) 
		draw_quads(p[0],p[3],ptBlur[3],ptBlur[0],1);

	if ((yes_or_no[2] || yes_or_no[3]) 
			&& !(yes_or_no[2] && yes_or_no[3])) 
		draw_quads(p[0],p[4],ptBlur[4],ptBlur[0],1);

	if ((yes_or_no[3] || yes_or_no[4]) 
			&& !(yes_or_no[3] && yes_or_no[4])) 
		draw_quads(p[0],p[5],ptBlur[5],ptBlur[0],1);

	if ((yes_or_no[4] || yes_or_no[5]) 
			&& !(yes_or_no[4] && yes_or_no[5])) 
		draw_quads(p[0],p[6],ptBlur[6],ptBlur[0],1);

	if ((yes_or_no[5] || yes_or_no[0]) 
			&& !(yes_or_no[5] && yes_or_no[0])) 
		draw_quads(p[0],p[1],ptBlur[1],ptBlur[0],1);
	
	glDisable(GL_BLEND);
}

void Pencil::material_holder()
{
	glColor4fv(holder_d.c);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,holder_a.c);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,holder_d.c);
	glMaterialfv(GL_FRONT,GL_SPECULAR,holder_s.c);
	glMaterialf(GL_FRONT,GL_SHININESS,60);
}

void Pencil::material_cap_neb()
{
	glColor4fv(neb_cap_d.c);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,neb_cap_a.c);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,neb_cap_d.c);
	glMaterialfv(GL_FRONT,GL_SPECULAR,neb_cap_s.c);
	glMaterialf(GL_FRONT,GL_SHININESS,60);
}

void Pencil::material_shadow()
{
	GLfloat mat_amb[]={0.15f,0.15f,0.15f,1.0f};
	GLfloat mat_spe[]={0.7f,0.7f,0.7f,1.0f};
	glColor4fv(shadow_c.c);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mat_amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,shadow_c.c);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_spe);
	glMaterialf(GL_FRONT,GL_SHININESS,30);

}

void Pencil::material_penumbra()
{
	glColor4fv(penu_c.c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, penu_c.c);

}

void Pencil::set_holder_mat(GLfloat* a, GLfloat* d, GLfloat* s)
{
	holder_a=a;
	holder_d=d;
	holder_s=s;

}

void Pencil::set_neb_cap_mat(GLfloat* a, GLfloat* d, GLfloat* s)
{
	neb_cap_a=a;
	neb_cap_d=d;
	neb_cap_s=s;
}

void Pencil::set_shadow_penu_mat(GLfloat* shad_clr, GLfloat* penu_clr)
{
	shadow_c=shad_clr;
	penu_c=penu_clr;
}

