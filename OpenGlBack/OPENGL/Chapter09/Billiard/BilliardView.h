// BilliardView.h : interface of the CBilliardView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYSDOPENGLVIEW_H__75C5AAEC_37B0_4A8B_9132_9A0C663F6DDC__INCLUDED_)
#define AFX_MYSDOPENGLVIEW_H__75C5AAEC_37B0_4A8B_9132_9A0C663F6DDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "mathdefs.h"
#include "LoadTGA.h"
#include <time.h>
#include <mmsystem.h>
#include <assert.h>

// This sets up the table position in the world
// TODO: Should load with the model....?
#define TABLE_POSITION		3.45f			//  球桌的高度
#define LEFT_BUMPER			4.88f			// Left X coordinate of Table Bumper
#define	RIGHT_BUMPER		4.88f			// Right X coordinate of Table Bumper
#define TOP_BUMPER			2.3f			// Top Z coordinate of Table Bumper	-  Ooops model error
#define	BOTTOM_BUMPER		2.45f			// Bottom Z coordinate of Table Bumper

// 球的物理尺寸
#define BALL_DIAMETER		0.1875f			// 球的直径

#define EPSILON				0.00001f		// ERROR TERM
#define DEFAULT_DAMPING		0.002f			//  缺省的阻尼
#define	CUE_STICK_FORCE		5.0f			// For Mouse interaction

#define	BALL_COUNT			2
#define	SYSTEM_COUNT		3
#define	MAX_CONTACTS		10

#define OGL_WBALL_DLIST		1			// 白球的显示列表
#define OGL_YBALL_DLIST		2			// 黄球的显示列表
#define OGL_CUE_DLIST		3			// 球杆的显示列表

#define ART_PATH			"art/"
#define MAX_TEXTURES		255

//  碰撞类型
enum tCollisionTypes
{
	NOT_COLLIDING,			//  没有碰撞
	PENETRATING,			//  穿过
	COLLIDING_WITH_WALL,	//  球与边界之间的碰撞
	COLLIDING_WITH_BALL		//  球与球之间的碰撞
};

enum tIntegratorTypes
{
	EULER_INTEGRATOR,
	MIDPOINT_INTEGRATOR,
	RK4_INTEGRATOR
};

//  在游戏模拟的过程中有关碰撞的数据结构
typedef struct s_Contact
{
	int		ball,ball2;		// 当两球碰撞时用到球2
    tVector normal;			// 碰撞平面的法向量
	int		type;			// 碰撞类型
	float	Kr;				// 恢复系数
} t_Contact;

//  碰撞平面的数据结构
typedef struct s_CollisionPlane
{
	tVector normal;			// 法向量
    float	d;				// ax + by + cz + d = 0
} t_CollisionPlane;

//  摄像机的数据结构
typedef struct s_Camera
{
	tVector		rot;		//  旋转
	tVector		trans;		//  平移
	float		fov;		//  视场
} t_Camera;

// 球杆的数据结构
typedef struct s_Cue
{ 
	float	draw,old_draw;	//  球杆向后拉动的距离
	float	yaw;			//  球杆的旋转角度
	tVector	pos;			//  球杆的位置
	float	drawtime;
} t_CueStick;

//  球的数据结构
typedef struct s_Ball
{
	tVector pos;			//  球的位置
    tVector v;				//  球的速度
	tVector f;				//  作用于球上的力
    tVector angMom;			//	Angular Momentum
    tVector torque;			//	作用于球上的力矩
	float	oneOverM;		//  质量的倒数
	tQuaternion	orientation;
	int		flags;			//  是否处于碰撞状态
} t_Ball;

#define POLY_SELECTED	1
#define POLY_TEXTURED	2

typedef struct s_TexPool
{
	char		map[255];
	GLuint		glTex;
	byte		*data;
	int			type;
}t_TexPool;

// 多边形数据结构(可以为四边形和三角形)
typedef struct 
{
	t2DCoord	t1[4],t2[4];
	uint		TexNdx1;
	uint		TexNdx2;
	unsigned short index[4];
	long		type;
	long		color[4];		
} tPrimPoly;

