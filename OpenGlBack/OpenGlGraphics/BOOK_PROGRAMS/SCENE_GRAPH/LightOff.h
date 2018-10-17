//LightOff.h
#ifndef LIGHTOFF_H
#define LIGHTOFF_H

#include "Light.h"

class TurnOff: public Node
{
 public:
  TurnOff(Light *L);
  void Render();

 private:
  GLenum LightName;
  
};

#endif
