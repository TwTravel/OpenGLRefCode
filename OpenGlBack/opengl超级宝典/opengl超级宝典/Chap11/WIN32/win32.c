/*
 * Example program demonstrating the use of glDepthFunc using WIN32 for Chapter 11.
 *
 * Written by Michael Sweet.
 *
 * Press the 'd' key to toggle between GL_LESS and GL_GREATER depth
 * tests.  Press the 'ESC' key to quit.
 */

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>


/*
 * Globals...
 */

int      Width;               /* Width of window */
int      Height;              /* Height of window */
GLenum   DepthFunc = GL_LESS; /* Depth comparison function */
HWND     Window;              /* Scene window */
HPALETTE Palette;             /* Color palette (if necessary) */
HDC      DC;                  /* Drawing context */
HGLRC    RC;                  /* OpenGL rendering context */
int      Format;              /* Pixel format number */


/*
 * Functions...
 */

void             SetupContext(void);
LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void             Keyboard(unsigned char key, int x, int y);
void             Redraw(void);
void             Resize(int width, int height);


/*
 * 'WinMain()' - Open a window and display a sphere and cube.
 */

int APIENTRY                     /* O - Exit status */
WinMain(HINSTANCE hInst,         /* I - Current process instance */
        HINSTANCE hPrevInstance, /* I - Parent process instance */
        LPSTR     lpCmdLine,     /* I - Command-line arguments */
        int       nCmdShow)      /* I - Show window at startup? */
    {
    MSG      msg;                /* Window UI event */
    WNDCLASS wc;                 /* Window class */


    /* Register main window... */
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc   = (WNDPROC)WinProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "Depth Test Example Using WIN32";
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    if (!RegisterClass(&wc))
        return (FALSE);

    /* Then create it... */
    Window = CreateWindow("Depth Test Example Using WIN32",
                          "Depth Test Example Using WIN32",
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
  	 		  32, 32, 792, 573,
			  NULL, NULL, hInst, NULL);

    if (!Window)
        return (FALSE);

    ShowWindow(Window, nCmdShow);
    UpdateWindow(Window);

    /* Loop on events until the user quits this application... */
    for (;;)
        while (GetMessage(&msg, NULL, 0, 0))
            {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }

    return (TRUE);
    }


/*
 * 'SetupContext()' - Make an OpenGL rendering context and color palette as
 *                    necessary.
 */

