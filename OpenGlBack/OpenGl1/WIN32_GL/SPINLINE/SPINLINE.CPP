#include <windows.h>
#include <math.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>

/* Windows globals, defines, and prototypes */
CHAR szAppName[]="SpinLine";
HWND  ghWnd;
HDC   ghDC;
HGLRC ghRC;

#define SWAPBUFFERS SwapBuffers(ghDC)
#define WIDTH           400
#define HEIGHT          320
#define LENGTH          1.0f
#define PIE				3.14159265
#define PLATELENGTH		4.3f	

//////////////////////////////////////////////////////////////////////////////

LONG WINAPI MainWndProc (HWND, UINT, WPARAM, LPARAM);

BOOL bSetupPixelFormat(HDC);

/* OpenGL globals, defines, and prototypes */

#define SEAT	1
#define AXIS	2
#define PLATE	3

#define LIGHTON	9


int AxisVisible = 1;
GLfloat P[9], Range;
GLfloat fRotateAngle=0.0;
GLfloat fEndAngle=3*PIE;

///////////////////////////////////////////////////////////////////////////


GLvoid GetP(LPSTR lpCmd);
GLvoid resize(GLsizei, GLsizei);
GLvoid initializeGL(GLsizei, GLsizei);
GLvoid drawScene(GLvoid);
GLvoid drawSpinLine(GLfloat fAngle,GLfloat fRadius);
 
GLvoid GetP(LPSTR lpCmd)
{
	char comstr[1024] = "\0", *other;
	int num;

	strcpy(comstr, lpCmd);
	for (num = 1; num <9; num++)
	{
		P[num] = (GLfloat) atof(comstr);
		if((other = strchr(comstr, 32)) != NULL)
		{	
			other++;
			strcpy(comstr, other);
		}
		else
			break;
	}

	return;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG        msg;
    WNDCLASS   wndclass;

	P[1] = 0.5f; P[2] = -0.7f;  //dU(1), dU(2)
	P[3] = 1.0f; P[4] = 0.2f;	//C1, C2
	P[5] = 0.56f; P[6] = 1.78f;	//W1, W2
	P[7] = -1.56f; P[8] = -1.50f;	//A1, A2

	if(strlen(lpCmdLine) != 0)
		GetP(lpCmdLine);

    /* Register the frame class */
    wndclass.style         = 0;

    wndclass.lpfnWndProc   = (WNDPROC)MainWndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon (hInstance, szAppName);
    wndclass.hCursor       = LoadCursor (NULL,IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wndclass.lpszMenuName  = szAppName;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass (&wndclass) )
        return FALSE;

    /* Create the frame */

    ghWnd = CreateWindow (szAppName,
             "车轮滚动——旋轮线的形成",
	     WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
             CW_USEDEFAULT,
             CW_USEDEFAULT,
             WIDTH,
             HEIGHT,
             NULL,
             NULL,
             hInstance,
             NULL);

    /* make sure window was created */
    if (!ghWnd)
        return FALSE;

    /* show and update main window */
//	SetWindowPos(NULL,HWND_TOP,0,0,400,320,SWP_SHOWWINDOW|SWP_NOMOVE);

    ShowWindow (ghWnd, nCmdShow);

    UpdateWindow (ghWnd);

    /* animation loop */
    while (1) {

        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE)
        {
            if (GetMessage(&msg, NULL, 0, 0) )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else {
                return TRUE;
            }
        }
    }
}

/* main window procedure */
LONG WINAPI MainWndProc (
    HWND    hWnd,
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam)
{
    LONG    lRet = 1;
    PAINTSTRUCT    ps;
    RECT rect;
	int iTimer;
	GLfloat dMax1 = P[3] + P[4], dMax2 = P[3] * P[1] + P[4] * P[2];

    switch (uMsg) {

    case WM_CREATE:
        ghDC = GetDC(hWnd);
        if (!bSetupPixelFormat(ghDC))
            PostQuitMessage (0);

        ghRC = wglCreateContext(ghDC);
        wglMakeCurrent(ghDC, ghRC);
        GetClientRect(hWnd, &rect);
        initializeGL(rect.right, rect.bottom);

		if(dMax1 == 0 || dMax2 == 0)
			Range = 0.1f;
		else
		{
			if(dMax1 >= dMax2)
				Range = 0.1f / dMax1;
			else
				Range = 0.1f / dMax2;
	//		iTimer = SetTimer(hWnd, 1, 50, NULL);
		}
        break;

    case WM_TIMER:
 
		PostMessage(hWnd, WM_PAINT, NULL, NULL);
		break;

    case WM_PAINT:
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
		drawScene();
        break;

    case WM_SIZE:
        GetClientRect(hWnd, &rect);
        resize(rect.right, rect.bottom);
        break;

    case WM_CLOSE:
        if (ghRC)
            wglDeleteContext(ghRC);
        if (ghDC)
            ReleaseDC(hWnd, ghDC);
        ghRC = 0;
        ghDC = 0;

        DestroyWindow (hWnd);
        break;

    case WM_DESTROY:
        if (ghRC)
            wglDeleteContext(ghRC);
        if (ghDC)
            ReleaseDC(hWnd, ghDC);

        PostQuitMessage (0);
        break;
  
    case WM_KEYDOWN:

        switch (wParam) {

 
		case VK_INSERT:
 
			break;
        case VK_LEFT:
			fEndAngle=1.5*PIE;
            break;
        case VK_RIGHT:
   
            break;
        case VK_UP:
             
            break;
        case VK_DOWN:
             
            break;
        case VK_HOME:
             
            break;
        case VK_END:
            
            break;

        }

    default:
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam);
        break;
    }

    return lRet;
}

