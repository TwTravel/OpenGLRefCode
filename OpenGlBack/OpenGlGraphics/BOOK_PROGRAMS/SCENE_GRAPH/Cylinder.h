//Cylinder.h

#ifndef CYLIN_H
#define CYLIN_H

#include "Geometry.h"

class Cylinder: public Geometry
{
 public:
  Cylinder(){};
  void SetValue(Enum, float);
  void Render();

 private:
  float Height;
  float Radius;
};

#endif

