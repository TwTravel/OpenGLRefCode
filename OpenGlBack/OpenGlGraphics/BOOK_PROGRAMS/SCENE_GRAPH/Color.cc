//Color.cc
#include "Color.h"

Color::Color()
{
}

void 
Color::SetValue(Enum Type)
{
  Changed=true;
  GetColor(Type, Color3f);
}

void
Color::SetValuev(float *v)
{
  int i;

  Changed=true;
  for(i=0; i<3; i++)
    Color3f[i]=v[i];
}

void
Color::SetValue(float v1, float v2, float v3)
{
  Changed=true;
  Color3f[0]=v1;
  Color3f[1]=v2;
  Color3f[2]=v3;
}

void 
Color::Render()
{
  if(Changed)
    glColor3fv(Color3f);
}