BOOL bSetupPixelFormat(HDC hdc)
{
    PIXELFORMATDESCRIPTOR pfd, *ppfd;
    int pixelformat;

    ppfd = &pfd;

    ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
    ppfd->nVersion = 1;

    ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;// | PFD_DOUBLEBUFFER;
    ppfd->dwLayerMask = PFD_MAIN_PLANE;
    ppfd->iPixelType = PFD_TYPE_RGBA;//PFD_TYPE_COLORINDEX;
    ppfd->cColorBits = 24;
    ppfd->cDepthBits = 32;
    ppfd->cAccumBits = 0;
    ppfd->cStencilBits = 0;

    pixelformat = ChoosePixelFormat(hdc, ppfd);

    if ( (pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0 )
    {
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK);

        return FALSE;
    }

    if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE)
    {
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK);
        return FALSE;
    }

    return TRUE;
}

/* OpenGL code */

GLvoid resize( GLsizei width, GLsizei height )
{
  
	/*
	GLfloat aspect;

    glViewport( 0, 0, width, height );

    aspect = (GLfloat) width / height;

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(45.0, aspect, 1.0, 7.0 );
//	gluLookAt(0.0,0.0,5.0,0.0,0.0,0.0,1.0,1.0,1.0);
    glMatrixMode( GL_MODELVIEW );
	*/
	  glViewport(0, 0, (GLsizei) width, (GLsizei) height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (width <= height) 
      glOrtho (-3.0, 3.0, 
         -3.0*(GLfloat)height/(GLfloat)width, 3.0*(GLfloat)height/(GLfloat)width,1.0,7.0);
   else 
      glOrtho(-3.0*(GLfloat)width/(GLfloat)height, 
         3.0*(GLfloat)width/(GLfloat)height, -3.0, 3.0,1.0,7.0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

}    

GLvoid createObjects()
{
 
	GLfloat light_position[]={0.0f, 0.2f, 4.0f, 0.0f};
	GLfloat light_ambient[]={1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_diffuse[]={0.8f, 0.8f, 0.8f, 0.5f};

	glLightfv (GL_LIGHT0, GL_POSITION, light_position);
	glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);

    glNewList(LIGHTON, GL_COMPILE);
		glEnable(GL_LIGHT0);
    glEndList();

	GLfloat x=PLATELENGTH;
	GLfloat y=0.040f;
	GLfloat z=1.0f;
	
    glNewList(SEAT, GL_COMPILE);
		glColor3f(0.5f, 0.5f, 0.5f);

		glBegin(GL_QUAD_STRIP);
		glVertex3f(0.0f,0.0f,z);
			glVertex3f(0.0f,0.0f,0.0f);

			glVertex3f(x,0.0f,z);
			glVertex3f(x,0.0f,0.0f);

			glVertex3f(x,y,z);
			glVertex3f(x,y,0.0);

			glVertex3f(0.0f,y,z);
			glVertex3f(0.0f,y,0.0f);

			glVertex3f(0.0f,0.0f,z);
			glVertex3f(0.0f,0.0f,0.0f);

 		glEnd();
		
		glBegin(GL_QUADS);
			glColor3f(0.3f, 0.3f, 0.3f);
			glNormal3f(0.0f,0.0f,1.0f);
			glVertex3f(0.0f,0.0f,z);
			glVertex3f(x,0.0f,z);
			glVertex3f(x,y,z);
			glVertex3f(0.0f,y,z);

			glNormal3f(0.0f,0.0f,-1.0f);
			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(0.0f,y,0.0f);
			glVertex3f(x,y,0.0f);
			glVertex3f(x,0.0f,0.0f);
			
		glEnd();

    glEndList();

//////////////////////////////////////////////////////////////////////////
	glNewList(AXIS, GL_COMPILE);
		glBegin(GL_LINES);
			glColor3f(0.0f,0.8f,1.0f);
//y轴
			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(0.0f,1.2f,0.0f);
//Y箭头			
			glVertex3f(0.0f,1.2f,0.0f);
			glVertex3f(0.04f,1.1f,0.0f);

			glVertex3f(0.0f,1.2f,0.0f);
			glVertex3f(-0.04f,1.1f,0.0f);
//X轴
			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(5.0f,0.0f,0.0f);
//X箭头	
			glVertex3f(5.0f,0.0f,0.0f);
			glVertex3f(4.8f,0.05f,0.0f);

			glVertex3f(5.0f,0.0f,0.0f);
			glVertex3f(4.8f,-0.05f,0.0f);

// Y字符：
			glColor3f(1.0f,1.0f,1.0f);
			glVertex3f(0.1f,1.3f,0.0f);
			glVertex3f(0.14f,1.24f,0.0f);

			glVertex3f(0.20f,1.32f,0.0f);
			glVertex3f(0.1f,1.16f,0.0f);

//X字符：	
			glVertex3f(5.10f,-0.10f,0.0f);
			glVertex3f(5.20f,0.10f,0.0f);

			glVertex3f(5.10f,0.08f,0.0f);
			glVertex3f(5.20f,-0.11f,0.0f);


		glEnd();
	glEndList();

//////////////////////////////////////////////////////////////////////	
	glNewList(PLATE, GL_COMPILE);

		glShadeModel(GL_SMOOTH);

		GLfloat fRadius;
		fRadius=(PLATELENGTH-0.5)/(3.0*PIE);
		double dStep=0.08;
		GLfloat fColor=0.0f;
		int nTotal=PIE/dStep;
		GLfloat fColorStep=0.5/(GLfloat)nTotal;////1

		glBegin(GL_TRIANGLE_FAN);
			glColor3f(0.5f,0.5f,0.5f);//////2
			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(fRadius,0.0f,0.0f);
	
			for(double dIndex=dStep; dIndex<2*PIE+dStep;dIndex += dStep)
			{
				if(dIndex<PIE)
				{
					fColor+=1.5*fColorStep;
				 
				}
					
				else
					fColor-=1.5*fColorStep;

				glColor3f(0.65+fColor,0.55+fColor,0.4+fColor);//////3
				glVertex3f(fRadius*cos(dIndex),fRadius*sin(dIndex),0.0f);
			}

		glEnd();

		///////////////////////////////////////////////////
/*		GLfloat lw[2];
		glGetFloatv(GL_LINE_WIDTH_RANGE,lw);
		glLineWidth(lw[1]/6.0);

		glBegin(GL_LINE_STRIP);
			glColor3f(0.0f,0.0f,0.0f);
			for(double dLineIndex=0.0; dLineIndex<2*PIE+dStep;dLineIndex += dStep)
				glVertex3f(fRadius*cos(dLineIndex),fRadius*sin(dLineIndex),0.0f);
		glEnd();
*/
	glEndList();
 
}

GLvoid initializeGL(GLsizei width, GLsizei height)
{
   	GLfloat	aspect;

	glClearColor(0.0f, 0.4f, 0.8f, 0.5f);
    glClearDepth(1.0);

    glEnable(GL_DEPTH_TEST);

    glMatrixMode( GL_PROJECTION );
    aspect = (GLfloat) width / height;
    gluPerspective( 45.0, aspect, 1.0, 7.0 );

    glMatrixMode( GL_MODELVIEW );
	
    createObjects();
}


GLvoid drawSpinLine(GLfloat fAngle,GLfloat fRadius)
{
	glBegin(GL_LINE_STRIP);

		GLfloat fStep=0.05f;
		
		glColor3f(0.2f,0.8f,0.5f);
		glVertex3f(0.0f,0.0f,0.01f);

		for(GLfloat fIndex=fStep;fIndex<fAngle;fIndex+=fStep)
		{
			GLfloat x=fIndex*fRadius-fRadius*sin(fIndex);
			GLfloat y=fRadius-fRadius*cos(fIndex);
			glVertex3f(x,y,0.01f);
		}

	glEnd();
}








GLvoid drawScene(GLvoid)
{
   
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glPushMatrix();
        glTranslated(0.0, -0.8, -5.0);
		glDisable(GL_LIGHTING);

		glPushMatrix();
			glTranslatef(-2.0f,-0.30f,0.0f);//-2.0
			
			glCallList(SEAT);
			
			glTranslatef(0.0f,0.05f,0.5f);
//		下面是基于以上坐标系的模型：
			
			glCallList(AXIS);
////////////////////////////////////////////////////////////////////////

			GLfloat fRadius;
			fRadius=(PLATELENGTH-0.5)/(3.0*PIE);
//			
			glPushMatrix();
				glTranslatef(0.0f,fRadius,0.0f);
				glCallList(PLATE);
			glPopMatrix();
//
			drawSpinLine(fEndAngle,fRadius);


		glPopMatrix();


 	glPopMatrix();

 
	SWAPBUFFERS;
	
}
 