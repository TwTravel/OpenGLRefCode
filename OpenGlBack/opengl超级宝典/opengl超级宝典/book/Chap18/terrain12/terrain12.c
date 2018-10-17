/*
 * Terrain example 12 from Chapter 18.
 *
 * Written by Michael Sweet
 */

#define BOOK_COVER
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <math.h>
#include "texture.h"

#ifndef WIN32
#  include <sys/time.h>
#endif /* !WIN32 */
#ifndef M_PI
#  define M_PI	(double)3.14159265358979323846
#endif /* !M_PI */


/*
 * Constants:
 */

#ifndef APIENTRY
#  define APIENTRY
#endif /* !APIENTRY */

#define TERRAIN_COUNT    121     /* Number of postings (must be odd) */
#define TERRAIN_SIZE     12000.0 /* Size of terrain database */
#define TERRAIN_VIEW     2000.0  /* Viewable distance in terrain database */
#define TERRAIN_SPACING  100.0   /* Spacing between postings */

/*
 * Terrain cell bits:
 *
 * XX 01 XX
 * 02 04 08
 * XX 10 XX
 *
 * XX = always drawn.
 */

#define TC_TOP    0x01
#define TC_LEFT   0x02
#define TC_CENTER 0x04
#define TC_RIGHT  0x08
#define TC_BOTTOM 0x10
#define TC_VERT   (TC_TOP | TC_CENTER | TC_BOTTOM)

/*
 * Terrain posting structure...
 */

typedef struct
    {
    GLfloat v[3]; /* Position */
    GLfloat n[3]; /* Lighting normal */
    int     used; /* Is this posting used? */
    } TP;


/*
 * Globals...
 */

int        Width = 800;       /* Width of window */
int        Height = 600;      /* Height of window */
GLenum     PolyMode = GL_FILL;/* Polygon drawing mode */
int        UseTexturing = 1;  /* Use texturing? */
int        ShowAircraft = 1;  /* Show the F-16? */
int        ShowFog = 1;       /* Show fog? */
int        ShowLighting = 1;  /* Show lighting? */
int        ShowSky = 1;       /* Show sky? */
int        ShowTerrain = 1;   /* Show 3D terrain? */
int        ShowWater = 1;     /* Show water? */
GLfloat    WaterLevel = 0.0;  /* Level of water */
GLfloat    Velocity = 50.0;   /* Flying speed */
GLfloat	   Position[3] =      /* Position of viewer */
    {
    0.0, 10.0, 0.0
    };
GLfloat	   Orientation[3] =   /* Orientation of viewer */
    {
    0.0, 0.0, 0.0
    };
GLuint     F16Body,           /* F-16 body */
           F16Rolleron[2];    /* F-16 rollerons */
GLuint     F16Texture[2];     /* Camoflage texture objects */
GLuint     TreeList;          /* Display list for tree */
GLuint     TreeTexture;       /* Texture image for tree */
int        TerrainCount;      /* Number of columns of terrain */
TP         Terrain[TERRAIN_COUNT][TERRAIN_COUNT];
                              /* Terrain postings */
GLuint     TerrainList;       /* Terrain display list */
GLuint     LandTexture;       /* Land texture object */
GLuint     SkyTexture;        /* Sky texture object */


/*
 * Functions...
 */

void   BuildF16(void);
void   BuildTree(void);
void   APIENTRY glu_vertex(GLdouble *xyz);
void   LoadTerrain(int lat, int lon);
void   Redraw(void);


/*
 * 'main()' - Open a window and display a sphere and cube.
 */

int                /* O - Exit status */
main(int  argc,    /* I - Number of command-line arguments */
     char *argv[]) /* I - Command-line arguments */
    {
    RECT                  rect; /* Bounding rectangle */
    HDC                   dc;   /* Metafile device context */
    PIXELFORMATDESCRIPTOR pfd;  /* Pixel format description structure */
    int                   pf;   /* Pixel format */
    HGLRC                 rc;   /* OpenGL rendering context */

    /* Create the metafile device context... */
    rect.left   = 0;
    rect.bottom = 0;
    rect.right  = Width;
    rect.top    = Height;

    dc = CreateEnhMetaFile(NULL, "terrain.emf", &rect, "Metafile Terrain");

    /* Set the pixel format... */
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize      = sizeof(pfd);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_SUPPORT_OPENGL;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cRedBits   = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits  = 8;
    pfd.cDepthBits = 16;

    pf = ChoosePixelFormat(dc, &pfd);
    SetPixelFormat(dc, pf, &pfd);

    /* Create and bind the rendering context */
    rc = wglCreateContext(dc);
    wglMakeCurrent(dc, rc);

    /* Load textures and objects... */
    puts("Loading objects and textures...");

    BuildF16();
    BuildTree();

    LandTexture = TextureLoad("land.bmp", GL_FALSE, GL_LINEAR_MIPMAP_LINEAR,
                              GL_LINEAR, GL_REPEAT);
    SkyTexture  = TextureLoad("sky.bmp", GL_FALSE, GL_LINEAR, GL_LINEAR,
                              GL_CLAMP);
    TreeTexture = TextureLoad("tree.bmp", GL_TRUE, GL_LINEAR, GL_LINEAR,
                              GL_CLAMP);

    LoadTerrain(36, -112);

    /* Draw the scene... */
    printf("Rendering scene at %dx%d units...\n", Width, Height);

    Redraw();

    puts("Scene drawn; closing metafile...");
    DeleteEnhMetaFile(CloseEnhMetaFile(dc));

    wglDeleteContext(rc);

    return (0);
    }


