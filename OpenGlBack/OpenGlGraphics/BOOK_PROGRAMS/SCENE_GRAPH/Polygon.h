//Polygon.h
#ifndef POLY_H
#define POLY_H

#include "Geometry.h"

class Polygon: public Geometry
{
 public:
  Polygon(){};
  void SetVerticesv(float v[][3], int);
  void Render();

 private:
  float Vertices[1000][3];
  int Size;
};

#endif
