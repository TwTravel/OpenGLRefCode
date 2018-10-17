//Cylinder.cc

#include "Cylinder.h"

void
Cylinder::SetValue(Enum Pname, float v)
{
  switch(Pname)
    {
    case HEIGHT:
      Height=v;
      break;
    case RADIUS:
      Radius=v;
      break;
    default:
      break;
    }
}

void
Cylinder::Render()
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

  GLUquadricObj *CylinderObj;
  CylinderObj=gluNewQuadric();
  gluCylinder(CylinderObj, Radius, Radius, Height, 30, 30);
   
  glPopAttrib();
}