/*
 * 'BuildF16()' - Build the F-16 model.
 */

void
BuildF16(void)
    {
    int                i;           /* Looping var */
    GLUquadric         *quadric;    /* Quadric object */
#ifdef GLU_VERSION_1_2
    GLUtesselator      *tess;       /* Tesselator object */
#else
    GLUtriangulatorObj *tess;
#endif /* GLU_VERSION_1_2 */
    static GLdouble    wing[][5] =  /* Main wing points */
        {
	/* { x, y, z, s, t } */
	{  0.25, 0.0, -1.0,  0.125, 0.0 },
	{  0.45, 0.0,  0.0,  0.25,  0.4 },
	{  1.65, 0.0,  0.8,  1.0,   0.8 },
	{  1.65, 0.0,  1.2,  1.0,   1.0 },
	{  0.35, 0.0,  1.2,  0.15,  1.0 },
	{  0.35, 0.0,  2.4,  0.15,  2.0 },
	{  0.25, 0.0,  2.4,  0.125, 2.0 },
	{  0.25, 0.0,  2.0,  0.125, 1.5 },
	{ -0.25, 0.0,  2.0, -0.125, 1.5 },
	{ -0.25, 0.0,  2.4, -0.125, 2.0 },
	{ -0.35, 0.0,  2.4, -0.15,  2.0 },
	{ -0.35, 0.0,  1.2, -0.15,  1.0 },
	{ -1.65, 0.0,  1.2, -1.0,   1.0 },
	{ -1.65, 0.0,  0.8, -1.0,   0.8 },
	{ -0.45, 0.0,  0.0, -0.25,  0.4 },
	{ -0.25, 0.0, -1.0, -0.125, 0.0 }
        };
    static GLdouble    tail[][5] =      /* Tail points */
        {
	/* { x, y, z, s, t } */
	{ 0.0, 0.24, 0.5, 1.5, 0.0 },
	{ 0.0, 0.4,  1.1, 1.2, 0.1 },
	{ 0.0, 1.0,  2.0, 0.4, 1.0 },
	{ 0.0, 1.0,  2.4, 0.05, 1.0 },
	{ 0.0, 0.4,  2.1, 0.2, 0.1 },
	{ 0.0, 0.24, 2.1, 0.2, 0.0 }
	};
    static GLdouble    left_fin[][5] =  /* Left fin points */
        {
	/* { x, y, z, s, t } */
	{ -0.1,  -0.1, 1.1, 0.0, 0.0 },
	{ -0.25, -0.3, 1.2, 1.0, 0.0 },
	{ -0.25, -0.3, 1.5, 1.0, 1.0 },
	{ -0.1,  -0.1, 1.5, 1.0, 0.0 }
	};
    static GLdouble    right_fin[][5] = /* Right fin points */
        {
	/* { x, y, z, s, t } */
	{ 0.1,  -0.1, 1.1, 0.0, 0.0 },
	{ 0.25, -0.3, 1.2, 1.0, 0.0 },
	{ 0.25, -0.3, 1.5, 1.0, 1.0 },
	{ 0.1,  -0.1, 1.5, 1.0, 0.0 }
	};
    static GLdouble    left_rolleron[][5] =  /* Left rolleron points */
        {
	/* { x, y, z, s, t } */
	{ -0.35, 0.0, 1.6, 0.0, 0.0 },
	{ -0.85, 0.0, 2.1, 1.0, 0.5 },
	{ -0.85, 0.0, 2.4, 1.0, 1.0 },
	{ -0.35, 0.0, 2.4, 0.0, 1.0 }
	};
    static GLdouble    right_rolleron[][5] = /* Right rolleron points */
        {
	/* { x, y, z, s, t } */
	{ 0.35, 0.0, 1.6, 0.0, 0.0 },
	{ 0.85, 0.0, 2.1, 1.0, 0.5 },
	{ 0.85, 0.0, 2.4, 1.0, 1.0 },
	{ 0.35, 0.0, 2.4, 0.0, 1.0 }
	};

    /* Load the texture images */
    F16Texture[0] = TextureLoad("camoflage.bmp", GL_FALSE, GL_NEAREST,
                                GL_NEAREST, GL_REPEAT);
    F16Texture[1] = TextureLoad("tail.bmp", GL_FALSE, GL_NEAREST,
                                GL_NEAREST, GL_REPEAT);

    /* Then build the F-16 body */
    F16Body = glGenLists(1);
    glNewList(F16Body, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, F16Texture[0]);

    tess = gluNewTess();
    gluTessCallback(tess, GLU_TESS_BEGIN, glBegin);
    gluTessCallback(tess, GLU_TESS_END, glEnd);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK *)())glu_vertex);

    quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);

    /* Nose */
    glColor3f(0.8, 0.8, 0.8);
    glPushMatrix();
        glTranslatef(0.0, 0.0, -2.5);
	gluCylinder(quadric, 0.0, 0.25, 1.0, 20, 2);
    glPopMatrix();

    /* Main fuselage */
    glColor3f(0.8, 0.8, 0.8);
    glPushMatrix();
        glTranslatef(0.0, 0.0, -1.5);
	gluCylinder(quadric, 0.25, 0.25, 3.5, 20, 2);
    glPopMatrix();

    /* Main wing */
    glColor3f(0.8, 0.8, 0.8);
    glNormal3f(0.0, 1.0, 0.0);
    gluTessBeginPolygon(tess, NULL);
