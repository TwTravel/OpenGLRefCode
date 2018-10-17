//Line.cc
#include "Line.h"

void
Line::SetVertices(float *v1, float *v2)
{
  int i;
  for(i=0; i<3; i++)
    {
      Vertices[0][i]=v1[i];
      Vertices[1][i]=v2[i];
    }
}

void
Line::SetVerticesv(float v[][3])
{
  SetVertices(v[0], v[1]);
}

void
Line::Render()
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  if(ColorNode)
    ColorNode->Render();
  if(MatNode)
    MatNode->Render();
  if(TransNode)
    TransNode->Render();
  if(StyleNode)
    StyleNode->Render();

  glBegin(GL_LINES);
    glVertex3fv(Vertices[0]);
    glVertex3fv(Vertices[1]);
  glEnd();  
    
  glPopAttrib();
  
}
