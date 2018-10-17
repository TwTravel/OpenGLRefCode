

#include <windows.h>
#include <stdio.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

#define	MAX_PARTICLES	1000		// ���������������

HDC			hDC=NULL;
HGLRC		hRC=NULL;
HWND		hWnd=NULL;
HINSTANCE	hInstance;

bool	keys[256];
bool	active=TRUE;
bool	fullscreen=TRUE;			// ȫ����־
bool	rainbow=true;
bool	sp;							// �Ƿ��¿ո��
bool	rp;							// �Ƿ��»س���

float	slowdown=2.0f;				// Slow Down Particles
float	xspeed;						// Base X Speed (To Allow Keyboard Direction Of Tail)
float	yspeed;						// Base Y Speed (To Allow Keyboard Direction Of Tail)
float	zoom=-40.0f;				// Used To Zoom Out

GLuint	loop;
GLuint	col;						// ��ǰ����ɫ
GLuint	delay;						// �ӳ�ʱ��
GLuint	texture[1];					// ���ӵ�����

typedef struct						// �������ӵĽṹ��
{
	bool	active;					// �����Ƿ��ǻ��
	float	life;					// ���ӵ�����
	float	fade;					// �����ٶ�
	float	r;						// ��ɫRed��ֵ
	float	g;						// ��ɫGreen��ֵ
	float	b;						// ��ɫBlue��ֵ
	float	x;						// ���ӵ���ά����X��ֵ
	float	y;						// ���ӵ���ά����Y��ֵ
	float	z;						// ���ӵ���ά����Z��ֵ
	float	xi;						// ������X������ٶ�
	float	yi;						// ������Y������ٶ�
	float	zi;						// ������Z������ٶ�
	float	xg;						// ������X����ļ��ٶ�
	float	yg;						// ������Y����ļ��ٶ�
	float	zg;						// ������Z����ļ��ٶ�
}
particles;

particles particle[MAX_PARTICLES];	// ��������

static GLfloat colors[12][3]=		// ���ӵ���ɫֵ
{
	{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
	{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
	{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
};

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

AUX_RGBImageRec *LoadBMP(char *Filename)				// װ��һ��λͼ�ļ�
{
        FILE *File=NULL;
        if (!Filename)	
        {
                return NULL;	
        }
        File=fopen(Filename,"r");	
        if (File)	
        {
			fclose(File);								// �ر��ļ�
			return auxDIBImageLoad(Filename);			// װ��λͼ�ļ�
        }
        return NULL;
}

int LoadGLTextures()									// װ��λͼ�ļ���Ϊ����
{
        int Status=FALSE;	
        AUX_RGBImageRec *TextureImage[1];
        memset(TextureImage,0,sizeof(void *)*1);

        if (TextureImage[0]=LoadBMP("Data/Particle.bmp"))	// װ����������
        {
			Status=TRUE;
			glGenTextures(1, &texture[0]);				// ��������

			glBindTexture(GL_TEXTURE_2D, texture[0]);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
        }

        if (TextureImage[0])							// ����������
		{
			if (TextureImage[0]->data)					// �������ͼ�����ݴ���
			{
				free(TextureImage[0]->data);			// �ͷ�����ͼ������
			}
			free(TextureImage[0]);						// �ͷ�����
		}
        return Status;	
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)	
{
	if (height==0)	
	{
		height=1;	
	}

	glViewport(0,0,width,height);	

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	


	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,200.0f);

	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();	
}

int InitGL(GLvoid)										// ������ʼ������
{
	if (!LoadGLTextures())								// װ������
	{
		return FALSE;									// ����������ڣ�����false
	}

	glShadeModel(GL_SMOOTH);							// ʹ�ù⻬��Ӱ
	glClearColor(0.0f,0.0f,0.0f,0.0f);					// ���ú�ɫ����
	glClearDepth(1.0f);									// ������Ȼ���ֵ
	glDisable(GL_DEPTH_TEST);							// ��ʹ����Ȳ���
	glEnable(GL_BLEND);									// ʹ���ں�
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);					// �����ںϲ���
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);	// �����ں�Ʒ��
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);				// ���÷�����Ʒ��
	glEnable(GL_TEXTURE_2D);							// ʹ������ӳ��
	glBindTexture(GL_TEXTURE_2D,texture[0]);			// ��������

	for (loop=0;loop<MAX_PARTICLES;loop++)				// ��ʼ�����е�����
	{
		particle[loop].active=true;								// ���е������ǻ��
		particle[loop].life=1.0f;								// ���ӵ�����Ϊ1.0
		particle[loop].fade=float(rand()%100)/1000.0f+0.003f;	// ����ĵ����ٶ�
		particle[loop].r=colors[loop*(12/MAX_PARTICLES)][0];	// �������ӵ���ɫֵRed
		particle[loop].g=colors[loop*(12/MAX_PARTICLES)][1];	// �������ӵ���ɫֵGreen
		particle[loop].b=colors[loop*(12/MAX_PARTICLES)][2];	// �������ӵ���ɫֵBlue
		particle[loop].xi=float((rand()%50)-26.0f)*10.0f;		// ��X���������ٶ�
		particle[loop].yi=float((rand()%50)-25.0f)*10.0f;		// ��Y���������ٶ�
		particle[loop].zi=float((rand()%50)-25.0f)*10.0f;		// ��Z���������ٶ�
		particle[loop].xg=0.0f;									// ����ˮƽ���ٶ�
		particle[loop].yg=-0.8f;								// ���ô�ֱ���ٶ�
		particle[loop].zg=0.0f;									// ����Z����ļ��ٶ�
	}

	return TRUE;
}

