//Sphere.cc

#include "Sphere.h"

Sphere::Sphere(float R)
{
  Radius=R;
}

void
Sphere::SetValue(Enum Pname, float v)
{
  if(Pname==RADIUS)
    Radius=v;
}

void
Sphere::Render()
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
  

  glutSolidSphere(Radius, 40, 40);

  glPopAttrib();
}
