//Cube.cc
#include "Cube.h"


Cube::Cube(float w, float h, float d)
{
  Width=w;
  Height=h;
  Depth=d;
}

void
Cube::SetValue(Enum PName, float v)
{
  switch(PName)
    {
    case WIDTH:
      Width=v;
      break;
    case HEIGHT:
      Height=v;
      break;
    case DEPTH:
      Depth=v;
      break;
    default:
      break;
    }
}


void
Cube::Polygon(GLubyte *Index)
{

  float x1=(-1)*(Width/2);
  float x2=(Width/2);
  float y1=(-1)*(Height/2);
  float y2=(Height/2);
  float z1=(-1)*(Depth/2);
  float z2=(Depth/2);

  
  GLfloat V[][3]={{x1, y1, z2},{x1, y2, z2},{x2, y2, z2},{x2, y1, z2},
		  {x1, y1, z1},{x1, y2, z1},{x2, y2, z1},{x2, y1, z1}};

  glBegin(GL_POLYGON);
    glVertex3fv(V[Index[0]]);
    glVertex3fv(V[Index[1]]);
    glVertex3fv(V[Index[2]]);
    glVertex3fv(V[Index[3]]);
  glEnd();

}

void
Cube::Render()
{
  int i;
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  if(ColorNode)
    ColorNode->Render();
  if(MatNode)
    MatNode->Render();
  if(TransNode)
    TransNode->Render();
  if(StyleNode)
    StyleNode->Render();


  GLubyte CubeIndex[]={0, 3, 2, 1, 2, 3, 7, 6, 3, 0, 4, 7, 1, 2, 6, 5,
		       4, 5, 6, 7, 5, 4, 0, 1};

  GLfloat N[][3]={{0, 0, 1},{1, 0, 0},{0, -1, 0},{0, 1, 0},
		  {0, 0, -1},{-1, 0, 0}};

  //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  //glEnable(GL_COLOR_MATERIAL);

  //glutSolidSphere(1.0, 40, 40);
  for(i=0; i<6; i++)
    {
      glNormal3fv(N[i]);
      Polygon(&CubeIndex[i*4]);      
    }
  /*    
  Polygon(0, 3, 2, 1);
  Polygon(2, 3, 7, 6);
  Polygon(3, 0, 4, 7);
  Polygon(1, 2, 6, 5);
  Polygon(4, 5, 6, 7);
  Polygon(5, 4, 0, 1);
  */

  glPopAttrib();
}





