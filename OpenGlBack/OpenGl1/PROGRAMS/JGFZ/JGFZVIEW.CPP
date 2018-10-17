// JgfzView.cpp : implementation of the CJgfzView class
//

#include "stdafx.h"
#include "Jgfz.h"

#include "JgfzDoc.h"
#include "JgfzView.h"

#include <math.h>
#include "my.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////
// CJgfzView

IMPLEMENT_DYNCREATE(CJgfzView, CView)

BEGIN_MESSAGE_MAP(CJgfzView, CView)
	//{{AFX_MSG_MAP(CJgfzView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(IDM_ABSOLUTEMOVEMENT, OnAbsolutemovement)
	ON_COMMAND(IDM_CONTROL_MOVE, OnControlMove)
	ON_COMMAND(IDM_CONTROL_STATIC, OnControlStatic)
	ON_COMMAND(IDM_RELATIVEMOVE_1, OnRelativemove1)
	ON_COMMAND(IDM_TRACE_1, OnTrace1)
	ON_COMMAND(IDM_SPEED_FAST, OnSpeedFast)
	ON_COMMAND(IDM_SPEED_SLOW, OnSpeedSlow)
	ON_COMMAND(IDM_SPEED_ULTRA, OnSpeedUltra)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////
// CJgfzView construction/destruction

CJgfzView::CJgfzView()
{
	m_pDC=NULL;

	m_rotateAngle=0.0f;		// 角度自由度
	m_speedFactor=15.0f;	// 速度选择器
	m_MoveStatic=Static;	// 运动与静止

	m_height=0.84f;			// 两铰之间的距离
	m_bar1Length=0.56f;		// 杆1的长度 
	m_bar2Length=1.7f;		// 杆2的长度
	m_radius=0.019f;		// 杆的半径
	m_taotongLength=0.16f;	// 套筒长度
	m_taotongRadius=0.04f;	// 套筒半径
	m_radiusFlag=TRUE;		// 系统模型自检标志

	m_zizuoCoefficient=20;	// 标准抛物线方程系数
	m_zizuoThick=0.3;		// 支座长度
	m_zizuoWidth1=0.13;		// 支座宽度
	m_zizuoWidth2=0.16;		// 支座宽度
	m_zizuoOffset=0.5;		// 偏移量，决定固定铰的开度

	//	运动类型控制参数
	m_absoluteRelative=AbsoluteMovement; 
	
	m_copyImage=TRUE;		// 图像输出控制
}

CJgfzView::~CJgfzView()
{
}

BOOL CJgfzView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

void CJgfzView::OnDraw(CDC* pDC)
{
	CJgfzDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	DrawScene();
}

#ifdef _DEBUG
void CJgfzView::AssertValid() const
{
	CView::AssertValid();
}

void CJgfzView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CJgfzDoc* CJgfzView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CJgfzDoc)));
	return (CJgfzDoc*)m_pDocument;
}
#endif //_DEBUG

void CJgfzView::Init()
{
	PIXELFORMATDESCRIPTOR pfd;
    int         n;
	HGLRC		hrc;

    m_pDC = new CClientDC(this);

    ASSERT(m_pDC != NULL);

    if (!bSetupPixelFormat())
        return;

    n =::GetPixelFormat(m_pDC->GetSafeHdc());
    ::DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

    hrc = wglCreateContext(m_pDC->GetSafeHdc());
    wglMakeCurrent(m_pDC->GetSafeHdc(), hrc);

    GetClientRect(&m_oldRect);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

BOOL CJgfzView::bSetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd = 
	{
        sizeof(PIXELFORMATDESCRIPTOR), 
        1,                             
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL|
			PFD_DOUBLEBUFFER,		
        PFD_TYPE_RGBA,              
        24,                         
        0, 0, 0, 0, 0, 0,           
        0,                          
        0,                          
        0,                          
        0, 0, 0, 0,                 
        32,                         
        0,                          
        0,                          
        PFD_MAIN_PLANE,             
        0,                          
        0, 0, 0                     
    };
    int pixelformat;

    if ( (pixelformat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd)) == 0 )
    {
        MessageBox("ChoosePixelFormat failed");
        return FALSE;
    }

    if (SetPixelFormat(m_pDC->GetSafeHdc(), pixelformat, &pfd) == FALSE)
    {
        MessageBox("SetPixelFormat failed");
        return FALSE;
    }

    return TRUE;
}

int CJgfzView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	Init();	

	return 0;
}

