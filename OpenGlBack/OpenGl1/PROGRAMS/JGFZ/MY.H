
//	my.h:

#include "gl\glaux.h"

#define PI	3.14159265358979323846
#define RADIANS(x)	(x)*PI/180.0

typedef struct vector
{
	GLdouble xCoord,yCoord,zCoord;
}VERTEXCOORDINATE;

typedef struct lengthangle
{
	GLdouble bar2RotateAngle,taotongTranslate;
}LENGTHANGLEINFO;	

//画圆环面
void torus(GLdouble innerRadius,GLdouble OuterRadius,int numc, int numt)
{
	int i, j, k;
	double s, t, x, y, z, xn,yn,zn,twopi,temp;
	double innerR,outerR;
		
	innerR=innerRadius;
	outerR=OuterRadius;

	if(outerR<innerR)
	{
		temp=outerR;
		outerR=innerR;
		innerR=temp;
	}
	twopi = 2 * PI;
	for (i = 0; i < numc; i++) 
	{
		glBegin(GL_QUAD_STRIP);
			for (j = 0; j <= numt; j++) 
			{
				 for (k = 1; k >= 0; k--) 
				 {
					s = (i + k) % numc + 0.5;
					t = j % numt;
			
					xn = cos(s*twopi/numc)*cos(t*twopi/numt);
					yn = cos(s*twopi/numc)*sin(t*twopi/numt);
					zn = sin(s * twopi / numc);
				
					x = (outerR+innerR*cos(s*twopi/numc))*cos(t*twopi/numt);
					y = (outerR+innerR*cos(s*twopi/numc))*sin(t*twopi/numt);
					z = innerR* sin(s * twopi / numc);

					glNormal3d(xn,yn,zn);
					glVertex3d(x, y, z);
				}
			}
		glEnd();
	}
}

//	画杆：
void DrawBar(UINT nLists,GLfloat m_radius,GLfloat m_length)
{
		GLUquadricObj* quadObj;
		glNewList(nLists,GL_COMPILE);
			quadObj=gluNewQuadric();
			gluQuadricDrawStyle(quadObj,GLU_FILL);
			gluQuadricOrientation(quadObj,GLU_INSIDE);
			gluQuadricNormals(quadObj,GLU_SMOOTH);
			switch(nLists)
			{
			case 1: case 5:
				gluCylinder(quadObj,m_radius,m_radius,m_length,10,1);
				break;
			case 2:
				gluCylinder(quadObj,m_radius,m_radius,m_length,8,1);
				break;
			case 3:
				gluCylinder(quadObj,m_radius,m_radius,m_length,15,1);
				break;
			case 6: case  7: case 8:
				gluCylinder(quadObj,m_radius,m_radius,m_length,15,1);
				gluQuadricOrientation(quadObj,GLU_OUTSIDE);
				glPushMatrix();
				gluDisk(quadObj,0.0,m_radius,15,1);
				glTranslatef(0.0f,0.0f,m_length);
				gluDisk(quadObj,0.0,m_radius,15,1);
				glPopMatrix();
				break;
		
			default:
				break;	// do nothing at all.
			}
		glEndList();

		gluDeleteQuadric(quadObj);
}

//	画套钩：
void DrawTaogua(GLfloat m_bar1Length,GLfloat m_radius)
{
	GLfloat materialAD[]={1.0f,0.0f,0.0f,1.0f};
	GLfloat materialSP[]={0.90f,0.9f,0.9f,1.0f};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,materialAD);
	glMaterialfv(GL_FRONT,GL_SPECULAR,materialSP);
	glMaterialf(GL_FRONT,GL_SHININESS,100.0);

 	glPushMatrix();
		glTranslatef(0.0f,0.0f,m_bar1Length+m_radius);
		glRotatef(92.0f,0.0f,1.0f,0.0f);
		torus(m_radius/2.0,m_radius,32,32);
	glPopMatrix();
}