void
SetupContext(void)
    {
    PIXELFORMATDESCRIPTOR pfd;     /* Pixel format information */
    LOGPALETTE            *pPal;   /* Pointer to logical palette */
    int                   nColors; /* Number of entries in palette */
    int                   ind,     /* Color index */
                          rmax,    /* Maximum red value */
                          gmax,    /* Maximum green value */
                          bmax;    /* Maximum blue value */


    /* Choose a pixel format... */
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize        = sizeof(pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    pfd.iPixelType   = PFD_TYPE_RGBA;	/* RGB color buffer */
    pfd.cDepthBits   = 16;		/* Need a depth buffer */

    Format = ChoosePixelFormat(DC, &pfd);
    if (!Format)
      exit(1);

    DescribePixelFormat(DC, Format, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    SetPixelFormat(DC, Format, &pfd);

    /* Create the OpenGL rendering context... */
    RC = wglCreateContext(DC);
    wglMakeCurrent(DC, RC);

    /* Find out if we need to define a color palette... */
    if (!(pfd.dwFlags & PFD_NEED_PALETTE))
        {
        Palette = NULL;
        return;
        }

    /* Allocate memory for a color palette... */
    nColors = 1 << pfd.cColorBits;	

    pPal = (LOGPALETTE *)malloc(sizeof(LOGPALETTE) +
                                nColors * sizeof(PALETTEENTRY));

    pPal->palVersion    = 0x300;
    pPal->palNumEntries = nColors;

    /*
     * Get the maximum values for red, green, and blue.  Then build 'nColors'
     * colors...
     */

    rmax = (1 << pfd.cRedBits) - 1;
    gmax = (1 << pfd.cGreenBits) - 1;
    bmax = (1 << pfd.cBlueBits) - 1;

    for (ind = 0; ind < nColors; ind ++)
        {
        pPal->palPalEntry[ind].peRed   = 255 * ((ind >> pfd.cRedShift) & rmax) / rmax;
        pPal->palPalEntry[ind].peGreen = 255 * ((ind >> pfd.cGreenShift) & gmax) / gmax;
        pPal->palPalEntry[ind].peBlue  = 255 * ((ind >> pfd.cBlueShift) & bmax) / bmax;
        pPal->palPalEntry[ind].peFlags = 0;
        }

    /* Create, select, and realize the palette... */
    Palette = CreatePalette(pPal);
    SelectPalette(DC, Palette, FALSE);
    RealizePalette(DC);

    free(pPal);
}


/*
 * 'WinProc()' - Handle window events in the window.
 */

LRESULT CALLBACK
WinProc(HWND   hWnd,   /* I - Window triggering this event */
        UINT   uMsg,   /* I - Message type */
        WPARAM wParam, /* I - 'word' parameter value */
        LPARAM lParam) /* I - 'long' parameter value */
    {
    PAINTSTRUCT ps;    /* WM_PAINT message info */
    RECT        rect;  /* Current client area rectangle */


    switch (uMsg)
        {
        case WM_CREATE :
            /*
             * 'Create' message.  Get device and rendering contexts, and 
	     * setup the client area for OpenGL drawing...
	     */

            DC = GetDC(hWnd);
	    SetupContext();
	    break;

        case WM_SIZE :
            /*
	     * Handle resizes...
	     */

            GetClientRect(hWnd, &rect);
	    Resize(rect.right, rect.bottom);

        case WM_PAINT :
            /*
             * Repaint the client area...
	     */

            BeginPaint(hWnd, &ps);
	    Redraw();
	    EndPaint(hWnd, &ps);
	    break;

        case WM_QUIT :
        case WM_CLOSE :
            /*
             * Destroy the window and exit...
             */

            DestroyWindow(Window);
            exit(0);
            break;

        case WM_DESTROY :
            /*
             * Release and free the device context, rendering
             * context, and color palette...
	     */

            if (RC)
    	        wglDeleteContext(RC);

            if (DC)
    	        ReleaseDC(Window, DC);

	    if (Palette)
	        DeleteObject(Palette);

            PostQuitMessage(0);
            break;

        case WM_QUERYNEWPALETTE :
            /*
             * Realize the color palette if necessary...
	     */

            if (Palette)
	        {
                SelectPalette(DC, Palette, FALSE);
                RealizePalette(DC);

                InvalidateRect(hWnd, NULL, FALSE);
	        return (TRUE);
	        }
            break;

        case WM_PALETTECHANGED:
            /*
             * Reselect our color palette if necessary...
	     */

            if (Palette && (HWND)wParam != hWnd)
	        {
                SelectPalette(DC, Palette, FALSE);
                RealizePalette(DC);

                UpdateColors(DC);
	        }
	    break;

        case WM_CHAR :
	    /*
	     * Handle key presses...
	     */

	    Keyboard((unsigned char)wParam, 0, 0);
	    break;

        default :
            /*
             * Pass all other messages through the default window
             * procedure...
             */

            return (DefWindowProc(hWnd, uMsg, wParam, lParam));
        }

    return (FALSE);
    }


/*
 * 'Keyboard()' - Handle key presses...
 */

void
Keyboard(unsigned char key, /* I - Key that was pressed */
         int           x,   /* I - Mouse X position */
	 int           y)   /* I - Mouse Y position */
    {
    if (key == 'd' || key == 'D')
        {
	/* Toggle depth test */
        if (DepthFunc == GL_LESS)
            DepthFunc = GL_GREATER;
        else
            DepthFunc = GL_LESS;

        Redraw();
        }
    else if (key == 0x1b)
        exit(0);
    }


/*
 * 'Redraw()' - Redraw the window...
 */

void
Redraw(void)
    {
    int               i, j;
    static GLUquadric *qobj    = NULL;
    static float      red_light[4]  = {  1.0,  0.0,  0.0,  1.0 };
    static float      red_pos[4]    = {  1.0,  1.0,  1.0,  0.0 };
    static float      blue_light[4] = {  0.0,  0.0,  1.0,  1.0 };
    static float      blue_pos[4]   = { -1.0, -1.0, -1.0,  0.0 };
    static GLfloat    cube_verts[][3] =
        {
	    { -1.0, -1.0,  1.0 }, /* Front bottom left */
	    {  1.0, -1.0,  1.0 }, /* Front bottom right */
	    {  1.0,  1.0,  1.0 }, /* Front top right */
	    { -1.0,  1.0,  1.0 }, /* Front bottom left */
	    { -1.0, -1.0, -1.0 }, /* Back bottom left */
	    {  1.0, -1.0, -1.0 }, /* Back bottom right */
	    {  1.0,  1.0, -1.0 }, /* Back top right */
	    { -1.0,  1.0, -1.0 }  /* Back bottom left */
	};
    static GLfloat    cube_norms[][3] =
        {
	    {  0.0,  0.0,  1.0 }, /* Front */
	    {  0.0,  0.0, -1.0 }, /* Back */
	    { -1.0,  0.0,  0.0 }, /* Left */
	    {  1.0,  0.0,  0.0 }, /* Right */
	    {  0.0, -1.0,  0.0 }, /* Bottom */
	    {  0.0,  1.0,  0.0 }  /* Top */
	};
    static int        cube_points[6][4] =
        {
	    { 0, 1, 2, 3 },       /* Front */
	    { 7, 6, 5, 4 },       /* Back */
	    { 4, 0, 3, 7 },       /* Left */
	    { 1, 5, 6, 2 },       /* Right */
	    { 1, 0, 4, 5 },       /* Bottom */
	    { 3, 2, 6, 7 }        /* Top */
	};

    /* Enable drawing features that we need... */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glShadeModel(GL_SMOOTH);
    glDepthFunc(DepthFunc); 

    /* Clear the color and depth buffers... */

    if (DepthFunc == GL_LESS)
        glClearDepth(1.0);
    else
        glClearDepth(0.0);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
     * Draw the cube and sphere in different colors...
     *
     * We have positioned two lights in this scene.  The first is red
     * and located above, to the right, and behind the viewer.  The
     * second is blue and located below, to the left, and in front of
     * the viewer.
     */

    glLightfv(GL_LIGHT0, GL_DIFFUSE, red_light);
    glLightfv(GL_LIGHT0, GL_POSITION, red_pos);

    glLightfv(GL_LIGHT1, GL_DIFFUSE, blue_light);
    glLightfv(GL_LIGHT1, GL_POSITION, blue_pos);

    glPushMatrix();
    glTranslatef(-1.0, 0.0, -20.0);
    if (!qobj)
        {
	qobj = gluNewQuadric();
	gluQuadricOrientation(qobj, GLU_OUTSIDE);
	}
    gluSphere(qobj, 1.0, 36, 18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.0, 0.0, -20.0);
    glRotatef(15.0, 0.0, 1.0, 0.0);
    glRotatef(15.0, 0.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    for (i = 0; i < 6; i ++)
        {
        glNormal3fv(cube_norms[i]);
        for (j = 0; j < 4; j ++)
	    glVertex3fv(cube_verts[cube_points[i][j]]);
	}
    glEnd();
    glPopMatrix();

    glFinish();
    }


/*
 * 'Resize()' - Resize the window...
 */

void
Resize(int width,  /* I - Width of window */
       int height) /* I - Height of window */
    {
    /* Save the new width and height */
    Width  = width;
    Height = height;

    /* Reset the viewport... */
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(22.5, (float)width / (float)height, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    }
