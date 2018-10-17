//Cube.h
#ifndef CUBE_H
#define CUBE_H

#include "Geometry.h"

class Cube:public Geometry
{
 public:
  Cube(){};
  Cube(float, float, float);
  void SetValue(Enum PName, float v);
  void Render();

 private:
  //  void Polygon(int a, int b, int c, int d);
  void Polygon(GLubyte *Index);
  float Height;
  float Width;
  float Depth;

};

#endif
