
#include <windows.h>
#include <iostream.h>
#include <fstream.h>
#include <conio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <dos.h>
#define INITGUID
#include <objbase.h>
#include <dinput.h>
#include "mouse.h"
#include "useful.h"
#include "bmp_load.h"
#include "terrain.h"
#include "lod.h"

// Definitions.

#define TRUE 1
#define FALSE 0
#define WIDTH 4.0

#define ZOOMSIZE 4

// Function prototypes.
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
VOID EnableOpenGL( HWND hWnd, HDC * hDC, HGLRC * hRC );
VOID DisableOpenGL( HWND hWnd, HDC hDC, HGLRC hRC );

void triangle1(int , int , int , int , int , int );
void triangle2(int , int , int , int , int , int );
void (* triangle)(int , int , int , int , int , int );


HWND main_window_handle;
HINSTANCE main_instance = NULL; // save the instance

int winWidth = 400;  
int winHeight = 300;

int bilinear_filtering = 1;
int wireframe_mode = -1;
int flying_mode = 1;
int detail_texture_mode = -1;

#define texScale 64.0 // This stretches one texture over an 8x8 area.

GLuint texName[100];

GLfloat xpos, ypos, zpos; 
GLfloat lookx, looky, lookz; // The current direction.
GLint mapX = 128, mapZ = 128; // The area of the map where the map screen is viewing.
GLfloat speed = 0.0; // The current forward velocity.
GLfloat timer = 0.0, wTimer = 0.0;
GLint mapOffset = 4096;

GLfloat lightPosition[] = {-1.0, 0.0, 0.2, 0.0};

float sin_t[256];
float cos_t[256];

LPDIRECTINPUT        lpdi      = NULL;    // dinput object
LPDIRECTINPUTDEVICE  lpdimouse   = NULL;  // dinput mouse
LPDIRECTINPUTDEVICE  lpdikey   = NULL;  // dinput keyboard
DIMOUSESTATE mouse_state;
UCHAR keystate[256];

LAND_TEX lt;

TEXTURE *detail_texture;

unsigned short int fov = 16; // Field of view. e.g. (-16, -16) to (16, 16)

int fps = 0;
double c1, c2;

void exitTerrain(void) 
{
	delete[] y;
	delete[] colour_map;
	destroy_bmp(detail_texture);
}

__inline float height_w(float x, float z) 
{
	return (sin(x+timer)+sin(z+timer))*16 + 32;
}

void check_height(void) 
{
	
	float x0, x1, lx, lz, x, z, midpoint;
	int fx, fz;
	
	x = xpos/(COMP*1.0);
	z = zpos/(COMP*1.0);
	fx = (int)(x);
	fz = (int)(z);
	lx = x - fx;
	lz = z - fz;
	
	x0 = y[IX(fx,fz)] + (y[IX(fx,fz+1)] - y[IX(fx,fz)])*lz;
	x1 = y[IX(fx+1,fz)] + (y[IX(fx+1,fz+1)] - y[IX(fx+1,fz)])*lz;
	midpoint = x0 + (x1 - x0)*lx;

	if(flying_mode	> 0) 
	{
		ypos = (midpoint * 8) + 1024.0f;
	}
	else 
	{
		ypos = (midpoint * 8) + 16.0f;
	}

	if(x<0)
		xpos = 0;
	else if(x>(M_SIZE))
		xpos = ((M_SIZE) << SHIFT);
	if(z<0)
		zpos = 0;
	else if(z>(M_SIZE))
		zpos = ((M_SIZE) << SHIFT);
	
}

float get_height(float xp, float zp) 
{
	
	float x0, x1, lx, lz, x, z, midpoint;
	int fx, fz;
	
	x = xp/(COMP*1.0);
	z = zp/(COMP*1.0);
	fx = (int)(x);
	fz = (int)(z);
	lx = x - fx;
	lz = z - fz;
	
	x0 = y[IX(fx,fz)] + (y[IX(fx,fz+1)] - y[IX(fx,fz)])*lz;
	x1 = y[IX(fx+1,fz)] + (y[IX(fx+1,fz+1)] - y[IX(fx+1,fz)])*lz;
	midpoint = x0 + (x1 - x0)*lx;
	
	if(midpoint < 0.0f) 
	{
		return 0.0f;
	}
	else 
	{
		return midpoint;
	}
}