//	返回第二杆旋转的角度及套筒需要移动的大小：
LENGTHANGLEINFO ClacuAngleBar2(GLdouble bar1Length,
		GLdouble radius,GLdouble height,
		GLdouble rotateAngle)
{
	LENGTHANGLEINFO lengthAngle;
	GLdouble rotateRad;
	GLdouble yAxis;
	GLdouble xAxis;
	GLdouble angle;
	GLdouble length;

	//	由于运动的特点以及OpenGL的机制，须对其进行分段处理
	if(rotateAngle<=90.0)
	{
		rotateRad=RADIANS(rotateAngle);
		yAxis=(bar1Length+radius)*sin(rotateRad)+height;
		xAxis=(bar1Length+radius)*cos(rotateRad);
		angle=atan2(yAxis,xAxis);
		angle=angle*180.0/PI;

		length=sqrt(yAxis*yAxis+xAxis*xAxis);
		
		//	杆2旋转的角度
		lengthAngle.bar2RotateAngle=angle; 

		//	套筒移动的大小
		lengthAngle.taotongTranslate=length;
	}
	else if(rotateAngle<=180.0)
	{
		GLdouble minusAngle;
		minusAngle=180.0-rotateAngle;
		rotateRad=RADIANS(minusAngle);
		yAxis=(bar1Length+radius)*sin(rotateRad)+height;
		xAxis=(bar1Length+radius)*cos(rotateRad);
		angle=atan2(yAxis,xAxis);
		angle=angle*180.0/PI;

		length=sqrt(yAxis*yAxis+xAxis*xAxis);
		lengthAngle.bar2RotateAngle=180.0-angle; 
		lengthAngle.taotongTranslate=length;
	}
	else if(rotateAngle<=270.0)
	{
		GLdouble minusAngle;
		minusAngle=270.0-rotateAngle;
		rotateRad=RADIANS(minusAngle);
		yAxis=height-(bar1Length+radius)*cos(rotateRad);
		xAxis=(bar1Length+radius)*sin(rotateRad);
		angle=atan2(yAxis,xAxis);
		angle=angle*180.0/PI;
		
		length=sqrt(yAxis*yAxis+xAxis*xAxis);
		lengthAngle.bar2RotateAngle=180.0-angle; 
		lengthAngle.taotongTranslate=length;
	}
	else if(rotateAngle<=360.0)
	{
		GLdouble minusAngle;
		minusAngle=360.0-rotateAngle;
		rotateRad=RADIANS(minusAngle);
		yAxis=height-(bar1Length+radius)*sin(rotateRad);
		xAxis=(bar1Length+radius)*cos(rotateRad);
		angle=atan2(yAxis,xAxis);
		angle=angle*180.0/PI;

		length=sqrt(yAxis*yAxis+xAxis*xAxis);
		lengthAngle.bar2RotateAngle=angle; 
		lengthAngle.taotongTranslate=length;
	}
	return lengthAngle;
}

//画绝对轨迹：
void DrawCircleRoad(GLdouble bar1Length,GLdouble radius,
				GLdouble rotateAngle,GLfloat sign)
{
	//	参数 rotateAngle 是当前旋转的角度
	//	杆1的长度加上挂钩的半径才是曲柄的真正长度：

	GLdouble step=0.01;
	GLdouble length=bar1Length+radius;
	GLfloat lw[2];
	GLdouble rotateRad=RADIANS(rotateAngle);

	glGetFloatv(GL_LINE_WIDTH_RANGE,lw);
	glLineWidth(lw[1]/8.0);

	glDisable(GL_LIGHTING);
	glColor3f(1.0f,1.0f,0.0f);
 	glBegin(GL_LINE_STRIP);
		
	for(GLdouble i=0.0;i<=rotateRad;i+=step)
			glVertex3f(0.0f,sign*length*sin(i),sign*length*cos(i));
	glEnd();

	glEnable(GL_LIGHTING);
 
}