void CJgfzView::OnDestroy() 
{
	HGLRC	hrc;

	hrc = ::wglGetCurrentContext();

    ::wglMakeCurrent(NULL,  NULL);
	
	KillTimer(1);
    if (hrc)
        ::wglDeleteContext(hrc);

    if (m_pDC)
        delete m_pDC;
	
	CView::OnDestroy();

}

void CJgfzView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	if(cy > 0)
    { 
		if((m_oldRect.right > cx) || (m_oldRect.bottom > cy))
            RedrawWindow();

        m_oldRect.right = cx;
        m_oldRect.bottom = cy;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
	
		EnableLighting();
		
		glFrustum(-1.0,1.0,-1.0,1.0,0.0,7.0);
		glViewport(0, 0, cx, cy);
    }
}

void CJgfzView::DrawScene()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f,0.4f,0.5f,1.0f);
 	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-0.5f);
	
	//背景：
	glPushMatrix();
		DrawBackground();
	glPopMatrix();

	glPushMatrix();
		DrawMove();
	glPopMatrix();

	// 如果显示相对运动，就不画两个支座：

	glPushMatrix();
		if((m_absoluteRelative==AbsoluteMovement)||
			(m_absoluteRelative==RelativeTrace1))
			DrawStatic();
	glPopMatrix();

	glFinish();
	SwapBuffers(wglGetCurrentDC());
}

