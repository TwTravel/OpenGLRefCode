
#include <math.h>

//#include "gl\glaux.h"
#include <gl\gl.h>

#define PI 3.14159265358979323846

GLenum rgb=GL_TRUE, doubleBuffer;

GLint colorIndexes1[3];
GLint colorIndexes2[3];

GLenum smooth = GL_TRUE;
GLenum lighting = GL_TRUE;
GLenum depth = GL_TRUE;
GLenum stepMode = GL_FALSE;
GLenum spinMode = GL_FALSE;
GLint contouring = 0;

GLint widthX, widthY;
GLint checkerSize;
float height;

GLint frames, curFrame = 0, nextFrame = 0;

struct facet {
    float color[3];
    float normal[3];
};
struct coord {
    float vertex[3];
    float normal[3];
};
struct mesh {
    GLint widthX, widthY;
    GLint numFacets;
    GLint numCoords;
    GLint frames;
    struct coord *coords;
    struct facet *facets;
} theMesh;

GLubyte contourTexture1[] = {
    255, 255, 255, 255,
    255, 255, 255, 255,
    255, 255, 255, 255,
    127, 127, 127, 127,
};
GLubyte contourTexture2[] = {
    255, 255, 255, 255,
    255, 127, 127, 127,
    255, 127, 127, 127,
    255, 127, 127, 127,
};

#define GETCOORD(frame, x, y) (&(theMesh.coords[frame*theMesh.numCoords+(x)+(y)*(theMesh.widthX+1)]))
#define GETFACET(frame, x, y) (&(theMesh.facets[frame*theMesh.numFacets+(x)+(y)*theMesh.widthX]))

static void  Animate(void)
{
    struct coord *coord;
    struct facet *facet;
    float *lastColor;
    float *thisColor;
    GLint i, j;

	rgb = GL_TRUE;

	glColor3f(0.0f,0.0f,1.0f);
//	glDisable(GL_LIGHTING);

/*
    if (nextFrame || !stepMode) 
	{
		curFrame++;
    }
    if (curFrame >= theMesh.frames) 
	{
		curFrame = 0;
    }

    if ((nextFrame || !stepMode) && spinMode) 
	{
		glRotatef(5.0, 0.0, 0.0, 1.0);
    }
    nextFrame = 0;

    for (i = 0; i < theMesh.widthX; i++) 
	{
		glBegin(GL_QUAD_STRIP);
		lastColor = NULL;
		for (j = 0; j < theMesh.widthY; j++) 
		{
			facet = GETFACET(curFrame, i, j);
			if (!smooth && lighting) 
			{
				glNormal3fv(facet->normal);
			}
			if (lighting) 
			{
				if (rgb) 
				{
					thisColor = facet->color;
					glColor3fv(facet->color);
				} 
				else 
				{
					thisColor = facet->color;
					glMaterialfv(GL_FRONT_AND_BACK, GL_COLOR_INDEXES, 
					facet->color);
				}
			} 
			else 
			{
				if (rgb) 
				{
					thisColor = facet->color;
					glColor3fv(facet->color);
				} 
				else 
				{
					thisColor = facet->color;
					glIndexf(facet->color[1]);
				}
			}

			if (!lastColor || (thisColor[0] != lastColor[0] && smooth)) {
			if (lastColor) 
			{
				glEnd();
				glBegin(GL_QUAD_STRIP);
			}
			coord = GETCOORD(curFrame, i, j);
			if (smooth && lighting) 
			{
				glNormal3fv(coord->normal);
			}
			glVertex3fv(coord->vertex);

			coord = GETCOORD(curFrame, i+1, j);
			if (smooth && lighting) 
			{
				glNormal3fv(coord->normal);
			}
			glVertex3fv(coord->vertex);
			}

			coord = GETCOORD(curFrame, i, j+1);
			if (smooth && lighting) 
			{
				glNormal3fv(coord->normal);
			}
			glVertex3fv(coord->vertex);

			coord = GETCOORD(curFrame, i+1, j+1);
			if (smooth && lighting) 
			{
			glNormal3fv(coord->normal);
        
			}
			glVertex3fv(coord->vertex);

			lastColor = thisColor;
    
		}
		glEnd();
    
	}
*/
    if (nextFrame || !stepMode) 
	{
		curFrame++;
    }
    if (curFrame >= theMesh.frames) 
	{
		curFrame = 0;
    }

    if ((nextFrame || !stepMode) && spinMode) 
	{
		glRotatef(5.0, 0.0, 0.0, 1.0);
    }
    nextFrame = 0;

    for (i = 0; i < theMesh.widthX; i++) 
	{
		glBegin(GL_QUAD_STRIP);
		lastColor = NULL;
		for (j = 0; j < theMesh.widthY; j++) 
		{
			facet = GETFACET(curFrame, i, j);
			if (!smooth && lighting) 
			{
				glNormal3fv(facet->normal);
			}
			if (lighting) 
			{
				if (rgb) 
				{
					thisColor = facet->color;
					glColor3fv(facet->color);
				} 
				else 
				{
					thisColor = facet->color;
					glMaterialfv(GL_FRONT_AND_BACK, GL_COLOR_INDEXES, 
					facet->color);
				}
			} 

			if (!lastColor || (thisColor[0] != lastColor[0] && smooth)) 
			{
				if (lastColor) 
				{
					glEnd();
					glBegin(GL_QUAD_STRIP);
				}
				coord = GETCOORD(curFrame, i, j);
				if (smooth && lighting) 
				{
					glNormal3fv(coord->normal);
				}
				glVertex3fv(coord->vertex);

				coord = GETCOORD(curFrame, i+1, j);
				if (smooth && lighting) 
				{
					glNormal3fv(coord->normal);
				}
				glVertex3fv(coord->vertex);
			}

			coord = GETCOORD(curFrame, i, j+1);
			if (smooth && lighting) 
			{
				glNormal3fv(coord->normal);
			}
			glVertex3fv(coord->vertex);

			coord = GETCOORD(curFrame, i+1, j+1);
			if (smooth && lighting) 
			{
			glNormal3fv(coord->normal);
        
			}
			glVertex3fv(coord->vertex);

			lastColor = thisColor;
    
		}
		glEnd();
    
	}

}