//	绘制下支座：
void DrawZiZuoXia(GLdouble zizuoCoefficient,
		GLdouble zizuoThick,GLdouble zizuoWidth1,
		GLdouble zizuoWidth2,GLdouble zizuoOffset,
		GLdouble radius)
{
	GLfloat materialAD[]={0.5f,0.50f,0.50f,1.0f};

	GLfloat materialSP[]={0.90f,0.9f,0.9f,1.0f};
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,materialAD);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,materialSP);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100.0);

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glFrontFace(GL_CCW);
	GLdouble  xAxis=zizuoWidth1/2.0;
	GLdouble  yAxis=-zizuoCoefficient*xAxis*xAxis;

	double i;

	glPushMatrix();
	glTranslatef(0.0f,0.0f,zizuoThick-0.05f);
	glBegin(GL_POLYGON);
		glNormal3f(0.0f,0.0f,1.0f);
		for(i=xAxis;i>=-xAxis;i-=0.001)
		{
			 yAxis=-zizuoCoefficient*i*i;
			glVertex3d(i,yAxis,0.0f);			
		}
		yAxis=-zizuoCoefficient*xAxis*xAxis;
		glVertex3d(-xAxis,yAxis,0.0f);
	glEnd();
	glPopMatrix();
	
	GLfloat materialAD2[]={1.0f,1.0f,1.0f,1.0f};
	GLfloat materialSP2[]={0.90f,0.9f,0.9f,1.0f};
	
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,materialAD2);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,materialSP2);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100.0);

	//	第一部分曲面：（简化，没有给出曲面精确法向量）
 	glBegin(GL_QUAD_STRIP);
		for(i=xAxis;i>=-xAxis;i-=0.001)		
		{
			yAxis=-zizuoCoefficient*i*i;
			glVertex3d(i,yAxis,0.0f);
			glVertex3d(i,yAxis,zizuoThick);
		}
	glEnd();

	GLdouble zBack=zizuoThick+2*radius+zizuoOffset;
	GLdouble zFront=zBack+zizuoThick;

	//	第二部分曲面：
 	glBegin(GL_QUAD_STRIP);
		for(i=xAxis;i>=-xAxis;i-=0.001)		
		{
			yAxis=-zizuoCoefficient*i*i;
			glVertex3d(i,yAxis,zBack);
			glVertex3d(i,yAxis,zFront);
		}
	glEnd();

	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,materialAD);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,materialSP);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100.0);

	//	第二部分后表面：
	glBegin(GL_POLYGON);
		glNormal3f(0.0f,0.0f,-1.0f);
		for(i=xAxis;i>=-xAxis;i-=0.001)
		{
			 yAxis=-zizuoCoefficient*i*i;
			glVertex3d(i,yAxis,zBack);			
		}
		yAxis=-zizuoCoefficient*xAxis*xAxis;
		glVertex3d(-xAxis,yAxis,zBack);
	glEnd();

	//	第二部分前表面：
	glBegin(GL_POLYGON);
		glNormal3f(0.0f,0.0f,1.0f);
		for(i=xAxis;i>=-xAxis;i-=0.001)
		{
			 yAxis=-zizuoCoefficient*i*i;
			glVertex3d(i,yAxis,zBack+zizuoThick);			
		}
		yAxis=-zizuoCoefficient*xAxis*xAxis;
		glVertex3d(-xAxis,yAxis,zBack+zizuoThick);
	glEnd();

	//	计算结合部位的切线，以使表面光滑。计算节点坐标：
	GLdouble tangent=zizuoCoefficient*zizuoWidth1;
	GLdouble  yAxis2=-zizuoCoefficient*xAxis*xAxis;
	yAxis=yAxis2-(zizuoWidth2-zizuoWidth1)*tangent;

	//	台体后表面：
	glBegin(GL_POLYGON);
		glVertex3d(zizuoWidth2/2.0,yAxis,0.0f);
		glVertex3d(zizuoWidth1/2.0,yAxis2,0.0f);	
		glVertex3d(-zizuoWidth1/2.0,yAxis2,0.0f);	
		glVertex3d(-zizuoWidth2/2.0,yAxis,0.0f);
	glEnd();

	//	台体前表面：
	glBegin(GL_POLYGON);
		glVertex3d(zizuoWidth2/2.0,yAxis,zFront);
		glVertex3d(zizuoWidth1/2.0,yAxis2,zFront);	
		glVertex3d(-zizuoWidth1/2.0,yAxis2,zFront);	
		glVertex3d(-zizuoWidth2/2.0,yAxis,zFront);
	glEnd();

	//	台体侧翼及上下表面：
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,materialAD2);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,materialSP2);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100.0);

	glBegin(GL_QUAD_STRIP);
		glVertex3d(zizuoWidth2/2.0,yAxis,0.0f);
		glVertex3d(zizuoWidth2/2.0,yAxis,zFront);

		glVertex3d(zizuoWidth1/2.0,yAxis2,0.0f);
		glVertex3d(zizuoWidth1/2.0,yAxis2,zFront);	

		glVertex3d(-zizuoWidth1/2.0,yAxis2,0.0f);	
		glVertex3d(-zizuoWidth1/2.0,yAxis2,zFront);	

		glVertex3d(-zizuoWidth2/2.0,yAxis,0.0f);
		glVertex3d(-zizuoWidth2/2.0,yAxis,zFront);
	glEnd();

}