void CJgfzView::DrawMove()
{
	LENGTHANGLEINFO lengthAngleInfo;//保存第2杆的运动信息

	switch(m_absoluteRelative)
	{
		case RelativeTrace1 :  
			glPushMatrix();
				glRotatef(m_rotateAngle,0.0f,0.0f,1.0f);
				DrawXY();
		
				//	画相对运动类型1的相对轨迹：
				DrawRelativeCurve1();
			glPopMatrix();
			break;
		case RelativeMovement1 :
			glPushMatrix();
				DrawXY();
		
				//	画相对运动类型1的相对轨迹：
				DrawRelativeCurve1();
			glPopMatrix();
			break;
		default:
			break;
	}

	//	完成平面转换，从Oxy到Ozy:
	glRotatef(90.0f,0.0f,1.0f,0.0f);

	//	当显示相对运动时，就不画绝对运动的轨迹：
	if((m_absoluteRelative==AbsoluteMovement)||
			(m_absoluteRelative==RelativeTrace1))
		DrawCircleRoad(m_bar1Length,m_radius,m_rotateAngle,1.0f);

	//	进行融合处理
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE,GL_ZERO);

	//	实现相对运动：
 	switch(m_absoluteRelative)
	{
		case RelativeMovement1 :
			glRotatef(m_rotateAngle,1.0f,0.0f,0.0f);
			break;
		default :
			break;
	}

	////////////////////////////////////////////////////
	////////////         绝对运动思想		////////////
	////////////////////////////////////////////////////

	//		第一根杆
 	glPushMatrix();
		
		//	已经重新设置过系统的坐标中心:	移位，向前
		glTranslatef(m_taotongRadius+m_radius+0.01f,0.0f,0.0f);
		
		//开始实现机构的绝对运动
		glRotatef(-m_rotateAngle,1.0f,0.0f,0.0f);

		//着色挂钩：
		DrawTaogua(m_bar1Length,m_radius);

		GLfloat materialAD[]={1.0f,1.0f,1.0f,0.0f};
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,materialAD);

		//	着色杆1
		DrawBar(BAR_1,m_radius,m_bar1Length);
		glCallList(BAR_1);
	glPopMatrix();

	//	第二根杆
	glPushMatrix();
		//	已经重新设置过系统的坐标中心
		if(m_absoluteRelative!=RelativeMovement3)
				glTranslatef(0.0f,-m_height,0.0f);
		
		//	获取杆2及套筒的运动信息
		if(m_absoluteRelative != RelativeTrace3)
			lengthAngleInfo=ClacuAngleBar2(m_bar1Length,
				m_radius,m_height,m_rotateAngle);

		glRotated(-lengthAngleInfo.bar2RotateAngle,1.0f,0.0f,0.0f);

		GLfloat materialAD2[]={1.0f,1.0f,1.0f,0.0f};
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,materialAD2);
		DrawBar(BAR_2,m_radius,m_bar2Length);
 		glCallList(BAR_2);

		//	套筒:进行融合处理
		glDepthMask(FALSE);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix(); 
			
			//	动画修改区域，套筒的运动：
			glTranslated(0.0,0.0,
				lengthAngleInfo.taotongTranslate
				-m_taotongLength/2.0);
				
			GLfloat materialAD3[]={0.10f,0.5f,0.0f,0.9f};
			GLfloat materialSP[]={0.5f,0.5f,0.5f,0.8f};
			glMaterialfv(GL_FRONT,GL_SPECULAR,materialSP);
			glMaterialf(GL_FRONT,GL_SHININESS,50.0);
			glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,materialAD3);
			DrawBar(TAOTONG, m_taotongRadius,m_taotongLength);
			glCallList(TAOTONG);
		glPopMatrix();

		///////////////////////////////////////////////

		//	如果是相对运动类型1（动点位于杆2上，动系旋转），就显示动点：
		if(	(m_absoluteRelative==RelativeTrace1)||
			(m_absoluteRelative==RelativeMovement1))
		{
			GLfloat ps[2];
			glGetFloatv(GL_POINT_SIZE_RANGE,ps);
			glPointSize(ps[1]/2.0f);
			GLfloat zTranslate=(m_bar1Length+m_radius)*(m_bar1Length+m_radius)
						+m_height*m_height;
			zTranslate=sqrt(zTranslate);
			glTranslatef(0.0f,0.0f,zTranslate);
			
			glBegin(GL_POINTS);
				glVertex3f(m_radius+0.09f,0.0f,0.0f);
			glEnd();
		}

	glPopMatrix();

	////////////////////////////////////////////////

	switch(m_absoluteRelative)
	{
		case RelativeTrace1: 
		case RelativeMovement1 :
			 //		如果不是绝对运动，则着色X,Y轴,
			 //		并进行内部反走样处理：
			glPushMatrix();	
			
				//向前移位，以使坐标轴通过杆1的中心线
				glTranslatef(m_taotongRadius+m_radius+0.01f,
					0.0f,0.0f);
				
				//	如果不是绝对运动，则相对坐标应该运动,
				//	此处仅处理相对运动类型1。
				glRotatef(-m_rotateAngle,1.0f,0.0f,0.0f);
				
				//	显示相对坐标轴 
				DrawCoordinate();
			glPopMatrix();
			break;

		default :
			break;
	}
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void CJgfzView::DrawStatic()
{
	GLdouble yOffset;
	GLdouble zOffset;
	yOffset=m_height-m_zizuoCoefficient
		*m_zizuoWidth1*m_zizuoWidth1/4.0
		+m_radius;
	zOffset=m_zizuoThick+m_radius+m_zizuoOffset/2.0;

	//	画绝对坐标系,其颜色将会受到融合的影响：
	
	if(m_absoluteRelative==AbsoluteMovement)
	{
		glPushMatrix();
			DrawXY();
			
			//	因为坐标系位于Oyz平面
			glRotatef(90.0f,0.0f,1.0f,0.0f);
			DrawCoordinate();
		glPopMatrix();
	}
	
	//	画下支座：
	glPushMatrix();
		glRotatef(3.0f,0.0f,1.0f,0.0f);
		glTranslated(0.0f,-yOffset,-zOffset);
		
		DrawZiZuoXia(m_zizuoCoefficient,m_zizuoThick,
			m_zizuoWidth1,m_zizuoWidth2,m_zizuoOffset,
			m_radius);
		 
		//	画下支座轴承：
		yOffset=-m_zizuoCoefficient*m_zizuoWidth1
			*m_zizuoWidth1/4.0+m_radius;

		//两端加5倍
		glTranslated(0.0,yOffset,-5*m_radius);
	
		GLfloat materialAD3[]={1.0f,1.0f,1.0f,1.0f};
		GLfloat materialSP[]={1.0f,1.0f,1.0f,1.0f};
		glMaterialfv(GL_FRONT,GL_SPECULAR,materialSP);
		glMaterialf(GL_FRONT,GL_SHININESS,100.0);
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,materialAD3);
		
		
		DrawBar(ZOUCHENG_X,m_radius+0.006f,2.0f*(zOffset+5*m_radius));
 		glCallList(ZOUCHENG_X);

		//	表面处理，(代码可以进一步向 DrawBar() 优化)：
	
		GLfloat materialAD1[]={1.0f,1.0f,1.0f,1.0f};
		GLfloat materialSP1[]={1.0f,1.0f,1.0f,1.0f};
		glMaterialfv(GL_FRONT,GL_SPECULAR,materialSP1);
		glMaterialf(GL_FRONT,GL_SHININESS,100.0);
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,materialAD1);

		glTranslated(0.0,0.0,2.0*(zOffset+5*m_radius));
	 
		GLUquadricObj* quadObj;
		quadObj=gluNewQuadric();
		gluQuadricDrawStyle(quadObj,GLU_FILL);
		gluQuadricOrientation(quadObj,GLU_OUTSIDE);
	 
		gluDisk(quadObj,0.0,m_radius+0.006,10,1);
	
	glPopMatrix();

	//	画上支座：
	glPushMatrix();
		glRotatef(3.0f,0.0f,1.0f,0.0f);

		//	画上支座底盘（已经包含表面处理)：
		GLfloat materialAD5[]={0.7f,0.7f,0.7f,1.0f};
		GLfloat materialSP5[]={1.0f,1.0f,1.0f,1.0f};
		glMaterialfv(GL_FRONT,GL_SPECULAR,materialSP5);
		glMaterialf(GL_FRONT,GL_SHININESS,100.0);
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,
			materialAD5);

		DrawBar(ZIZUOUP_1,4*m_radius,10*m_radius);
		glCallList(ZIZUOUP_1);

		//	画上支座顶盘（已经包含表面处理)：
		glPushMatrix();
			glTranslatef(0.0f,0.0f,-14*m_radius);
			DrawBar(ZIZUOUP_2,3.5*m_radius,8*m_radius);
			glCallList(ZIZUOUP_2);
		glPopMatrix();
	
		//	画上支座轴承（已经包含表面处理)：
		GLfloat materialAD6[]={0.9f,0.9f,0.9f,1.0f};
		GLfloat materialSP6[]={1.0f,1.0f,1.0f,1.0f};
		glMaterialfv(GL_FRONT,GL_SPECULAR,materialSP6);
		glMaterialf(GL_FRONT,GL_SHININESS,100.0);
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,
			materialAD6);

		glPushMatrix();
			glTranslatef(0.0f,0.0f,-20*m_radius);
			DrawBar(ZOUCHENG_S,1.5*m_radius,12*m_radius);
			glCallList(ZOUCHENG_S);
		glPopMatrix();

	glPopMatrix();
}