#ifdef GL_VERSION_1_2
    gluTessBeginContour(tess);
#endif /* GL_VERSION_1_2 */

    for (i = 0; i < 16; i ++)
        gluTessVertex(tess, wing[i], wing[i]);

#ifdef GL_VERSION_1_2
    gluTessEndContour(tess);
#endif /* GL_VERSION_1_2 */
    gluTessEndPolygon(tess);

    /* Fins */
    glColor3f(0.8, 0.8, 0.8);
    glNormal3f(-1.0, 0.0, 0.0);
    gluTessBeginPolygon(tess, NULL);
#ifdef GL_VERSION_1_2
    gluTessBeginContour(tess);
#endif /* GL_VERSION_1_2 */

    for (i = 0; i < 4; i ++)
        gluTessVertex(tess, left_fin[i], left_fin[i]);

#ifdef GL_VERSION_1_2
    gluTessEndContour(tess);
#endif /* GL_VERSION_1_2 */
    gluTessEndPolygon(tess);

    glColor3f(0.8, 0.8, 0.8);
    glNormal3f(1.0, 0.0, 0.0);
    gluTessBeginPolygon(tess, NULL);
#ifdef GL_VERSION_1_2
    gluTessBeginContour(tess);
#endif /* GL_VERSION_1_2 */

    for (i = 0; i < 4; i ++)
        gluTessVertex(tess, right_fin[i], right_fin[i]);

#ifdef GL_VERSION_1_2
    gluTessEndContour(tess);
#endif /* GL_VERSION_1_2 */
    gluTessEndPolygon(tess);

    /* Tail */
    glBindTexture(GL_TEXTURE_2D, F16Texture[1]);
    glColor3f(0.8, 0.8, 0.8);
    glNormal3f(1.0, 0.0, 0.0);
    gluTessBeginPolygon(tess, NULL);
#ifdef GL_VERSION_1_2
    gluTessBeginContour(tess);
#endif /* GL_VERSION_1_2 */

    for (i = 0; i < 6; i ++)
        gluTessVertex(tess, tail[i], tail[i]);

#ifdef GL_VERSION_1_2
    gluTessEndContour(tess);
#endif /* GL_VERSION_1_2 */
    gluTessEndPolygon(tess);

    /* Don't texture any more of the body... */
    glDisable(GL_TEXTURE_2D);

    /* Canopy */
    glPushMatrix();
        glColor4f(0.5, 0.5, 1.0, 0.75);
        glTranslatef(0.0, 0.2, -1.0);
	glScalef(1.0, 1.0, 0.65 / 0.15); 
	gluSphere(quadric, 0.15, 6, 12);
    glPopMatrix();

    /* Engine */
    glPushMatrix();
        /* Cowling */
        glColor3f(0.1, 0.1, 0.1);
        glTranslatef(0.0, 0.0, 2.0);
	gluCylinder(quadric, 0.25, 0.15, 0.5, 20, 2);
	gluDisk(quadric, 0.0, 0.25, 20, 2);

        /* Exhaust */
	glPushAttrib(GL_LIGHTING_BIT);
	glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
        glColor3f(0.5, 0.5, 1.0);
	gluCylinder(quadric, 0.2, 0.0, 0.3, 10, 2);
        glColor4f(0.25, 0.25, 1.0, 0.75);
	gluCylinder(quadric, 0.2, 0.1, 0.4, 10, 2);
	glPopAttrib();
    glPopMatrix();

    glEndList();

    /* Now the left rolleron */
    F16Rolleron[0] = glGenLists(1);
    glNewList(F16Rolleron[0], GL_COMPILE);

    glBindTexture(GL_TEXTURE_2D, F16Texture[0]);
    glColor3f(0.8, 0.8, 0.8);
    glNormal3f(0.0, 1.0, 0.0);
    gluTessBeginPolygon(tess, NULL);
#ifdef GL_VERSION_1_2
    gluTessBeginContour(tess);
#endif /* GL_VERSION_1_2 */

    for (i = 0; i < 4; i ++)
        gluTessVertex(tess, left_rolleron[i], left_rolleron[i]);

#ifdef GL_VERSION_1_2
    gluTessEndContour(tess);
#endif /* GL_VERSION_1_2 */
    gluTessEndPolygon(tess);

    glEndList();

    /* And the right rolleron */
    F16Rolleron[1] = glGenLists(1);
    glNewList(F16Rolleron[1], GL_COMPILE);

    glBindTexture(GL_TEXTURE_2D, F16Texture[0]);
    glColor3f(0.8, 0.8, 0.8);
    glNormal3f(0.0, 1.0, 0.0);
    gluTessBeginPolygon(tess, NULL);
#ifdef GL_VERSION_1_2
    gluTessBeginContour(tess);