void CircleFill(GLdouble radius)
{
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glFrontFace(GL_CCW);
		double degree=0.0;
		double radians=0.0;
		glBegin(GL_POLYGON);
			glVertex3d(radius,0.0,0.0);
			
			for(int i=1;i<=36;i++)
			{
				degree+=10.0;

				radians=RADIANS(degree);
				glVertex3d(radius*cos(radians),radius*sin(radians),0.0);
			}

		glEnd();
}

//	获取动点位置：
VERTEXCOORDINATE DrawRelative1Point(GLdouble height,GLdouble bar1Length,
				GLdouble radius,GLdouble initLength,GLdouble rotateAngle)
{
	//	变量rotateAngle以角度表示
	GLdouble mnLength;
	GLdouble omLength;
	GLdouble radiusConst;

	GLdouble cosAngle;
	GLdouble sinAngle;

	VERTEXCOORDINATE coord;
	LENGTHANGLEINFO lengthAngle;

	radiusConst=bar1Length+radius;

	//	获得摆杆（杆2）的运动信息：
	lengthAngle=ClacuAngleBar2(bar1Length,radius,height,rotateAngle);

	GLdouble rotateBar1=RADIANS(rotateAngle);
	GLdouble rotateBar2=RADIANS(lengthAngle.bar2RotateAngle);

	//	下面对当前角度进行判断：
	if(rotateBar1<=PI/2.0)
	{
		mnLength=lengthAngle.taotongTranslate-initLength;
		omLength=height*height+initLength*initLength 
			-2.0*height*initLength*cos(3.14158265/2.0-rotateBar2);
		omLength=sqrt(omLength);

		cosAngle=(radiusConst *radiusConst+omLength*omLength
			-mnLength*mnLength)/(2*radiusConst*omLength);
		sinAngle=1.0-cosAngle*cosAngle;

		sinAngle=sqrt(sinAngle);

		coord.xCoord=omLength*cosAngle;
		coord.yCoord=-omLength*sinAngle;
	}
	else if(rotateBar1<=PI)
	{
		mnLength=lengthAngle.taotongTranslate-initLength;
		omLength=height*height+initLength*initLength 
			-2.0*height*initLength*cos(rotateBar2-PI/2.0);
		omLength=sqrt(omLength);

		cosAngle=(radiusConst *radiusConst+omLength*omLength
			-mnLength*mnLength)/(2*radiusConst*omLength);
		sinAngle=1.0-cosAngle*cosAngle;

		sinAngle=sqrt(sinAngle);

		coord.xCoord=omLength*cosAngle;
		coord.yCoord=omLength*sinAngle;
	}
	else if(rotateBar1<=PI*1.5)
	{
		mnLength=initLength-lengthAngle.taotongTranslate;
		omLength=height*height+initLength*initLength 
			-2.0*height*initLength*cos(rotateBar2-PI/2.0);
		omLength=sqrt(omLength);

		cosAngle=(radiusConst *radiusConst+omLength*omLength
			-mnLength*mnLength)/(2*radiusConst*omLength);
		sinAngle=1.0-cosAngle*cosAngle;

		sinAngle=sqrt(sinAngle);

		coord.xCoord=omLength*cosAngle;
		coord.yCoord=-omLength*sinAngle;
	}
	else 
	{
		mnLength=initLength-lengthAngle.taotongTranslate;
		omLength=height*height+initLength*initLength 
			-2.0*height*initLength*cos(PI/2.0-rotateBar2);
		omLength=sqrt(omLength);

		cosAngle=(radiusConst *radiusConst+omLength*omLength
			-mnLength*mnLength)/(2*radiusConst*omLength);
		sinAngle=1.0-cosAngle*cosAngle;

		sinAngle=sqrt(sinAngle);

		coord.xCoord=omLength*cosAngle;
		coord.yCoord=omLength*sinAngle;
	}
	coord.zCoord=0.0;
	return coord;
}