// Function that moves the eye / turns the angle of sight.
// Updates scene if update != 0. 
void move(int type, GLfloat amount, int update)
{
	GLfloat a;
	switch(type)
	{
	case FORWARD:
		xpos += lookx * amount;
		zpos += lookz * amount;
		break;
	case ROTATE:
		lookx = lookx*cos(amount) + lookz*sin(amount);
		lookz = lookz*cos(amount) - lookx*sin(amount);
		/* lookx^2 + lookz^2 != 1 for some reason. Fix. */
		a = 1/sqrt(lookx*lookx + lookz*lookz);
		lookx *= a;
		lookz *= a;
		break;
	case LOOKUP:
		looky += amount;
		break;
	}
	
	if (update)
	{
		check_height();
		glMatrixMode(GL_MODELVIEW); 
		glLoadIdentity();
		
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	}
}

void initTerrain(void) 
{
	y = new int[M_SIZE*M_SIZE];
	if(!y) 
	{
		cout << "ERROR - Unable to allocate memory for height map." << endl;
	}
	else 
	{
		cout << "Succesfully allocated memory for height map." << endl;
	}
	colour_map = new COLOUR[M_SIZE*M_SIZE];
	if(!colour_map) 
	{
		cout << "ERROR - Unable to allocate memory for shadow map." << endl;
	}
	else 
	{
		cout << "Succesfully allocated memory for shadow map." << endl;
	}
}

__inline int checkRange(int a) 
{
	if(a>255) 
	{
		a = 255;
	}
	else if(a<0) 
	{
		a = 0;
	}
	
	return a;
}

__inline int distance(int x1, int z1, int x2, int z2) 
{
	return sqrt((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1));
}

void colorTerrain(void) 
{
	int x, z;
	int h;
	
	for(x=0; x<M_SIZE; x++) 
	{
		for(z=0; z<M_SIZE; z++) 
		{
			h = 200 + (y[IX(x, z)] - y[IX(x+1, z)]) * 8;
			
			if(h>255) 		
				h = 255;			
			else if(h<0)			
				h = 0; 
									
			colour_map[IX(x, z)].r = h;
			colour_map[IX(x, z)].g = h;
			colour_map[IX(x, z)].b = h;
		}
	}
	
	lt.NEW_BITMAP(texName, y);
}

#ifndef f4
#define f4(a, b, c, d) ((a) + (b) + (c) + (d)) / 4
#endif

void randomTerrain(void) 
{
	int x, z;
	int i, j;
	int bsize, csize;
	int rnd;
	//  将所有的地形高程数据清零	
	for(i=0; i<(M_SIZE*M_SIZE); i++) 
	{
		y[i] = 0;
	}
	bsize = M_SIZE;
	csize = M_SIZE/2;
	rnd = 256;
	//  获取随机高程数据值	
	while(csize > 0) 
	{
		for(x=0; x<M_SIZE; x+=bsize) 
		{
			for(z=0; z<M_SIZE; z+=bsize) 
			{
				y[IX(x+csize, z+csize)] = f4(y[IX(x, z)], y[IX(x+bsize, z)], y[IX(x+bsize, z+bsize)], y[IX(x, z+bsize)]) + rand()%(rnd+1) - (rnd/2);
			}
		}
		for(x=0; x<M_SIZE; x+=bsize) 
		{
			for(z=0; z<M_SIZE; z+=bsize) 
			{
				y[IX(x+csize, z)] = f4(y[IX(x, z)],y[IX(x+bsize, z)],y[IX(x+csize, z+csize)],y[IX(x+csize, z-csize)]) + (rand()%(rnd+1) - (rnd/2));
			}
		}
		for(x=0; x<M_SIZE; x+=bsize) 
		{
			for(z=0; z<M_SIZE; z+=bsize) 
			{
				y[IX(x, z+csize)] = f4(y[IX(x, z)],y[IX(x, z+bsize)],y[IX(x-csize, z+csize)],y[IX(x+csize, z+csize)]) + (rand()%(rnd+1) - (rnd/2));
			}
		} 
		bsize /= 2;
		csize /= 2;
		if(csize < 16)
			rnd /= 2;
	}
	//  对高程数据进行平滑处理	
	for(i=0; i<M_SIZE*M_SIZE; i++) 
	{
		if(y[i] < 0) 
		{
			y[i] = 0;
		}
	} 
	for(j=0; j<8; j++) 
	{
		for(x=0; x<M_SIZE; x++) 
		{
			for(z=0; z<M_SIZE; z++) 
			{
				i =	y[IX(x-1, z-1)] + y[IX(x,   z-1)] +	y[IX(x+1, z-1)] + y[IX(x-1, z)] + 
					y[IX(x,   z)] +	y[IX(x+1, z)] +	y[IX(x-1, z+1)] + y[IX(x,   z+1)] + y[IX(x+1, z+1)];
				y[IX(x, z)] = i / 9;
			}
		}
	} 
	
	colorTerrain();
}