static void InitMesh(void)
{
    struct coord *coord;
    struct facet *facet;
    float dp1[3], dp2[3];
    float *pt1, *pt2, *pt3;
    float angle, d, x, y;
    GLint numFacets, numCoords, frameNum, i, j;
/////////////////////
/*
	colorIndexes1[0]=0.2;
	colorIndexes1[1]=0.5;
	colorIndexes1[2]=0.8;
	colorIndexes1[0]=0.5;
	colorIndexes1[1]=0.4;
	colorIndexes1[2]=0.0;

*/

////////////////

	doubleBuffer = GL_TRUE;

    frames = 10;
    widthX = 10;
    widthY = 10;
    checkerSize = 2;
    height = 0.2;


    theMesh.widthX = widthX;
    theMesh.widthY = widthY;
    theMesh.frames = frames;

    numFacets = widthX * widthY;
    numCoords = (widthX + 1) * (widthY + 1);

    theMesh.numCoords = numCoords;
    theMesh.numFacets = numFacets;

    theMesh.coords = (struct coord *)malloc(frames*numCoords*
                        sizeof(struct coord));
    theMesh.facets = (struct facet *)malloc(frames*numFacets*
                        sizeof(struct facet));
    if (theMesh.coords == NULL || theMesh.facets == NULL) 
	{
    
    return;//printf("Out of memory.\n");
    }

    for (frameNum = 0; frameNum < frames; frameNum++) 
	{
		for (i = 0; i <= widthX; i++) 
		{
			x = i / (float)widthX;
			for (j = 0; j <= widthY; j++) 
			{
				y = j / (float)widthY;

				d = sqrt(x*x+y*y);
				if (d == 0.0) 
				{
					d = 0.0001;
        		}
				angle = 2 * PI * d + (2 * PI / frames * frameNum);

				coord = GETCOORD(frameNum, i, j);

				coord->vertex[0] = x - 0.5;
				coord->vertex[1] = y - 0.5;
				coord->vertex[2] = (height - height * d) * cos(angle);

				coord->normal[0] = -(height / d) * x * ((1 - d) * 2 * PI *
                   sin(angle) + cos(angle));
				coord->normal[1] = -(height / d) * y * ((1 - d) * 2 * PI *
                   sin(angle) + cos(angle));
				coord->normal[2] = -1;

				d = 1.0 / sqrt(coord->normal[0]*coord->normal[0]+
                   coord->normal[1]*coord->normal[1]+1);
				coord->normal[0] *= d;
				coord->normal[1] *= d;
				coord->normal[2] *= d;
				}
    
		}
		for (i = 0; i < widthX; i++) 
		{
			for (j = 0; j < widthY; j++) 
			{
				facet = GETFACET(frameNum, i, j);
				if (((i/checkerSize)%2)^(j/checkerSize)%2) 
				{
					if (rgb) 
					{
						facet->color[0] = 1.0;
						facet->color[1] = 0.2;
						facet->color[2] = 0.2;
            
					} 
					else 
					{
						facet->color[0] = colorIndexes1[0];
						facet->color[1] = colorIndexes1[1];
						facet->color[2] = colorIndexes1[2];
					}
				} 
				else 
				{
					if (rgb) 
					{
						facet->color[0] = 0.2;
						facet->color[1] = 1.0;
						facet->color[2] = 0.2;
					} 
					else 
					{
						facet->color[0] = colorIndexes2[0];
						facet->color[1] = colorIndexes2[1];
						facet->color[2] = colorIndexes2[2];
					}
				}
				pt1 = GETCOORD(frameNum, i, j)->vertex;
				pt2 = GETCOORD(frameNum, i, j+1)->vertex;
				pt3 = GETCOORD(frameNum, i+1, j+1)->vertex;

				dp1[0] = pt2[0] - pt1[0];
				dp1[1] = pt2[1] - pt1[1];
				dp1[2] = pt2[2] - pt1[2];

				dp2[0] = pt3[0] - pt2[0];
				dp2[1] = pt3[1] - pt2[1];
				dp2[2] = pt3[2] - pt2[2];

				facet->normal[0] = dp1[1] * dp2[2] - dp1[2] * dp2[1];
				facet->normal[1] = dp1[2] * dp2[0] - dp1[0] * dp2[2];
				facet->normal[2] = dp1[0] * dp2[1] - dp1[1] * dp2[0];

				d = 1.0 / sqrt(facet->normal[0]*facet->normal[0]+
                   facet->normal[1]*facet->normal[1]+
                   facet->normal[2]*facet->normal[2]);

				facet->normal[0] *= d;
				facet->normal[1] *= d;
				facet->normal[2] *= d;
			}
		}
	}
}

