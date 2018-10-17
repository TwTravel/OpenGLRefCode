

#include <windows.h>
#include <stdio.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

#define	MAX_PARTICLES	1000		// 创建的最大粒子数

HDC			hDC=NULL;
HGLRC		hRC=NULL;
HWND		hWnd=NULL;
HINSTANCE	hInstance;

bool	keys[256];
bool	active=TRUE;
bool	fullscreen=TRUE;			// 全屏标志
bool	rainbow=true;
bool	sp;							// 是否按下空格键
bool	rp;							// 是否按下回车键

float	slowdown=2.0f;				// Slow Down Particles
float	xspeed;						// Base X Speed (To Allow Keyboard Direction Of Tail)
float	yspeed;						// Base Y Speed (To Allow Keyboard Direction Of Tail)
float	zoom=-40.0f;				// Used To Zoom Out

GLuint	loop;
GLuint	col;						// 当前的颜色
GLuint	delay;						// 延迟时间
GLuint	texture[1];					// 粒子的纹理

typedef struct						// 定义粒子的结构体
{
	bool	active;					// 粒子是否是活动的
	float	life;					// 粒子的寿命
	float	fade;					// 淡化速度
	float	r;						// 颜色Red的值
	float	g;						// 颜色Green的值
	float	b;						// 颜色Blue的值
	float	x;						// 粒子的三维坐标X的值
	float	y;						// 粒子的三维坐标Y的值
	float	z;						// 粒子的三维坐标Z的值
	float	xi;						// 粒子在X方向的速度
	float	yi;						// 粒子在Y方向的速度
	float	zi;						// 粒子在Z方向的速度
	float	xg;						// 粒子在X方向的加速度
	float	yg;						// 粒子在Y方向的加速度
	float	zg;						// 粒子在Z方向的加速度
}
particles;

particles particle[MAX_PARTICLES];	// 粒子数组