void update_world(void)
{
	timer += 0.05;
	wTimer += 0.0005;
	
	if(wTimer>1.0) 
	{
		wTimer = 0.0;
	}
	
	move(FORWARD, speed, 1);
}

#define GOTO_ERR goto err;

void init(void) 
{
	GLfloat fogColor[] = {0.12, 0.27, 0.7, 1.0};
	GLfloat lightDiffuse[] = {1.0, 0.75, 0.5, 1.0};
	
	glClearColor(0.12, 0.27, 0.7, 1.0);
	
	for(int i=0; i<256; i++) 
	{
		sin_t[i] = sin((i/128.0) * PI);
		cos_t[i] = cos((i/128.0) * PI);
	}
	
	initTerrain();
	
	xpos = (118) * COMP;
	ypos = 10.0;
	zpos = (125) * COMP;
	
	lookx = (120.0) * COMP;
	looky = (0.0) * COMP;
	lookz = (120.0) * COMP;
	
	speed = 0;
	
	update_world();
	
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 1);
	glFogf(GL_FOG_START, (M_SIZE/2)*0.1*COMP);
	glFogf(GL_FOG_END, (M_SIZE/2)*0.5*COMP);
	
	glGenTextures(100, texName);
	
	srand(1111111);

	randomTerrain();
	
	srand((unsigned)time(NULL));

	detail_texture = load_bitmap("DETAIL.BMP");

	glBindTexture(GL_TEXTURE_2D, texName[20]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5, detail_texture->w, detail_texture->h, 0, GL_RGB, GL_UNSIGNED_BYTE, detail_texture->data);
	
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightDiffuse);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHT0);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glShadeModel(GL_SMOOTH);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	
	glEnable(GL_TEXTURE_2D);
	
	glEnable(GL_DITHER);

	triangle = triangle1;

	move(ROTATE, 2.6, 0);
	move(LOOKUP, -0.2, 0);
	
	c1 = (unsigned)time(NULL);
	c2 = 0;
	fps = 0;
}

__inline void setColor(int x, int z) {
	glColor3ub(colour_map[IX(x, z)].r, colour_map[IX(x, z)].g, colour_map[IX(x, z)].b);
}

__inline void setVertex(int x, int z) {
	glVertex3f((x << SHIFT), y[IX(x, z)] * 8, (z << SHIFT));
}

__inline void setVertex0(int x, int z) {
	glVertex3f((x << SHIFT), (sin(x+timer)+sin(z+timer))*16, (z << SHIFT));
}

__inline void setVertex1(int x, int y, int z) {
	glVertex3f((x << SHIFT), y * 8, (z << SHIFT));
}

void triangle1(int x1, int z1, int x2, int z2, int x3, int z3) {
	glBegin(GL_TRIANGLES);
	glTexCoord2f(x1/(texScale), z1/(texScale));
	setColor(x1, z1);
	setVertex(x1, z1);
	
	glTexCoord2f(x2/(texScale), z2/(texScale));
	setColor(x2, z2);
	setVertex(x2, z2);
	
	glTexCoord2f(x3/(texScale), z3/(texScale));
	setColor(x3, z3);
	setVertex(x3, z3);
	glEnd();
}

void triangle2(int x1, int z1, int x2, int z2, int x3, int z3) {
	glBegin(GL_LINE_LOOP);
	setVertex(x1, z1);
	setVertex(x2, z2);
	setVertex(x3, z3);
	glEnd();
}