/*
static void SetColorMap(void) 
{

    static float green[3] = {0.2, 1.0, 0.2};
    static float red[3] = {1.0, 0.2, 0.2};
    float *color, percent;
    GLint *indexes, entries, i, j;

    entries = auxGetColorMapSize();

    colorIndexes1[0] = 1;
    colorIndexes1[1] = 1 + (GLint)((entries - 1) * 0.3);
    colorIndexes1[2] = (GLint)((entries - 1) * 0.5);
    colorIndexes2[0] = 1 + (GLint)((entries - 1) * 0.5);
    colorIndexes2[1] = 1 + (GLint)((entries - 1) * 0.8);
    colorIndexes2[2] = entries - 1;

    for (i = 0; i < 2; i++) {
    switch (i) {
      case 0:
        color = green;
        indexes = colorIndexes1;
        break;
      case 1:
        color = red;
        indexes = colorIndexes2;
        break;
    }

    for (j = indexes[0]; j < indexes[1]; j++) {
        percent = 0.2 + 0.8 * (j - indexes[0]) /
              (float)(indexes[1] - indexes[0]);
        auxSetOneColor(j, percent*color[0], percent*color[1],
               percent*color[2]);
    }
    for (j=indexes[1]; j<=indexes[2]; j++) {
        percent = (j - indexes[1]) / (float)(indexes[2] - indexes[1]);
        auxSetOneColor(j, percent*(1-color[0])+color[0],
               percent*(1-color[1])+color[1],
               percent*(1-color[2])+color[2]);
    }
    }

	colorIndexes1[0]=0.2;
	colorIndexes1[1]=0.5;
	colorIndexes1[2]=0.8;
	colorIndexes1[0]=0.5;
	colorIndexes1[1]=0.4;
	colorIndexes1[2]=0.0;
}
*/
static void InitMaterials(void)
{
	static float ambient[] = {0.1, 0.1, 0.1, 1.0};
    static float diffuse[] = {0.5, 1.0, 1.0, 1.0};
    static float position[] = {90.0, 90.0, 150.0, 0.0};
    static float front_mat_shininess[] = {60.0};
    static float front_mat_specular[] = {0.2, 0.2, 0.2, 1.0};
    static float front_mat_diffuse[] = {0.5, 0.28, 0.38, 1.0};
    static float back_mat_shininess[] = {60.0};
    static float back_mat_specular[] = {0.5, 0.5, 0.2, 1.0};
    static float back_mat_diffuse[] = {1.0, 1.0, 0.2, 1.0};
    static float lmodel_ambient[] = {1.0, 1.0, 1.0, 1.0};
    static float lmodel_twoside[] = {GL_TRUE};

    glMatrixMode(GL_PROJECTION);
    gluPerspective(450, 1.0, 0.5, 10.0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess);
    glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse);
    glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess);
    glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular);
    glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse);


    if (rgb) 
	{
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    }

    if (rgb) 
	{
		glEnable(GL_COLOR_MATERIAL);
    } 
	else 
	{
//		SetColorMap();
    }
}

static void InitTexture(void)
{
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}
