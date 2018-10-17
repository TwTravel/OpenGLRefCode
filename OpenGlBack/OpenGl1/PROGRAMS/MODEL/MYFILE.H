#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>


#define PI ((float)3.14159265358979323846)

#define COMPUTE_SPHERE_VERTICES(rings, sections) \
   (((rings)+1)*(sections)+2)
#define COMPUTE_SPHERE_TRIANGLES(rings, sections) \
    (((rings)+1)*(sections)*2)
#define RINGS 50
#define SECTIONS 50
#define SVERTICES COMPUTE_SPHERE_VERTICES(RINGS, SECTIONS)
#define STRIANGLES COMPUTE_SPHERE_TRIANGLES(RINGS, SECTIONS)

#define RGB_COLOR(red, green, blue) \
    (((DWORD)(BYTE)(red) << 0) | \
     ((DWORD)(BYTE)(green) << 8) | \
     ((DWORD)(BYTE)(blue) << 16))
    
#define FRANDOM(x) (((float)rand() / RAND_MAX) * (x))

#define DROT 10.0f

#define NLISTS 3
    
typedef struct
{
    float x, y, z;			//点
    float fNx, fNy, fNz;	//法线
    DWORD dwColor;
} VERTEX;

typedef struct
{
    int iV1;
    int iV2;
    int iV3;
} TRIANGLE;		

typedef struct _SPHERE
{
    GLfloat xr, yr, zr;
    GLfloat fDXr, fDYr, fDZr;
} SPHERE;

SPHERE sphere;

BOOL fDisplayList = TRUE;

int iCurrentType = 0;
GLint dlLists[NLISTS];

DWORD dwTotalMillis = 0;
int iTotalTriangles = 0;

char *pszListType[NLISTS] =
{
    "Vertex Array", "Direct Triangles", "Direct Strips"
};

void DrawVertexArray(void);
void DrawTriangles(void);
void DrawStrips(void);

void (*pfnDraw[NLISTS])(void) =
{
    DrawVertexArray, DrawTriangles, DrawStrips
};

VERTEX vtxSphere[SVERTICES];
TRIANGLE triSphere[STRIANGLES];

void GenerateSphere(float fRadius, int iRings, int iSections,
                    VERTEX *pvtx, TRIANGLE *ptri)
{
    float fTheta, fPhi; 
    float fDTheta, fDPhi;
    float x, y, z, fV, fRSinTheta;
    int   i, j, n, m;               
    int   iVert, iTri;

    iTri = COMPUTE_SPHERE_TRIANGLES(iRings, iSections);
    iVert = COMPUTE_SPHERE_VERTICES(iRings, iSections);

    //	获取顶部和底部顶点信息：
    pvtx[0].x = 0.0f;
    pvtx[0].y = fRadius;
    pvtx[0].z = 0.0f;
    pvtx[0].fNx = 0.0f;
    pvtx[0].fNy = 1.0f;
    pvtx[0].fNz = 0.0f;
    pvtx[0].dwColor = RGB_COLOR(0, 0, 255);
    pvtx[iVert - 1].x = 0.0f;
    pvtx[iVert - 1].y = -fRadius;
    pvtx[iVert - 1].z = 0.0f;
    pvtx[iVert - 1].fNx = 0.0f;
    pvtx[iVert - 1].fNy = -1.0f;
    pvtx[iVert - 1].fNz = 0.0f;
    pvtx[iVert - 1].dwColor = RGB_COLOR(0, 255, 0);

    //产生顶点信息：
    fDTheta = PI / (float) (iRings + 2);
    fDPhi = 2.0f * PI / (float) iSections;
    
	n = 1;

    fTheta = fDTheta;

	//	球坐标生成法：

    for (i = 0; i <= iRings; i++)
    {
        y = (float)(fRadius * cos(fTheta)); 
        fV = fTheta / PI; 
        fRSinTheta = (float)(fRadius * sin(fTheta));
        fPhi = 0.0f;
	
        for (j = 0; j < iSections; j++)
        {
            x = (float)(fRSinTheta * sin(fPhi));
            z = (float)(fRSinTheta * cos(fPhi));
            pvtx[n].x = x;
            pvtx[n].z = z;
            pvtx[n].y = y;
            pvtx[n].fNx = x / fRadius;
            pvtx[n].fNy = y / fRadius;
            pvtx[n].fNz = z / fRadius;
            if (n & 1)
            {
                pvtx[n].dwColor = RGB_COLOR(0, 0, 255);
            }
            else
            {
                pvtx[n].dwColor = RGB_COLOR(0, 255, 0);
            }
            fPhi += fDPhi;
            n++;
        }
	
        fTheta += fDTheta;
    }

    //生成三角形信息（两极）：
    for (i = 0; i < iSections; i++)
    {
        ptri[i].iV1 = 0;
        ptri[i].iV2 = i + 1;
        ptri[i].iV3 = 1 + ((i + 1) % iSections);

        ptri[iTri - iSections + i].iV1 = iVert - 1;
        ptri[iTri - iSections + i].iV2 = iVert - 2 - i;
        ptri[iTri - iSections + i].iV3 = iVert - 2 - ((1 + i) % iSections);
    }

	//生成三角形信息（极回归线内）	
    m = 1; 
    n = iSections; 
	
    for (i = 0; i < iRings; i++)
    {
        for (j = 0; j < iSections; j++)
        {
            ptri[n].iV1 = m + j;
            ptri[n].iV2 = m + iSections + j;
            ptri[n].iV3 = m + iSections + ((j + 1) % iSections);
            ptri[n + 1].iV1 = ptri[n].iV1;
            ptri[n + 1].iV2 = ptri[n].iV3;
            ptri[n + 1].iV3 = m + ((j + 1) % iSections);
            n += 2;
        }
        m += iSections;
    }
}