void draw_point(int x, int z, int width, int direction) {
	
	switch(direction) {
	case NORTH:
		triangle(x, z, x + width, z - width, x - width, z - width);
		return;
	case SOUTH:
		triangle(x, z, x - width, z + width, x + width, z + width);
		return;
	case EAST:
		triangle(x, z, x + width, z + width, x + width, z - width);
		return;
	case WEST:
		triangle(x, z, x - width, z - width, x - width, z + width);
		return; 
	case NORTH_L:
		triangle(x, z, x, z - width, x - width, z - width);
		return;
	case NORTH_R:
		triangle(x, z, x + width, z - width, x, z - width);
		return;
		
	case SOUTH_L:
		triangle(x, z, x - width, z + width, x, z + width);
		return;
	case SOUTH_R:
		triangle(x, z, x, z + width, x + width, z + width);
		return;
		
	case EAST_T:
		triangle(x, z, x + width, z, x + width, z - width);
		return;
	case EAST_B:
		triangle(x, z, x + width, z + width, x + width, z);
		return;
		
	case WEST_T:
		triangle(x, z, x - width, z - width, x - width, z);
		return;
	case WEST_B:
		triangle(x, z, x - width, z, x - width, z + width);
		return;
	default:
		break;
	};
}

void display(void) 
{
	
	int i;
	int x, z;
	int aX, aZ;
	int r;
	float v1[3];
	float location_f[3];
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glViewport(0, 0, (GLsizei) winWidth, (GLsizei) winHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(60.0, (GLfloat) winWidth/(GLfloat) winHeight, 1.0, ((M_SIZE + 4) * 2) << SHIFT);
	
	// 设置摄像机位置
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt (xpos, ypos, zpos, xpos+lookx, ypos+looky, zpos+lookz, 0.0, 1.0, 0.0);
	
	reset_quad_tree();
	setup_quadtree(128, 128, 128);
	
	if(wireframe_mode < 0) 
	{
		glEnable(GL_TEXTURE_2D);
	}
	else 
	{
		glColor3f(0.0, 0.0, 0.0);
	}
	
	draw(128, 128, 128, 0);
	
	if((wireframe_mode < 0) && (flying_mode < 0) && (detail_texture_mode > 0)) 
	{
		
		glBindTexture(GL_TEXTURE_2D, texName[20]);
		
		glColor4f(1.0, 1.0, 1.0, 1.0);
		
		glEnable(GL_BLEND);
		
		glDisable(GL_DEPTH_TEST);
		
		glBegin(GL_QUADS);
		
		location_f[0] = xpos / COMP;
		location_f[2] = zpos / COMP;
		
		aX = mapX - mapX%2;
		aZ = mapZ - mapZ%2;
		
		for(x=aX-16; x<aX+16; x+=2) 
		{
			for(z=aZ-16; z<aZ+16; z+=2) 
			{
				v1[0] = x;
				v1[2] = z;
				
				glColor4f(1.0, 1.0, 1.0, 0.5 - DISTANCE(v1, location_f)/20.0f);
				glTexCoord2f(0.0, 0.0);
				setVertex(x, z); 
				
				v1[0] = x;
				v1[2] = z+2;
				
				glColor4f(1.0, 1.0, 1.0, 0.5 - DISTANCE(v1, location_f)/20.0f);
				glTexCoord2f(0.0, 1.0);
				setVertex(x, z+2); 
				
				v1[0] = x+2;
				v1[2] = z+2;
				
				glColor4f(1.0, 1.0, 1.0, 0.5 - DISTANCE(v1, location_f)/20.0f);
				glTexCoord2f(1.0, 1.0);
				setVertex(x+2, z+2); 
				
				v1[0] = x+2;
				v1[2] = z;
				
				glColor4f(1.0, 1.0, 1.0, 0.5 - DISTANCE(v1, location_f)/20.0f);
				glTexCoord2f(1.0, 0.0);
				setVertex(x+2, z); 
			}
		}
		
		glEnd();
		
		glEnable(GL_DEPTH_TEST);
		
		glDisable(GL_BLEND); 
	}

	r = M_SIZE << SHIFT;
	
	glDisable(GL_TEXTURE_2D);
	
	glBegin(GL_QUADS);
	
	for(i=0; i<256; i+=16) 
	{
		glColor3f(0.12, 0.27, 0.7);
		glVertex3f(xpos + sin_t[(i + 16)&255] * r, (64 << SHIFT), zpos + cos_t[(i + 16)&255] * r);
		
		glColor3f(0.42, 0.57, 1.0);
		glVertex3f(xpos + sin_t[(i + 16)&255] * r, 0, zpos + cos_t[(i + 16)&255] * r);
		
		glColor3f(0.42, 0.57, 1.0);
		glVertex3f(xpos + sin_t[i] * r, 0, zpos + cos_t[i] * r);
		
		glColor3f(0.12, 0.27, 0.7);
		glVertex3f(xpos + sin_t[i] * r, (64 << SHIFT), zpos + cos_t[i] * r);
	}
	
	for(i=0; i<256; i+=16) 
	{
		glColor3f(0.42, 0.57, 1.0);
		glVertex3f(xpos + sin_t[(i + 16)&255] * r, 0, zpos + cos_t[(i + 16)&255] * r);
		
		glColor3f(0.12, 0.27, 0.7);
		glVertex3f(xpos + sin_t[(i + 16)&255] * r, -(16 << SHIFT), zpos + cos_t[(i + 16)&255] * r);
		
		glColor3f(0.12, 0.27, 0.7);
		glVertex3f(xpos + sin_t[i] * r, -(16 << SHIFT), zpos + cos_t[i] * r);
		
		glColor3f(0.42, 0.57, 1.0);
		glVertex3f(xpos + sin_t[i] * r, 0, zpos + cos_t[i] * r);
	}
	
	glEnd();

	glFlush();
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow )
{
	WNDCLASS wc;
	HWND hWnd;
	HDC hDC;
	HGLRC hRC;
	MSG msg;
	BOOL bQuit = FALSE;
	int i;
	
	// 注册窗口类
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "GLSample";
	RegisterClass( &wc );
	
	// 生成主窗口
	hWnd = CreateWindow( 
		"GLSample", "OpenGL Terrain Program", 
		WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION, 
		0, 0, winWidth, winHeight,
		NULL, NULL, hInstance, NULL );
	
	main_window_handle = hWnd;
	
	// first create the direct input object
	if (DirectInputCreate(hInstance,DIRECTINPUT_VERSION,&lpdi,NULL)!=DI_OK)
		GOTO_ERR;
	
	// create a keyboard device  //////////////////////////////////
	if (lpdi->CreateDevice(GUID_SysKeyboard, &lpdikey, NULL)!=DI_OK)
		GOTO_ERR;
	
	// set cooperation level
	if (lpdikey->SetCooperativeLevel(hWnd, 
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)!=DI_OK)
		GOTO_ERR;
	
	// set data format
	if (lpdikey->SetDataFormat(&c_dfDIKeyboard)!=DI_OK)
		GOTO_ERR;
	
	// create a mouse device  //////////////////////////////////
	if (lpdi->CreateDevice(GUID_SysMouse, &lpdimouse, NULL)!=DI_OK)
		GOTO_ERR;
	
	// set cooperation level
	if (lpdimouse->SetCooperativeLevel(hWnd, 
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)!=DI_OK)
		GOTO_ERR;
	
	// set data format
	if (lpdimouse->SetDataFormat(&c_dfDIMouse)!=DI_OK)
		GOTO_ERR;
	
	// enable OpenGL for the window
	EnableOpenGL( hWnd, &hDC, &hRC );
	
	init(); // Load textures, setup OpenGL etc.
	
	ShowCursor(FALSE);
	
	// acquire the keyboard
	if (lpdikey->Acquire()!=DI_OK)
		GOTO_ERR;
	
	// acquire the mouse
	if (lpdimouse->Acquire()!=DI_OK)
		GOTO_ERR;
	
	// program main loop
	while ( !bQuit ) {
		
		// check for messages
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
			
			// handle or dispatch messages
			if ( msg.message == WM_QUIT ) {
				bQuit = TRUE;
			} else {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			
		} else {
			
			// OpenGL animation code goes here	
			
			// This following code does some boundary checking.
			
			mapX = xpos / COMP;
			mapZ = zpos / COMP;
			
			location[0] = mapX;
			location[2] = mapZ;
			
			// get the keyboard info
			lpdikey->GetDeviceState(256, keystate);
			
			if(keystate[DIK_ESCAPE]) 
			{
				PostQuitMessage( 0 ); // 退出程序
			}
			
			if(keystate[DIK_W]) 
			{
				if(flying_mode > 0) 
				{
					if(speed < 128) 
					{
						speed += 8;
					}
				}
				else 
				{
					if(speed < 8) 
					{
						speed += 2;
					}
				}
			}
			else 
			{
				if(flying_mode > 0) 
				{
					if(speed > 0) 
					{
						speed -= 4;
					}
				}
				else 
				{
					if(speed > 0) 
					{
						speed -= 1;
					}
				}
			}
			
			if(keystate[DIK_S]) 
			{
				speed = 0;
			}
			
			if(keystate[DIK_1]) 
			{
				lod_level = 8;
			}
			if(keystate[DIK_2]) 
			{
				lod_level = 16;
			}
			if(keystate[DIK_3]) 
			{
				lod_level = 32;
			}
			if(keystate[DIK_4]) 
			{
				lod_level = 64;
			}
			
			if(keystate[DIK_T]) {
				detail_texture_mode = -detail_texture_mode;
				Sleep(100);
			}

			if(keystate[DIK_F]) {
				flying_mode = -flying_mode;
				speed = 0;
				Sleep(100);
			}
			
			if(keystate[DIK_B]) {
				for(i=0; i<100; i++) {
					glBindTexture(GL_TEXTURE_2D, texName[i]);
					if(bilinear_filtering > 0) {
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					}
					else {
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					}
				}
				
				bilinear_filtering = -bilinear_filtering;
				
				Sleep(100);
			}

			if(keystate[DIK_M]) {
				if(wireframe_mode > 0) {
					triangle = triangle1;
				}
				else {
					triangle = triangle2;
				}
				
				wireframe_mode = -wireframe_mode;
				
				Sleep(100);
			}
			
			if(keystate[DIK_SPACE]) 
			{
				randomTerrain();
			}
			
			// 获得鼠标信息
			lpdimouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state);
			
			move(ROTATE, (GLfloat)((-mouse_state.lX)/100.0), 0);
			move(LOOKUP, (GLfloat)((mouse_state.lY)/100.0), 0);
			
			update_world(); // 更新地形数据
			
			display(); // 绘制地形
			
			SwapBuffers( hDC ); 
		}		
	}
	
	ShowCursor(TRUE);
	
	lpdimouse->Unacquire();
	lpdimouse->Release();
	
	lpdikey->Unacquire();
	lpdikey->Release();
	
	lpdi->Release();
	
	goto end;
	
err:
	MessageBox(main_window_handle, "Unable to initialise Direct Input\n NOTE: DirectX is unavailable on NT.", "Error", MB_OK | MB_ICONERROR);
	
end:
	
	// shutdown OpenGL
	DisableOpenGL( hWnd, hDC, hRC );
	
	// destroy the window explicitly
	DestroyWindow( hWnd );
	
	return msg.wParam;	
}

// Window Procedure

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) {

	switch ( message ) {
		
	case WM_CREATE:
		return 0;
		
	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;
		
	case WM_DESTROY:
		return 0;
		
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );	
	}
}

// Enable OpenGL

VOID EnableOpenGL( HWND hWnd, HDC * hDC, HGLRC * hRC )
{
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;
	
	// get the device context (DC)
	*hDC = GetDC( hWnd );
	
	// set the pixel format for the DC
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | 
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	iFormat = ChoosePixelFormat( *hDC, &pfd );
	SetPixelFormat( *hDC, iFormat, &pfd );
	
	// create and enable the render context (RC)
	*hRC = wglCreateContext( *hDC );
	wglMakeCurrent( *hDC, *hRC );
}

// Disable OpenGL

VOID DisableOpenGL( HWND hWnd, HDC hDC, HGLRC hRC )
{
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( hRC );
	ReleaseDC( hWnd, hDC );
}