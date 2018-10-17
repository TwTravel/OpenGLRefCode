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
#define TABLE_POSITION		3.45f			//  �����ĸ߶�
#define LEFT_BUMPER			4.88f			// Left X coordinate of Table Bumper
#define	RIGHT_BUMPER		4.88f			// Right X coordinate of Table Bumper
#define TOP_BUMPER			2.3f			// Top Z coordinate of Table Bumper	-  Ooops model error
#define	BOTTOM_BUMPER		2.45f			// Bottom Z coordinate of Table Bumper

// �������ߴ�
#define BALL_DIAMETER		0.1875f			// ���ֱ��

#define EPSILON				0.00001f		// ERROR TERM
#define DEFAULT_DAMPING		0.002f			//  ȱʡ������
#define	CUE_STICK_FORCE		5.0f			// For Mouse interaction

#define	BALL_COUNT			2
#define	SYSTEM_COUNT		3
#define	MAX_CONTACTS		10

#define OGL_WBALL_DLIST		1			// �������ʾ�б�
#define OGL_YBALL_DLIST		2			// �������ʾ�б�
#define OGL_CUE_DLIST		3			// ��˵���ʾ�б�

#define ART_PATH			"art/"
#define MAX_TEXTURES		255

//  ��ײ����
enum tCollisionTypes
{
	NOT_COLLIDING,			//  û����ײ
	PENETRATING,			//  ����
	COLLIDING_WITH_WALL,	//  ����߽�֮�����ײ
	COLLIDING_WITH_BALL		//  ������֮�����ײ
};

enum tIntegratorTypes
{
	EULER_INTEGRATOR,
	MIDPOINT_INTEGRATOR,
	RK4_INTEGRATOR
};

//  ����Ϸģ��Ĺ������й���ײ�����ݽṹ
typedef struct s_Contact
{
	int		ball,ball2;		// ��������ײʱ�õ���2
    tVector normal;			// ��ײƽ��ķ�����
	int		type;			// ��ײ����
	float	Kr;				// �ָ�ϵ��
} t_Contact;

//  ��ײƽ������ݽṹ
typedef struct s_CollisionPlane
{
	tVector normal;			// ������
    float	d;				// ax + by + cz + d = 0
} t_CollisionPlane;

//  ����������ݽṹ
typedef struct s_Camera
{
	tVector		rot;		//  ��ת
	tVector		trans;		//  ƽ��
	float		fov;		//  �ӳ�
} t_Camera;

// ��˵����ݽṹ
typedef struct s_Cue
{ 
	float	draw,old_draw;	//  �����������ľ���
	float	yaw;			//  ��˵���ת�Ƕ�
	tVector	pos;			//  ��˵�λ��
	float	drawtime;
} t_CueStick;

//  ������ݽṹ
typedef struct s_Ball
{
	tVector pos;			//  ���λ��
    tVector v;				//  ����ٶ�
	tVector f;				//  ���������ϵ���
    tVector angMom;			//	Angular Momentum
    tVector torque;			//	���������ϵ�����
	float	oneOverM;		//  �����ĵ���
	tQuaternion	orientation;
	int		flags;			//  �Ƿ�����ײ״̬
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

// ��������ݽṹ(����Ϊ�ı��κ�������)
typedef struct 
{
	t2DCoord	t1[4],t2[4];
	uint		TexNdx1;
	uint		TexNdx2;
	unsigned short index[4];
	long		type;
	long		color[4];		
} tPrimPoly;

// ���峡�������ݽṹ(�����������κ��ı���)
typedef struct 
{
	long		vertexCnt;		//  ������Ŀ
	tVector		*vertex;		//  ��������
	long		triCnt,quadCnt;	//  �����κ��ı��ε���Ŀ
	tPrimPoly	*tri,*quad;		//  �����κ��ı��ε�����
	char		map[255];		//  ����ӳ��
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
//��ӳ�Ա�������Ա����
	BOOL RenderWorld();
	BOOL SetupPixelFormat(void);
	void SetLogicalPalette(void);
	BOOL InitializeOpenGL(CDC* pDC);

	HGLRC		m_hRC;			//OpenGL����������
	HPALETTE	m_hPalette;		//OpenGL��ɫ��
	CDC*	    m_pDC;			//OpenGL�豸������
/////////////////////////////////////////////////////////////////
	int		g_MouseHitX, g_MouseHitY;		//  ��굥��λ��
	int		g_Dragging;						//  ����Ƿ�Ϊ�϶�״̬
	float	g_LastYaw, g_LastPitch;
	int		g_DrawingStick ;				//  �Ƿ������˵ı�־
	float	g_LastDraw;
	float	g_LastTime;
	int		g_SimRunning;					//  �Ƿ������Ϸ��־
	long	g_TimeIterations;
	int		g_UseFixedTimeStep;				//  �Ƿ�ʹ�ù̶���ʱ�䲽��
	float	g_MaxTimeStep;

	t_CueStick		g_CueStick;				//  һ�����
	t_Ball			g_Ball[2];				//  ������

	float			g_Kd;
	float			g_Kr_Bumper;			//  ����߽���ײ�Ļָ�ϵ��
	float			g_Kr_Ball;				//  ��������ײ�Ļָ�ϵ��

	float			g_Csf;
	float			g_Ckf;
	int				g_IntegratorType;
	int				g_CollisionRootFinding;		// ONLY SET WHEN FINDING A COLLISION
	int				g_UseDamping;				// �Ƿ�ʹ������
	int				g_UseFriction;				// �Ƿ�ʹ��Ħ��
	int				g_CueHitBall;				// ����Ƿ������
	int				g_BallInPlay;				// ���Ƿ����˶�
	tVector			g_CueForce;							
	tVector			g_Gravity;

	t_Contact		g_Contact[MAX_CONTACTS];				// ���ܷ�������ײ
	int				g_ContactCnt;							// ��ײ������
	t_CollisionPlane	g_CollisionPlane[4];				// ��ײƽ����
	int					g_CollisionPlaneCnt;			
	t_Ball			g_GameSys[SYSTEM_COUNT][BALL_COUNT];	// LIST OF PHYSICAL PARTICLES
	t_Ball			*g_CurrentSys,*g_TargetSys;

	t_Camera	g_POV;							// �����
	int			g_TextureCnt;					// װ���������
	t_TexPool	g_TexPool[MAX_TEXTURES];		// ����������Ϣ
	t_Visual	g_Scene;						// ��̬����

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
