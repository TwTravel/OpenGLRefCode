//Polygon.cc
#include "Polygon.h"

void
Polygon::SetVerticesv(float v[][3], int S)
{
  int i, j;
  Size=S;

  for(i=0; i<S; i++)
    for(j=0; j<3; j++)
      Vertices[i][j]=v[i][j];
}

void
Polygon::Render()
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

  glBegin(GL_POLYGON);
  for(i=0; i<Size; i++)
    glVertex3fv(Vertices[i]);
  glEnd();  
  
  glPopAttrib();
  
}