void CJgfzView::DrawCoordinate()
{
	GLfloat xLength=m_bar1Length+12*m_radius;

	if(m_absoluteRelative==RelativeTrace2)
		xLength=m_bar1Length;

	GLfloat yLength=25*m_radius;
 
	GLfloat lw[2];
	glGetFloatv(GL_LINE_WIDTH_RANGE,lw);
	glLineWidth(lw[1]/12.0);

	 //  如果只显示绝对运动坐标系，就没有必要进行反走样：
	if(m_absoluteRelative!= AbsoluteMovement)
	{
		glShadeModel(GL_FLAT);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);
	}
 	glDisable(GL_LIGHTING);
	glColor3f(0.0f,0.0f,1.0f);
 
	//位于 Ozy 坐标:
	glBegin(GL_LINES);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,xLength);

		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,yLength,0.0f);
	glEnd();

	//坐标箭头：
 	glLineWidth(lw[1]/12.0);

	//Y方向箭头：
	glPushMatrix();
		glTranslatef(0.0f,yLength,0.0f);
		glBegin(GL_LINES);	
			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(0.0f,-0.065f,0.01f);

			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(0.0f,-0.065f,-0.01f);
		glEnd();
	glPopMatrix();

	//	Z方向箭头，但事实上显示为X方向。
	glPushMatrix();
		glTranslatef(0.0f,0.0f,xLength);
		glBegin(GL_LINES);	
			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(0.0f,0.01f,-0.065f);

			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(0.0f,-0.01f,-0.065f);
		glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);

	//	在绝对运动的情况下，没有进行坐标轴反走样：
	if(m_absoluteRelative!= AbsoluteMovement)
	{
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_BLEND);
	}
}