void InitSphere(void)
{
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    GenerateSphere(1.0f, RINGS, SECTIONS, vtxSphere, triSphere);
    
    glVertexPointer(3, GL_FLOAT, sizeof(VERTEX), &vtxSphere[0].x);
    glNormalPointer(GL_FLOAT, sizeof(VERTEX), &vtxSphere[0].fNx);
    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(VERTEX), &vtxSphere[0].dwColor);
    
    srand(time(NULL));
    sphere.xr = 0.0f;
    sphere.yr = 0.0f;
    sphere.zr = 0.0f;
    sphere.fDXr = DROT - FRANDOM(2 * DROT);
    sphere.fDYr = DROT - FRANDOM(2 * DROT);
    sphere.fDZr = DROT - FRANDOM(2 * DROT);
}

void UpdateSphere(void)
{
    sphere.xr += sphere.fDXr;
    sphere.yr += sphere.fDYr;
    sphere.zr += sphere.fDZr;
}

void DrawVertexArray(void)
{
    glDrawElements(GL_TRIANGLES, STRIANGLES*3, GL_UNSIGNED_INT, triSphere);
}

void Vertex(int iVert)
{
    VERTEX *pvtx;

    pvtx = &vtxSphere[iVert];
    glColor3ubv((GLubyte *)&pvtx->dwColor);
    glNormal3fv(&pvtx->fNx);
    glVertex3fv(&pvtx->x);
}

void DrawTriangles(void)
{
    int iVert, *pidx;
    glBegin(GL_TRIANGLES);
    pidx = (int *)&triSphere[0];
    for (iVert = 0; iVert < STRIANGLES*3; iVert++)
    {
        Vertex(*pidx++);
    }
    glEnd();
}

void DrawStrips(void)
{
    int iIdxBase;
    int iRing, iSection;

    // Triangle fans for top and bottom caps
    glBegin(GL_TRIANGLE_FAN);
    
    Vertex(0);
    iIdxBase = 1;
    for (iSection = 0; iSection <= SECTIONS; iSection++)
    {
        Vertex(iIdxBase+(iSection % SECTIONS));
    }
    
    glEnd();

    glBegin(GL_TRIANGLE_FAN);

    Vertex(SVERTICES-1);
    iIdxBase = SVERTICES-SECTIONS-1;
    for (iSection = SECTIONS; iSection >= 0 ; iSection--)
    {
        Vertex(iIdxBase+(iSection % SECTIONS));
    }

    glEnd();

    // Triangle strips for each ring
    iIdxBase = 1;
    for (iRing = 0; iRing < RINGS; iRing++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        
        for (iSection = 0; iSection <= SECTIONS; iSection++)
        {
            Vertex(iIdxBase+(iSection % SECTIONS));
            Vertex(iIdxBase+(iSection % SECTIONS)+SECTIONS);
        }

        glEnd();

        iIdxBase += SECTIONS;
    }
}

void Init_Sphere_Data(void)
{
    float fv4[4];
    int iv1[1];
    int i;
    GLint dlBase;
    
    InitSphere();
    
    fv4[0] = 0.05f;
    fv4[1] = 0.05f;
    fv4[2] = 0.05f;
    fv4[3] = 1.0f;
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fv4);
    
    fv4[0] = 0.0f;
    fv4[1] = 1.0f;
    fv4[2] = 1.0f;
    fv4[3] = 0.0f;
    glLightfv(GL_LIGHT0, GL_POSITION, fv4);
    fv4[0] = 0.9f;
    fv4[1] = 0.9f;
    fv4[2] = 0.9f;
    fv4[3] = 1.0f;
    glLightfv(GL_LIGHT0, GL_DIFFUSE, fv4);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    
    fv4[0] = 0.6f;
    fv4[1] = 0.6f;
    fv4[2] = 0.6f;
    fv4[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, fv4);
    iv1[0] = 40;
    glMaterialiv(GL_FRONT, GL_SHININESS, iv1);
    
    glEnable(GL_CULL_FACE);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    dlBase = glGenLists(NLISTS);
    for (i = 0; i < NLISTS; i++)
    {
        dlLists[i] = dlBase+i;
        glNewList(dlLists[i], GL_COMPILE);
        pfnDraw[i]();
        glEndList();
    }
}

DWORD dwTotalMillis = 0;
int iTotalTriangles = 0;

void ResetTotals(void)
{
    dwTotalMillis = 0;
    iTotalTriangles = 0;
}

void Redraw(void)
{
    DWORD dwMillis;
    
    glClear(GL_COLOR_BUFFER_BIT);

	glRotatef(sphere.xr, 1.0f, 0.0f, 0.0f);
	glRotatef(sphere.yr, 0.0f, 1.0f, 0.0f);
	glRotatef(sphere.zr, 0.0f, 0.0f, 1.0f);

    if (fDisplayList)
    {
        glCallList(dlLists[iCurrentType]);
    }
    else
    {
        pfnDraw[iCurrentType]();
    }
    
    UpdateSphere();

    dwMillis = GetTickCount()-dwMillis;

    dwTotalMillis += dwMillis;
    iTotalTriangles += STRIANGLES;

}