static GLfloat colors[12][3]=		// 粒子的颜色值
{
	{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
	{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
	{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
};

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

AUX_RGBImageRec *LoadBMP(char *Filename)				// 装入一个位图文件
{
        FILE *File=NULL;
        if (!Filename)	
        {
                return NULL;	
        }
        File=fopen(Filename,"r");	
        if (File)	
        {
			fclose(File);								// 关闭文件
			return auxDIBImageLoad(Filename);			// 装入位图文件
        }
        return NULL;
}

int LoadGLTextures()									// 装入位图文件作为纹理
{
        int Status=FALSE;	
        AUX_RGBImageRec *TextureImage[1];
        memset(TextureImage,0,sizeof(void *)*1);

        if (TextureImage[0]=LoadBMP("Data/Particle.bmp"))	// 装入粒子纹理
        {
			Status=TRUE;
			glGenTextures(1, &texture[0]);				// 生成纹理

			glBindTexture(GL_TEXTURE_2D, texture[0]);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
        }

        if (TextureImage[0])							// 如果纹理存在
		{
			if (TextureImage[0]->data)					// 如果纹理图像数据存在
			{
				free(TextureImage[0]->data);			// 释放纹理图像数据
			}
			free(TextureImage[0]);						// 释放纹理
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

int InitGL(GLvoid)										// 场景初始化函数
{
	if (!LoadGLTextures())								// 装入纹理
	{
		return FALSE;									// 如果纹理不存在，返回false
	}

	glShadeModel(GL_SMOOTH);							// 使用光滑阴影
	glClearColor(0.0f,0.0f,0.0f,0.0f);					// 设置黑色背景
	glClearDepth(1.0f);									// 设置深度缓冲值
	glDisable(GL_DEPTH_TEST);							// 不使用深度测试
	glEnable(GL_BLEND);									// 使用融合
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);					// 设置融合参数
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);	// 设置融合品质
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);				// 设置反走样品质
	glEnable(GL_TEXTURE_2D);							// 使用纹理映射
	glBindTexture(GL_TEXTURE_2D,texture[0]);			// 捆绑纹理

	for (loop=0;loop<MAX_PARTICLES;loop++)				// 初始化所有的粒子
	{
		particle[loop].active=true;								// 所有的粒子是活动的
		particle[loop].life=1.0f;								// 粒子的寿命为1.0
		particle[loop].fade=float(rand()%100)/1000.0f+0.003f;	// 随机的淡化速度
		particle[loop].r=colors[loop*(12/MAX_PARTICLES)][0];	// 设置粒子的颜色值Red
		particle[loop].g=colors[loop*(12/MAX_PARTICLES)][1];	// 设置粒子的颜色值Green
		particle[loop].b=colors[loop*(12/MAX_PARTICLES)][2];	// 设置粒子的颜色值Blue
		particle[loop].xi=float((rand()%50)-26.0f)*10.0f;		// 在X方向的随机速度
		particle[loop].yi=float((rand()%50)-25.0f)*10.0f;		// 在Y方向的随机速度
		particle[loop].zi=float((rand()%50)-25.0f)*10.0f;		// 在Z方向的随机速度
		particle[loop].xg=0.0f;									// 设置水平加速度
		particle[loop].yg=-0.8f;								// 设置垂直加速度
		particle[loop].zg=0.0f;									// 设置Z方向的加速度
	}

	return TRUE;
}

int DrawGLScene(GLvoid)										// 场景绘制函数
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// 清除颜色缓冲区
	glLoadIdentity();

	for (loop=0;loop<MAX_PARTICLES;loop++)					// 遍历所有的粒子
	{
		if (particle[loop].active)							// 如果粒子是活动的，则绘制
		{
			float x=particle[loop].x;						// 获取粒子的X坐标
			float y=particle[loop].y;						// 获取粒子的Y坐标
			float z=particle[loop].z+zoom;					// 获取粒子的Z坐标，与场景的缩放有关
			glColor4f(particle[loop].r,particle[loop].g,particle[loop].b,particle[loop].life);

			glBegin(GL_TRIANGLE_STRIP);						// 以三角形面片的形式模拟绘制
			    glTexCoord2d(1,1); glVertex3f(x+0.5f,y+0.5f,z); 
				glTexCoord2d(0,1); glVertex3f(x-0.5f,y+0.5f,z); 
				glTexCoord2d(1,0); glVertex3f(x+0.5f,y-0.5f,z); 
				glTexCoord2d(0,0); glVertex3f(x-0.5f,y-0.5f,z); 
			glEnd();
			//  根据粒子的速度计算粒子新的位置
			particle[loop].x+=particle[loop].xi/(slowdown*1000);
			particle[loop].y+=particle[loop].yi/(slowdown*1000);
			particle[loop].z+=particle[loop].zi/(slowdown*1000);
			//  根据粒子的加速度计算粒子新的速度
			particle[loop].xi+=particle[loop].xg;
			particle[loop].yi+=particle[loop].yg;
			particle[loop].zi+=particle[loop].zg;
			//  根据淡化参数计算粒子的寿命
			particle[loop].life-=particle[loop].fade;

			if (particle[loop].life<0.0f)					// 如果粒子寿命终结
			{
				particle[loop].life=1.0f;					// 重新赋予新的生命长度
				particle[loop].fade=float(rand()%100)/1000.0f+0.003f;	// 随机淡化值
				//  粒子新的位置坐标
				particle[loop].x=0.0f;
				particle[loop].y=0.0f;	
				particle[loop].z=0.0f;
				//  粒子的三个方向的随机速度
				particle[loop].xi=xspeed+float((rand()%60)-32.0f);
				particle[loop].yi=yspeed+float((rand()%60)-30.0f);
				particle[loop].zi=float((rand()%60)-30.0f);
				//  粒子的颜色值
				particle[loop].r=colors[col][0];
				particle[loop].g=colors[col][1];
				particle[loop].b=colors[col][2];
			}

			// 如果按下数字键盘'8'键，并且Y方向的加速小于1.5f，则Y方向的加速度递增
			if (keys[VK_NUMPAD8] && (particle[loop].yg<1.5f)) particle[loop].yg+=0.01f;

			// 如果按下数字键盘'2'键，并且Y方向的加速大于-1.5f，则Y方向的加速度递减
			if (keys[VK_NUMPAD2] && (particle[loop].yg>-1.5f)) particle[loop].yg-=0.01f;

			// 如果按下数字键盘'6'键，并且X方向的加速小于1.5f，则X方向的加速度递增
			if (keys[VK_NUMPAD6] && (particle[loop].xg<1.5f)) particle[loop].xg+=0.01f;

			// 如果按下数字键盘'4'键，并且X方向的加速大于1.5f，则X方向的加速度递减
			if (keys[VK_NUMPAD4] && (particle[loop].xg>-1.5f)) particle[loop].xg-=0.01f;

			if (keys[VK_TAB])										// 如果按下Tab键
			{
				//  粒子位于中心
				particle[loop].x=0.0f;	
				particle[loop].y=0.0f;	
				particle[loop].z=0.0f;	
				//  粒子的速度随机
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
	if (!CreateGLWindow("粒子使用演示",400,300,16,fullscreen))
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

				if (keys[VK_ADD] && (slowdown>1.0f)) slowdown-=0.01f;		// 粒子速度增加
				if (keys[VK_SUBTRACT] && (slowdown<4.0f)) slowdown+=0.01f;	// 粒子速度减小

				if (keys[VK_PRIOR])	zoom+=0.1f;		// 放大
				if (keys[VK_NEXT])	zoom-=0.1f;		// 缩小

				if (keys[VK_RETURN] && !rp)			// 是否按下Return键
				{
					rp=true;
					rainbow=!rainbow;				// 彩虹模式开关
				}
				if (!keys[VK_RETURN]) rp=false;		// 如果释放Return键，清除标志
				
				if ((keys[' '] && !sp) || (rainbow && (delay>25)))	// 是否按下空格键
				{
					if (keys[' '])	rainbow=false;
					sp=true;
					delay=0;
					col++;
					if (col>11)	col=0;
				}
				if (!keys[' '])	sp=false;			// 如果释放空格键，清除标志

				// 如果按下UP键，并且Y方向的速度小于200，则Y方向的速度递增
				// If Up Arrow And Y Speed Is Less Than 200 Increase Upward Speed
				if (keys[VK_UP] && (yspeed<200)) yspeed+=1.0f;

				// 如果按下DOWN键，并且Y方向的速度大于200，则Y方向的速度递减
				if (keys[VK_DOWN] && (yspeed>-200)) yspeed-=1.0f;

				// 如果按下RIGHT键，并且X方向的速度小于200，则X方向的速度递增
				if (keys[VK_RIGHT] && (xspeed<200)) xspeed+=1.0f;

				// 如果按下LEFT键，并且X方向的速度大于200，则X方向的速度递减
				if (keys[VK_LEFT] && (xspeed>-200)) xspeed-=1.0f;

				delay++;

				if (keys[VK_F1])						// 是否按下F1键
				{
					keys[VK_F1]=FALSE;	
					KillGLWindow();
					fullscreen=!fullscreen;				// 全屏/窗口模式切换
					if (!CreateGLWindow("粒子的使用演示",400,300,16,fullscreen))
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