#endif /* GL_VERSION_1_2 */

    for (i = 0; i < 4; i ++)
        gluTessVertex(tess, right_rolleron[i], right_rolleron[i]);

#ifdef GL_VERSION_1_2
    gluTessEndContour(tess);
#endif /* GL_VERSION_1_2 */
    gluTessEndPolygon(tess);

    glEndList();

    gluDeleteQuadric(quadric);
    gluDeleteTess(tess);
    }


/*
 * 'glu_vertex()' - Set a vertex.
 */

void APIENTRY
glu_vertex(GLdouble *xyz)  /* I - XYZ location + ST texture coordinate */
    {
    glTexCoord2dv(xyz + 3);
    glVertex3dv(xyz);
    }


/*
 * 'BuildTree()' - Build the tree model.
 */

void
BuildTree(void)
    {
    /* The tree model is two rectangles at right angles using a transparent tree texture. */
    TreeList = glGenLists(1);
    glNewList(TreeList, GL_COMPILE);
    glBegin(GL_QUADS);
	glTexCoord2i(0, 0);
	glVertex2f(-1.0, 0.0);
	glTexCoord2i(1, 0);
	glVertex2f(1.0, 0.0);
	glTexCoord2i(1, 1);
	glVertex2f(1.0, 4.0);
	glTexCoord2i(0, 1);
	glVertex2f(-1.0, 4.0);

	glTexCoord2i(0, 0);
	glVertex3f(0.0, 0.0, 1.0);
	glTexCoord2i(1, 0);
	glVertex3f(0.0, 0.0, -1.0);
	glTexCoord2i(1, 1);
	glVertex3f(0.0, 4.0, -1.0);
	glTexCoord2i(0, 1);
	glVertex3f(0.0, 4.0, 1.0);
    glEnd();
    glEndList();
    }

/*
 * 'LoadTerrain()' - Load Digital Terrain Elevation Data (DTED) from disk...
 */