// 定义场景的数据结构(包含有三角形和四边形)
typedef struct 
{
	long		vertexCnt;		//  顶点数目
	tVector		*vertex;		//  顶点数据
	long		triCnt,quadCnt;	//  三角形和四边形的数目
	tPrimPoly	*tri,*quad;		//  三角形和四边形的数据
	char		map[255];		//  纹理映射
} t_Visual;

class CBilliardView : public CView
{
protected: // create from serialization only
	CBilliardView();
	DECLARE_DYNCREATE(CBilliardView)

// Attributes
public:
	CBilliardDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBilliardView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBilliardView();
/////////////////////////////////////////////////////////////////
//添加成员函数与成员变量
	BOOL RenderWorld();
	BOOL SetupPixelFormat(void);
	void SetLogicalPalette(void);
	BOOL InitializeOpenGL(CDC* pDC);

	HGLRC		m_hRC;			//OpenGL绘制描述表
	HPALETTE	m_hPalette;		//OpenGL调色板
	CDC*	    m_pDC;			//OpenGL设备描述表
/////////////////////////////////////////////////////////////////
	int		g_MouseHitX, g_MouseHitY;		//  鼠标单击位置
	int		g_Dragging;						//  鼠标是否为拖动状态
	float	g_LastYaw, g_LastPitch;
	int		g_DrawingStick ;				//  是否绘制球杆的标志
	float	g_LastDraw;
	float	g_LastTime;
	int		g_SimRunning;					//  是否进入游戏标志
	long	g_TimeIterations;
	int		g_UseFixedTimeStep;				//  是否使用固定的时间步长
	float	g_MaxTimeStep;

	t_CueStick		g_CueStick;				//  一个球杆
	t_Ball			g_Ball[2];				//  两个球

	float			g_Kd;
	float			g_Kr_Bumper;			//  球与边界碰撞的恢复系数
	float			g_Kr_Ball;				//  球与球碰撞的恢复系数

	float			g_Csf;
	float			g_Ckf;
	int				g_IntegratorType;
	int				g_CollisionRootFinding;		// ONLY SET WHEN FINDING A COLLISION
	int				g_UseDamping;				// 是否使用阻尼
	int				g_UseFriction;				// 是否使用摩擦
	int				g_CueHitBall;				// 球杆是否击中球
	int				g_BallInPlay;				// 球是否在运动
	tVector			g_CueForce;							
	tVector			g_Gravity;

	t_Contact		g_Contact[MAX_CONTACTS];				// 可能发生的碰撞
	int				g_ContactCnt;							// 碰撞计数器
	t_CollisionPlane	g_CollisionPlane[4];				// 碰撞平面数
	int					g_CollisionPlaneCnt;			
	t_Ball			g_GameSys[SYSTEM_COUNT][BALL_COUNT];	// LIST OF PHYSICAL PARTICLES
	t_Ball			*g_CurrentSys,*g_TargetSys;

	t_Camera	g_POV;							// 摄像机
	int			g_TextureCnt;					// 装入的纹理数
	t_TexPool	g_TexPool[MAX_TEXTURES];		// 保存纹理信息
	t_Visual	g_Scene;						// 静态场景

	int			tx,ty;
	float		magnitude;
	CLoadTGA	m_LoadTGA;
	CMathDefs	m_MathDefs;

///////////////////////////////////////////////////////////////////////////////

	BOOL InitGame(void);
	void InitRender(void);
	void LoadTextures();
	void LoadSceneFile(char *filename);
	void SetupViewRC(void);
	void RenderCueAndBalls();
	void RenderScene();
	void SetupBalls();
	void ComputeForces( t_Ball	*system);
	void IntegrateSysOverTime(t_Ball *initial,t_Ball *source, t_Ball *target, float deltaTime);
	void EulerIntegrate( float DeltaTime);
	void MidPointIntegrate( float DeltaTime);
	int CheckForCollisions( t_Ball	*system );
	void ResolveCollisions( t_Ball	*system );
	void Simulate(float DeltaTime, BOOL running);
	float GetTime( void );

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CBilliardView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in BilliardView.cpp
inline CBilliardDoc* CBilliardView::GetDocument()
   { return (CBilliardDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSDOPENGLVIEW_H__75C5AAEC_37B0_4A8B_9132_9A0C663F6DDC__INCLUDED_)