void CJgfzView::DrawXY()
{
	//X',Y'字符位于坐标原点：
	CClientDC dc(this);
	
 	SelectObject(dc.m_hDC,GetStockObject(SYSTEM_FONT));
	
	glDisable(GL_LIGHTING);
	glColor3f(1.0f,1.0f,1.0f);
	glPushMatrix();
		if(m_absoluteRelative==RelativeTrace2)
			glTranslatef(m_bar1Length+1.5*m_radius +0.015f,0.0f,0.0f);
		else
			glTranslatef(m_bar1Length+13.5*m_radius +0.015f,-0.05f,0.0f);

		int iListGen=glGenLists(6);
		wglUseFontBitmaps(dc.m_hDC,0,256,1000);
		glRasterPos3f(0.0F,0.0F,0.0F);
		glListBase(1000);

		if(m_absoluteRelative==AbsoluteMovement)
			glCallLists(1,GL_UNSIGNED_BYTE,"x");
		else
			glCallLists(2,GL_UNSIGNED_BYTE,"x'");
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0.0f,26.5*m_radius +0.015f,0.0f);
		wglUseFontBitmaps(dc.m_hDC,0,256,iListGen+3);
		glRasterPos3f(0.0F,0.0F,0.0F);
		glListBase(iListGen+3);
		if(m_absoluteRelative==AbsoluteMovement)
			glCallLists(1,GL_UNSIGNED_BYTE,"y");
		else
			glCallLists(2,GL_UNSIGNED_BYTE,"y'");
	glPopMatrix();

	glEnable(GL_LIGHTING);	
}

void CJgfzView::DrawBackground()
{
	//	画面的艺术处理：
}

void CJgfzView::DrawRelativeCurve1()
{
	VERTEXCOORDINATE coord;
	GLdouble initLength=sqrt((m_bar1Length+m_radius)
		*(m_bar1Length+m_radius)+m_height*m_height);

	GLdouble step=0.5;	//	角度

	GLfloat lw[2];
	glGetFloatv(GL_LINE_WIDTH_RANGE,lw);
	glLineWidth(lw[1]/8.0);

	glDisable(GL_LIGHTING);

 	glBegin(GL_LINE_STRIP);
		glColor3f(0.5f,1.0f,0.2f);
		glVertex3d(m_bar1Length+m_radius,0.0,0.0);
		for(GLdouble i=0.0;i<=m_rotateAngle;i+=step)
		{
			coord=DrawRelative1Point(m_height,
				m_bar1Length,
				m_radius,initLength,i);
			glVertex3d(coord.xCoord,coord.yCoord,0.0);
		}
	glEnd();

	glEnable(GL_LIGHTING);
}

void CJgfzView::EnableLighting()
{
	GLfloat light0Ambient[]={0.1f,0.1f,0.1f,1.0f};
	GLfloat light0Diffuse[]={0.8f,0.8f,0.8f,1.0f};
	GLfloat light0Position[]={0.0f,1.0f,2.0f,1.0f};

	glLightfv(GL_LIGHT0,GL_AMBIENT,light0Ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light0Diffuse);
	glLightfv(GL_LIGHT0,GL_POSITION,light0Position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void CJgfzView::OnTimer(UINT nIDEvent) 
{
	 m_rotateAngle+=m_speedFactor;
	 if(m_rotateAngle>360.0f)
	 {
		 m_rotateAngle=0.0f;
		 OnControlStatic();
	 }
	 Invalidate(FALSE);

	CView::OnTimer(nIDEvent);	
}

void CJgfzView::OnAbsolutemovement() 
{
	KillTimer(1);
	m_rotateAngle=0.0f;
	m_absoluteRelative=AbsoluteMovement;
	Invalidate(FALSE);
}

void CJgfzView::OnControlMove() 
{
	if(m_MoveStatic==Move)	return;
	m_MoveStatic=Move;
	m_radius=0.0185f;
	SetTimer(1,50,0);
	Invalidate(FALSE);
}

void CJgfzView::OnControlStatic() 
{
	if(m_MoveStatic==Static)	return;
	m_MoveStatic=Static;
	KillTimer(1);
	Invalidate(FALSE);
}

void CJgfzView::OnRelativemove1() 
{
	m_rotateAngle=0.0f;
	m_absoluteRelative=RelativeMovement1;
	
	KillTimer(1);
	Invalidate(FALSE);
}

void CJgfzView::OnTrace1() 
{
	m_absoluteRelative=RelativeTrace1;
	m_rotateAngle=0.0f;
	KillTimer(1);
	Invalidate(FALSE);
}

void CJgfzView::OnSpeedFast() 
{
	m_speedFactor=25.0f;
	Invalidate(FALSE);
}

void CJgfzView::OnSpeedSlow() 
{
	m_speedFactor=10.0f;
	Invalidate(FALSE);
}

void CJgfzView::OnSpeedUltra() 
{
	m_speedFactor=35.0f;
	Invalidate(FALSE);
}