void
LoadTerrain(int lat,              /* I - Latitude in degrees */
            int lon)              /* I - Longitude in degrees */
    {
    int           i, j;           /* Looping vars */
    TP            *tp;            /* Terrain posting */
    gzFile        fp;             /* DTED file */
    char          filename[255];  /* Name of DTED file */
    unsigned char buf[254],       /* Record buffer */
                  *bufptr;        /* Pointer to current elevation */
    GLfloat       escale,         /* East-west scaling */
                  nscale;         /* North-south scaling */
    GLfloat       nx, ny, nz, nw; /* Normal components */
    int           cell;           /* Current cell */
    int           strip;          /* 0 = no strip, 1 or -1 = strip */
    int           num_triangles;  /* Number of triangles all together */

    /* Set the size of the terrain... */
    TerrainCount = (abs(lat) >= 80) ? 21 : (abs(lat) >= 75) ? 31 :
                   (abs(lat) >= 70) ? 41 : (abs(lat) >= 50) ? 61 : 121;
    nscale       = TERRAIN_SIZE / (TERRAIN_COUNT - 1);
    escale       = TERRAIN_SIZE / (TerrainCount - 1);

    /* Try opening the DTED file... */
    sprintf(filename, "dted/%c%03d/%c%02d.dt0",
            lon < 0 ? 'w' : 'e', abs(lon), lat < 0 ? 's' : 'n', abs(lat));
    if ((fp = gzopen(filename, "rb")) != NULL)
        {
        printf("Reading %s...\n", filename);

        /*
         * Seek to the first data record and read all the columns.  This
	 * offset is valid for all Level 0 and most Level 1 and 2 DTED
	 * files.
         */

        gzseek(fp, 3436, SEEK_SET);

        WaterLevel = 100000.0;

        for (i = 0, tp = Terrain[0]; i < TERRAIN_COUNT; i ++)
            {
	    printf("\rColumn %d", i);
	    fflush(stdout);

            gzread(fp, buf, sizeof(buf)); /* Read record */

            for (j = TERRAIN_COUNT - 1, bufptr = buf;
	         j >= 0;
		 j --, bufptr += 2, tp ++)
                {
		/* Convert signed-magnitude */
	        if (*bufptr & 0x80)
                    tp->v[1] = -0.5 * (((bufptr[0] & 0x7f) << 8) | bufptr[1]);
	        else
	            tp->v[1] = 0.5 * ((bufptr[0] << 8) | bufptr[1]);

                if (tp->v[1] < WaterLevel)
		    WaterLevel = tp->v[1];
		}
            }

        gzclose(fp);

        WaterLevel += 200.0; /* Purely arbitrary... */

        puts("\rDone reading terrain.");
        }
    else
        memset(Terrain, 0, sizeof(Terrain));

    /* Set the coordinates of each posting */
    for (i = 0, tp = Terrain[0]; i < TerrainCount; i ++)
        for (j = 0; j < TERRAIN_COUNT; j ++, tp ++)
            {
	    tp->v[0] = (i - TerrainCount / 2) * escale;
	    tp->v[2] = (TERRAIN_COUNT / 2 - j) * nscale;
	    tp->used = 0;
            }

    /*
     * Loop through the terrain arrays and regenerate the
     * lighting normals based on the terrain height.
     */
    for (i = 0, tp = Terrain[0]; i < (TerrainCount - 1); i ++, tp ++)
        for (j = 0; j < (TERRAIN_COUNT - 1); j ++, tp ++)
            {
            /*
             * Compute the cross product of the vectors above and to
             * the right (simplified for this special case).
             */

            nx = tp[0].v[1] - tp[1].v[1];
            ny = -1.0;
            nz = tp[0].v[1] - tp[TERRAIN_COUNT].v[1];
            nw = -1.0 / sqrt(nx * nx + ny * ny + nz * nz);

            /* Normalize the normal vector and store it... */
            tp->n[0] = nx * nw;
            tp->n[1] = ny * nw;
            tp->n[2] = nz * nw;
            }

    /*
     * Set the top and right normals to be the same as the
     * second-to-last normals.
     */

    for (i = 0, tp = Terrain[TerrainCount - 2];
         i < TERRAIN_COUNT;
	 i ++, tp ++)
        {
	  tp[TERRAIN_COUNT].n[0] = tp[0].n[0];
	  tp[TERRAIN_COUNT].n[1] = tp[0].n[1];
	  tp[TERRAIN_COUNT].n[2] = tp[0].n[2];
	}

    for (i = 0, tp = Terrain[0] + TERRAIN_COUNT - 2;
         i < (TerrainCount - 1);
	 i ++, tp += TERRAIN_COUNT)
        {
	  tp[1].n[0] = tp[0].n[0];
	  tp[1].n[1] = tp[0].n[1];
	  tp[1].n[2] = tp[0].n[2];
	}

    /* Position the viewer just above the terrain */
    Position[1] = Terrain[TerrainCount / 2][TERRAIN_COUNT / 2].v[1] + 10.0;

    /* Now scan the terrain for "interesting" postings */
    for (i = TerrainCount, tp = Terrain[0] + 1; i > 0; i --, tp += 2)
        for (j = TERRAIN_COUNT - 2; j > 0; j --, tp ++)
	    if (fabs(2 * tp[0].v[1] - tp[1].v[1] - tp[-1].v[1]) > 100.0)
	        tp->used = 1;

    for (j = 0; j < TERRAIN_COUNT; j ++)
        for (i = TerrainCount - 2, tp = Terrain[1] + j;
	     i > 0;
	     i --, tp += TERRAIN_COUNT)
	    if (fabs(2 * tp[0].v[1] - tp[TERRAIN_COUNT].v[1] -
	             tp[-TERRAIN_COUNT].v[1]) > 100.0)
	        tp->used = 1;

    /* Average postings that are not used... */
    for (i = 0; i < TerrainCount; i += 2)
        for (j = TERRAIN_COUNT / 2, tp = Terrain[i] + 1;
	     j > 0;
	     j --, tp += 2)
	    if (!tp->used)
		tp[0].v[1] = 0.5 * (tp[-1].v[1] + tp[1].v[1]);

    for (j = 0; j < TERRAIN_COUNT; j += 2)
        for (i = TerrainCount / 2, tp = Terrain[1] + j;
	     i > 0;
	     i --, tp += 2)
	    if (!tp->used)
		tp[0].v[1] = 0.5 * (tp[-TERRAIN_COUNT].v[1] + tp[TERRAIN_COUNT].v[1]);

    for (i = 1; i < TerrainCount; i += 2)
        for (j = TERRAIN_COUNT / 2, tp = Terrain[i] + 1;
	     j > 0;
	     j --, tp += 2)
	    if (!tp->used)
		tp[0].v[1] = 0.25 * (tp[-1].v[1] + tp[1].v[1] +
		                     tp[-TERRAIN_COUNT].v[1] +
				     tp[TERRAIN_COUNT].v[1]);

    /* Now build a display list with the tessellation... */
    TerrainList = glGenLists(1);
    glNewList(TerrainList, GL_COMPILE);

    num_triangles = 0;

    for (i = TerrainCount / 2, tp = Terrain[0];
         i > 0;
         i --, tp += TERRAIN_COUNT + 1)
        {
        /* Start this pass with no strips going... */
	strip = 0;

	for (j = TERRAIN_COUNT / 2; j > 0; j --, tp += 2)
	    {
	    /* Figure out which points in the 3x3 cell are set... */
	    cell = 0;
	    
	    if (tp[1].used)
	        cell |= TC_LEFT;
	    if (tp[TERRAIN_COUNT].used)
	        cell |= TC_BOTTOM;
	    if (tp[TERRAIN_COUNT + 1].used)
	        cell |= TC_CENTER;
	    if (tp[TERRAIN_COUNT + 2].used)
	        cell |= TC_TOP;
	    if (tp[2 * TERRAIN_COUNT + 1].used)
	        cell |= TC_RIGHT;

            if ((cell & TC_VERT) == 0)
	        {
		/* OK, this cell can be represented using strips */
		if (strip == 0)
		    {
                    glBegin(GL_TRIANGLE_STRIP);
		    if (cell == TC_RIGHT)
		        {
		        glNormal3fv(tp[0].n);
                        glVertex3fv(tp[0].v);
		        glNormal3fv(tp[2 * TERRAIN_COUNT].n);
                        glVertex3fv(tp[2 * TERRAIN_COUNT].v);
		        glNormal3fv(tp[2 * TERRAIN_COUNT + 1].n);
                        glVertex3fv(tp[2 * TERRAIN_COUNT + 1].v);
			glEnd();

			glBegin(GL_TRIANGLE_STRIP);
		        glNormal3fv(tp[0].n);
                        glVertex3fv(tp[0].v);
		        glNormal3fv(tp[2 * TERRAIN_COUNT + 1].n);
                        glVertex3fv(tp[2 * TERRAIN_COUNT + 1].v);
			}
		    else
		        {
		        glNormal3fv(tp[0].n);
                        glVertex3fv(tp[0].v);
		        glNormal3fv(tp[2 * TERRAIN_COUNT].n);
                        glVertex3fv(tp[2 * TERRAIN_COUNT].v);

			}

		    strip = 1;
		    }

                if (cell == (TC_LEFT | TC_RIGHT))
		    {
		    if (strip > 0)
		        {
		        glNormal3fv(tp[1].n);
                        glVertex3fv(tp[1].v);
		        glNormal3fv(tp[2 * TERRAIN_COUNT + 1].n);
                        glVertex3fv(tp[2 * TERRAIN_COUNT + 1].v);
			}
		    else
		        {
		        glNormal3fv(tp[2 * TERRAIN_COUNT + 1].n);
                        glVertex3fv(tp[2 * TERRAIN_COUNT + 1].v);
		        glNormal3fv(tp[1].n);
                        glVertex3fv(tp[1].v);
			}

		    num_triangles += 2;
		    }
		else if (cell == TC_LEFT)
		    {
		    glNormal3fv(tp[1].n);
                    glVertex3fv(tp[1].v);

		    if (strip < 0)
		        {
			glEnd();

			strip = 1;

			glBegin(GL_TRIANGLE_STRIP);
		        glNormal3fv(tp[1].n);
                        glVertex3fv(tp[1].v);
		        glNormal3fv(tp[2 * TERRAIN_COUNT].n);
                        glVertex3fv(tp[2 * TERRAIN_COUNT].v);
			}
		    else
		        strip = -strip;

		    num_triangles ++;
		    }
		else if (cell == TC_RIGHT)
		    {
		    glNormal3fv(tp[2 * TERRAIN_COUNT + 1].n);
                    glVertex3fv(tp[2 * TERRAIN_COUNT + 1].v);

		    if (strip > 0)
		        {
			glEnd();

			glBegin(GL_TRIANGLE_STRIP);
		        glNormal3fv(tp[0].n);
                        glVertex3fv(tp[0].v);
		        glNormal3fv(tp[2 * TERRAIN_COUNT + 1].n);
                        glVertex3fv(tp[2 * TERRAIN_COUNT + 1].v);
			}
		    else
		        strip = -strip;

		    num_triangles ++;
		    }

                if (strip > 0)
		    {
		    glNormal3fv(tp[2].n);
                    glVertex3fv(tp[2].v);
		    glNormal3fv(tp[2 * TERRAIN_COUNT + 2].n);
                    glVertex3fv(tp[2 * TERRAIN_COUNT + 2].v);
		    }
		else
		    {
		    glNormal3fv(tp[2 * TERRAIN_COUNT + 2].n);
                    glVertex3fv(tp[2 * TERRAIN_COUNT + 2].v);
		    glNormal3fv(tp[2].n);
                    glVertex3fv(tp[2].v);
		    }

		num_triangles += 2;
		}
	    else
	        {
	        if (strip)
		    {
		    glEnd();
		    strip = 0;
		    }

                glBegin(GL_TRIANGLE_FAN);
		glNormal3fv(tp[TERRAIN_COUNT + 1].n);
                glVertex3fv(tp[TERRAIN_COUNT + 1].v);

		glNormal3fv(tp[0].n);
                glVertex3fv(tp[0].v);

                if (cell & TC_BOTTOM)
		    {
		    num_triangles ++;
		    glNormal3fv(tp[TERRAIN_COUNT].n);
                    glVertex3fv(tp[TERRAIN_COUNT].v);
		    }

		num_triangles ++;
		glNormal3fv(tp[2 * TERRAIN_COUNT].n);
                glVertex3fv(tp[2 * TERRAIN_COUNT].v);
                
		if (cell & TC_RIGHT)
		    {
		    num_triangles ++;
		    glNormal3fv(tp[2 * TERRAIN_COUNT + 1].n);
                    glVertex3fv(tp[2 * TERRAIN_COUNT + 1].v);
		    }

		num_triangles ++;
		glNormal3fv(tp[2 * TERRAIN_COUNT + 2].n);
                glVertex3fv(tp[2 * TERRAIN_COUNT + 2].v);

                if (cell & TC_TOP)
		    {
		    num_triangles ++;
		    glNormal3fv(tp[TERRAIN_COUNT + 2].n);
                    glVertex3fv(tp[TERRAIN_COUNT + 2].v);
		    }

		num_triangles ++;
		glNormal3fv(tp[2].n);
                glVertex3fv(tp[2].v);

		if (cell & TC_LEFT)
		    {
		    num_triangles ++;
		    glNormal3fv(tp[1].n);
                    glVertex3fv(tp[1].v);
		    }

		num_triangles ++;
		glNormal3fv(tp[0].n);
                glVertex3fv(tp[0].v);
		glEnd();
		}
	    }

        if (strip)
	    glEnd();
	}

    glEndList();

    printf("Number triangles = %d\n", num_triangles);
    }


