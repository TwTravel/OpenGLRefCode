//LightOff.cc
#include "LightOff.h"


TurnOff::TurnOff(Light *L)
{
  LightName=L->LightName;
}

void
TurnOff::Render()
{
  glDisable(GL_LIGHTING);
  glDisable(LightName);
}



