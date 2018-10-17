/*
 * OpenGL galaxy demo from Chapter 7.
 *
 * Written by Michael Sweet.
 */

/*
 * Include necessary headers.
 */

#include "bitmap.h"
#include <GL/glut.h>


/*
 * Globals...
 */

char       *Filenames[] =          /* Bitmap files to load */
           {
	   "cluster.bmp",
	   "nova.bmp",
	   "spiral.bmp",
	   "nova.bmp",
	   "nova2.bmp",
	   "nova.bmp",
	   "spiral.bmp"
	   };
#define NUM_FILES (sizeof(Filenames) / sizeof(Filenames[0]))

int        Width;                  /* Width of window */
int        Height;                 /* Height of window */
BITMAPINFO *BitmapInfo[NUM_FILES]; /* Bitmap information */
GLubyte    *BitmapBits[NUM_FILES]; /* Bitmap data */


/*
 * Functions...
 */

GLubyte *LoadRGBA(const char *filename, BITMAPINFO **info);
void    Redraw(void);
void    Resize(int width, int height);


/*
 * 'main()' - Open a window and display a bitmap.
 */

int                /* O - Exit status */
main(int  argc,    /* I - Number of command-line arguments */
     char *argv[]) /* I - Command-line arguments */
    {
    int i;         /* Looping var */

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(792, 573);
    glutCreateWindow("Galaxy");
    glutReshapeFunc(Resize);
    glutDisplayFunc(Redraw);

    for (i = 0; i < NUM_FILES; i ++)
        BitmapBits[i] = LoadRGBA(Filenames[i], &(BitmapInfo[i]));

    glutMainLoop();
    return (0);
    }


GLubyte *                      /* O - RGBA bitmap data */
LoadRGBA(const char *filename, /* I - Filename */
         BITMAPINFO **info)    /* O - Bitmap information */
    {
    GLubyte    *bgr;           /* BGR bitmap data */
    GLubyte    *rgba;          /* RGBA bitmap data */
    int        x, y;           /* Looping vars */
    int        length;         /* Length of scanline */
    GLubyte    *bgr_ptr;       /* Pointer into BGR data */
    GLubyte    *rgba_ptr;      /* Pointer into RGBA data */

    bgr    = LoadDIBitmap(filename, info);
    rgba   = malloc((*info)->bmiHeader.biWidth *
                    (*info)->bmiHeader.biHeight * 4);
    length = ((*info)->bmiHeader.biWidth * 3 + 3) & ~3;

    for (y = 0; y < (*info)->bmiHeader.biHeight; y ++)
        {
        bgr_ptr  = bgr + y * length;
        rgba_ptr = rgba + y * (*info)->bmiHeader.biWidth * 4;

        for (x = 0; x < (*info)->bmiHeader.biWidth; x ++, bgr_ptr += 3, rgba_ptr += 4)
            {
            rgba_ptr[0] = bgr_ptr[2];
            rgba_ptr[1] = bgr_ptr[1];
            rgba_ptr[2] = bgr_ptr[0];
            rgba_ptr[3] = (bgr_ptr[0] + bgr_ptr[1] + bgr_ptr[2]) / 3;
            }
        }
    free(bgr);
    return (rgba);
    }


/*
 * 'Redraw()' - Redraw the window...
 */

void
Redraw(void)
    {
    int i, j;  /* Looping vars */
    int count; /* Number of stars/galaxies to draw */


    /* Clear the window to black */
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw stars... */
    count = Width * Height / 500;

    glBegin(GL_POINTS);

    glColor3f(0.5, 0.0, 0.0);
    for (i = 0; i < count; i ++)
        glVertex2i(rand() % Width, rand() % Height);

    count /= 2;
    glColor3f(1.0, 1.0, 0.9);
    for (i = 0; i < count; i ++)
        glVertex2i(rand() % Width, rand() % Height);
    
    glColor3f(0.9, 0.9, 1.0);
    for (i = 0; i < count; i ++)
        glVertex2i(rand() % Width, rand() % Height);
    glEnd();

    /* Draw galaxies */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    count /= 25;
    for (i = 0; i < count; i ++)
        {
	j = rand() % NUM_FILES;
        glRasterPos2i(rand() % Width, rand() % Height);
	glBitmap(0, 0, 0, 0, -BitmapInfo[j]->bmiHeader.biWidth / 2,
	         -BitmapInfo[j]->bmiHeader.biHeight / 2, NULL);
        glDrawPixels(BitmapInfo[j]->bmiHeader.biWidth,
                     BitmapInfo[j]->bmiHeader.biHeight,
                     GL_RGBA, GL_UNSIGNED_BYTE, BitmapBits[j]);
        }

    glDisable(GL_BLEND);

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
    glOrtho(0.0, (GLfloat)width, 0.0, (GLfloat)height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    }