/*
 * 'Redraw()' - Redraw the window...
 */

void
Redraw(void)
    {
    int     i, j;          /* Looping vars */
    TP      *tp;           /* Current terrain posting */
    float   x, y;          /* Position between postings */
    float   theta, st, ct; /* Sky coordinates */
    static GLfloat sunpos[4] = { 0.7071, 0.7071, 0.0, 0.0 };
    static GLfloat suncolor[4] = { 0.5, 0.5, 0.4, 1.0 };
    static GLfloat sunambient[4] = { 0.5, 0.5, 0.4, 1.0 };
    static GLfloat fogcolor[4] = { 1.0, 1.0, 1.0, 1.0 };
    static GLfloat s_vector[4] = { 150.0 / TERRAIN_SIZE, 0.0, 0.0, 0.0 };
    static GLfloat t_vector[4] = { 0.0, 0.0, 150.0 / TERRAIN_SIZE, 0.0 };

    /* Reset the viewport... */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(22.5, (float)Width / (float)Height, 4.0, TERRAIN_VIEW);
    if (Width < Height)
        glRotatef(90.0, 0.0, 0.0, 1.0); /* Rotate for landscape orientation */
    glMatrixMode(GL_MODELVIEW);

    /* Clear the window to light blue... */
    if (ShowFog)
        glClearColor(0.8, 0.8, 1.0, 1.0);
    else
        glClearColor(0.75, 0.75, 1.0, 1.0);

    puts("Clearing the bitmap...");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Setup viewing transformations for the current orientation... */
    glPushMatrix();

    if (ShowAircraft)
        {
	glTranslatef(-2.5, 0.0, -25.0);
	glRotatef(Orientation[2], 0.0, 0.0, 1.0);
	glRotatef(Orientation[0], -1.0, 0.0, 0.0);
        glRotatef(Orientation[1], 0.0, 1.0, 0.0);
        glTranslatef(0.0, -2.5, -2.5);
	}
    else
	{
	glRotatef(Orientation[2], 0.0, 0.0, 1.0);
	glRotatef(Orientation[0], -1.0, 0.0, 0.0);
        glRotatef(Orientation[1], 0.0, 1.0, 0.0);
	}

    /* Show fog if needed... */
    if (ShowFog)
        {
	glFogi(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, 0.0005);
	glFogfv(GL_FOG_COLOR, fogcolor);
	glEnable(GL_FOG);
	}
    else
        glDisable(GL_FOG);

    glTranslatef(0.0, -Position[1], 0.0);

    glPolygonMode(GL_FRONT_AND_BACK, PolyMode);
    glDisable(GL_LIGHTING);

    /* Draw the sky */
    if (SkyTexture && ShowSky && UseTexturing && PolyMode == GL_FILL)
        {
	puts("Drawing the sky...");

        glShadeModel(GL_SMOOTH);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, SkyTexture);

        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0.5, 0.5);
        glVertex3f(0.0, TERRAIN_VIEW, 0.0);
        for (theta = 0.0; theta < (2.1 * M_PI); theta += M_PI / 8)
            {
            ct = cos(theta);
            st = sin(theta);
            glTexCoord2f(0.5 + 0.3 * ct, 0.5 + 0.3 * st);
            glVertex3f(ct * TERRAIN_VIEW * 0.7071,
	               TERRAIN_VIEW * 0.7071,
	               st * TERRAIN_VIEW * 0.7071);
            }
        glEnd();

        glBegin(GL_TRIANGLE_STRIP);
        for (theta = 0.0; theta < (2.1 * M_PI); theta += M_PI / 8)
            {
            ct = cos(theta);
            st = sin(theta);
            glTexCoord2f(0.5 + 0.3 * ct, 0.5 + 0.3 * st);
            glVertex3f(ct * TERRAIN_VIEW * 0.7071,
	               TERRAIN_VIEW * 0.7071,
	               st * TERRAIN_VIEW * 0.7071);
            glTexCoord2f(0.5 + 0.5 * ct, 0.5 + 0.5 * st);
            glVertex3f(ct * TERRAIN_VIEW, -10.0,
	               st * TERRAIN_VIEW);
            }
        glEnd();
        }

    /* Setup lighting if needed... */
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, sunambient);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
    glEnable(GL_COLOR_MATERIAL);

    if (ShowLighting)
        {
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, sunpos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, suncolor);
        glLightfv(GL_LIGHT0, GL_AMBIENT, sunambient);
        }
    else
        glDisable(GL_LIGHT0);

    /* Then draw the terrain... */
    puts("Drawing the terrain...");

    if (UseTexturing && LandTexture && PolyMode == GL_FILL)
        {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, LandTexture);
	glShadeModel(GL_SMOOTH);
        }
    else
        {
        glDisable(GL_TEXTURE_2D);
	glShadeModel(GL_FLAT);
	}

    glTranslatef(-Position[0], 0.0, -Position[2]);

    glColor3f(0.15, 0.4, 0.1);

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, s_vector);

    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, t_vector);

    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    if (ShowTerrain)
        {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glCallList(TerrainList);

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        }
     else
        {
        glBegin(GL_QUADS);
        glNormal3f(0.0, 1.0, 0.0);
        glVertex3f(-0.5 * TERRAIN_SIZE, 0.0, -0.5 * TERRAIN_SIZE);
        glVertex3f( 0.5 * TERRAIN_SIZE, 0.0, -0.5 * TERRAIN_SIZE);
        glVertex3f( 0.5 * TERRAIN_SIZE, 0.0,  0.5 * TERRAIN_SIZE);
        glVertex3f(-0.5 * TERRAIN_SIZE, 0.0,  0.5 * TERRAIN_SIZE);
        glEnd();
	}

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    if (ShowWater && ShowTerrain)
        {
	puts("Drawing the water...");

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glColor4f(0.0, 0.0, 0.25, 0.75);
	glNormal3f(0.0, 1.0, 0.0);

        for (i = 0; i < (TERRAIN_COUNT / 10); i ++)
	    {
	    glBegin(GL_TRIANGLE_STRIP);
            glNormal3f(0.0, 1.0, 0.0);
            for (j = 0; j <= (TERRAIN_COUNT / 10); j ++)
	        {
		glTexCoord2i(i, j);
		glVertex3f(i * 10.0 * TERRAIN_SPACING - 0.5 * TERRAIN_SIZE,
		           WaterLevel,
		           0.5 * TERRAIN_SIZE - j * 10.0 * TERRAIN_SPACING);

		glTexCoord2i(i + 1, j);
		glVertex3f((i + 1) * 10.0 * TERRAIN_SPACING - 0.5 * TERRAIN_SIZE,
		           WaterLevel,
		           0.5 * TERRAIN_SIZE - j * 10.0 * TERRAIN_SPACING);
		}
	    glEnd();
	    }

        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        }

    /* Draw some trees... */
    if (UseTexturing)
        {
        glBindTexture(GL_TEXTURE_2D, TreeTexture);
	glEnable(GL_TEXTURE_2D);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	}

    glEnable(GL_DEPTH_TEST);

    if (ShowLighting)
        glDisable(GL_LIGHT0);

    puts("Drawing the trees...");

    glColor3f(0.0, 0.2, 0.1);
    srand(0); /* Make sure the trees always show up in the same spot */

    for (i = 0; i < 20000; i ++)
        {
	/* Offset the tree to the posting + some random offset */
	x = (rand() % 1100) * 0.01 + TerrainCount / 2 - 5;
	y = TERRAIN_COUNT / 2 + 5 - i * 0.0005;

        tp = Terrain[(int)x] + (int)y;
	x  = fmod(x, 1.0);
	y  = fmod(y, 1.0);

	glPushMatrix();
	glTranslatef((1.0 - x) * tp[0].v[0] + x * tp[TERRAIN_COUNT].v[0],
	             (1.0 - y) * ((1.0 - x) * tp[0].v[1] + x * tp[TERRAIN_COUNT].v[1]) +
		     y * ((1.0 - x) * tp[1].v[1] + x * tp[TERRAIN_COUNT + 1].v[1]),
	             (1.0 - y) * tp[0].v[2] + y * tp[1].v[2]);
	glCallList(TreeList);
	glPopMatrix();
	}

    if (ShowLighting)
        glEnable(GL_LIGHT0);

    if (ShowAircraft)
        {
	puts("Drawing the aircraft...");

        /* Setup viewing transformations for the F-16... */
        glPushMatrix();
        glTranslatef(Position[0], Position[1], Position[2]);

        glRotatef(Orientation[1], 0.0, -1.0, 0.0);
        glRotatef(Orientation[0], 1.0, 0.0, 0.0);
        glRotatef(Orientation[2], 0.0, 0.0, -1.0);

        glEnable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (UseTexturing)
	    glEnable(GL_TEXTURE_2D);
        else
            glDisable(GL_TEXTURE_2D);

        // Draw the main body
        glCallList(F16Body);

        // Draw the rollerons...
        if (UseTexturing)
	    glEnable(GL_TEXTURE_2D);

        glCallList(F16Rolleron[0]);
	glCallList(F16Rolleron[1]);

        glPopMatrix();

#ifdef BOOK_COVER
        /**** Show an extra aircraft for the book cover... ****/
        glPushMatrix();
        glTranslatef(Position[0] + 7.5, Position[1] - 2.5, Position[2] - 10.0);

        glRotatef(Orientation[1] + 345.0, 0.0, -1.0, 0.0);
        glRotatef(Orientation[0], 1.0, 0.0, 0.0);
        glRotatef(Orientation[2] - 45.0, 0.0, 0.0, -1.0);

        glEnable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (UseTexturing)
	    glEnable(GL_TEXTURE_2D);
        else
            glDisable(GL_TEXTURE_2D);

        // Draw the main body
        glCallList(F16Body);

        // Draw the rollerons...
        if (UseTexturing)
	    glEnable(GL_TEXTURE_2D);

	glCallList(F16Rolleron[0]);
	glCallList(F16Rolleron[1]);

        glPopMatrix();
#endif /* BOOK_COVER */
	}

    glPopMatrix();

    /* Finish up */
    glFlush();
    }