int DrawGLScene(GLvoid)										// �������ƺ���
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// �����ɫ������
	glLoadIdentity();

	for (loop=0;loop<MAX_PARTICLES;loop++)					// �������е�����
	{
		if (particle[loop].active)							// ��������ǻ�ģ������
		{
			float x=particle[loop].x;						// ��ȡ���ӵ�X����
			float y=particle[loop].y;						// ��ȡ���ӵ�Y����
			float z=particle[loop].z+zoom;					// ��ȡ���ӵ�Z���꣬�볡���������й�
			glColor4f(particle[loop].r,particle[loop].g,particle[loop].b,particle[loop].life);

			glBegin(GL_TRIANGLE_STRIP);						// ����������Ƭ����ʽģ�����
			    glTexCoord2d(1,1); glVertex3f(x+0.5f,y+0.5f,z); 
				glTexCoord2d(0,1); glVertex3f(x-0.5f,y+0.5f,z); 
				glTexCoord2d(1,0); glVertex3f(x+0.5f,y-0.5f,z); 
				glTexCoord2d(0,0); glVertex3f(x-0.5f,y-0.5f,z); 
			glEnd();
			//  �������ӵ��ٶȼ��������µ�λ��
			particle[loop].x+=particle[loop].xi/(slowdown*1000);
			particle[loop].y+=particle[loop].yi/(slowdown*1000);
			particle[loop].z+=particle[loop].zi/(slowdown*1000);
			//  �������ӵļ��ٶȼ��������µ��ٶ�
			particle[loop].xi+=particle[loop].xg;
			particle[loop].yi+=particle[loop].yg;
			particle[loop].zi+=particle[loop].zg;
			//  ���ݵ��������������ӵ�����
			particle[loop].life-=particle[loop].fade;

			if (particle[loop].life<0.0f)					// ������������ս�
			{
				particle[loop].life=1.0f;					// ���¸����µ���������
				particle[loop].fade=float(rand()%100)/1000.0f+0.003f;	// �������ֵ
				//  �����µ�λ������
				particle[loop].x=0.0f;
				particle[loop].y=0.0f;	
				particle[loop].z=0.0f;
				//  ���ӵ��������������ٶ�
				particle[loop].xi=xspeed+float((rand()%60)-32.0f);
				particle[loop].yi=yspeed+float((rand()%60)-30.0f);
				particle[loop].zi=float((rand()%60)-30.0f);
				//  ���ӵ���ɫֵ
				particle[loop].r=colors[col][0];
				particle[loop].g=colors[col][1];
				particle[loop].b=colors[col][2];
			}

			// ����������ּ���'8'��������Y����ļ���С��1.5f����Y����ļ��ٶȵ���
			if (keys[VK_NUMPAD8] && (particle[loop].yg<1.5f)) particle[loop].yg+=0.01f;

			// ����������ּ���'2'��������Y����ļ��ٴ���-1.5f����Y����ļ��ٶȵݼ�
			if (keys[VK_NUMPAD2] && (particle[loop].yg>-1.5f)) particle[loop].yg-=0.01f;

			// ����������ּ���'6'��������X����ļ���С��1.5f����X����ļ��ٶȵ���
			if (keys[VK_NUMPAD6] && (particle[loop].xg<1.5f)) particle[loop].xg+=0.01f;

			// ����������ּ���'4'��������X����ļ��ٴ���1.5f����X����ļ��ٶȵݼ�
			if (keys[VK_NUMPAD4] && (particle[loop].xg>-1.5f)) particle[loop].xg-=0.01f;

			if (keys[VK_TAB])										// �������Tab��
			{
				//  ����λ������
				particle[loop].x=0.0f;	
				particle[loop].y=0.0f;	
				particle[loop].z=0.0f;	
				//  ���ӵ��ٶ����
				particle[loop].xi=float((rand()%50)-26.0f)*10.0f;
				particle[loop].yi=float((rand()%50)-25.0f)*10.0f;
				particle[loop].zi=float((rand()%50)-25.0f)*10.0f;
			}
		}
    }
	return TRUE;
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}


BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else									// Otherwise
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_SIZE:								// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop

	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;							// Windowed Mode
	}

	// Create Our OpenGL Window
	if (!CreateGLWindow("����ʹ����ʾ",400,300,16,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	if (fullscreen)									// Are We In Fullscreen Mode
	{
		slowdown=1.0f;								// If So, Speed Up The Particles (3dfx Issue)
	}

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
			{
				done=TRUE;							// ESC or DrawGLScene Signalled A Quit
			}
			else									// Not Time To Quit, Update Screen
			{
				SwapBuffers(hDC);					// Swap Buffers (Double Buffering)

				if (keys[VK_ADD] && (slowdown>1.0f)) slowdown-=0.01f;		// �����ٶ�����
				if (keys[VK_SUBTRACT] && (slowdown<4.0f)) slowdown+=0.01f;	// �����ٶȼ�С

				if (keys[VK_PRIOR])	zoom+=0.1f;		// �Ŵ�
				if (keys[VK_NEXT])	zoom-=0.1f;		// ��С

				if (keys[VK_RETURN] && !rp)			// �Ƿ���Return��
				{
					rp=true;
					rainbow=!rainbow;				// �ʺ�ģʽ����
				}
				if (!keys[VK_RETURN]) rp=false;		// ����ͷ�Return���������־
				
				if ((keys[' '] && !sp) || (rainbow && (delay>25)))	// �Ƿ��¿ո��
				{
					if (keys[' '])	rainbow=false;
					sp=true;
					delay=0;
					col++;
					if (col>11)	col=0;
				}
				if (!keys[' '])	sp=false;			// ����ͷſո���������־

				// �������UP��������Y������ٶ�С��200����Y������ٶȵ���
				// If Up Arrow And Y Speed Is Less Than 200 Increase Upward Speed
				if (keys[VK_UP] && (yspeed<200)) yspeed+=1.0f;

				// �������DOWN��������Y������ٶȴ���200����Y������ٶȵݼ�
				if (keys[VK_DOWN] && (yspeed>-200)) yspeed-=1.0f;

				// �������RIGHT��������X������ٶ�С��200����X������ٶȵ���
				if (keys[VK_RIGHT] && (xspeed<200)) xspeed+=1.0f;

				// �������LEFT��������X������ٶȴ���200����X������ٶȵݼ�
				if (keys[VK_LEFT] && (xspeed>-200)) xspeed-=1.0f;

				delay++;

				if (keys[VK_F1])						// �Ƿ���F1��
				{
					keys[VK_F1]=FALSE;	
					KillGLWindow();
					fullscreen=!fullscreen;				// ȫ��/����ģʽ�л�
					if (!CreateGLWindow("���ӵ�ʹ����ʾ",400,300,16,fullscreen))
					{
						return 0;	
					}
				}
			}
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}
