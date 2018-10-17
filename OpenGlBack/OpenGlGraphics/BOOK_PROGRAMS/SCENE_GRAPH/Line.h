//Line.h
#ifndef LINE_H
#define LINE_H

#include "Geometry.h"

class Line: public Geometry
{
 public:
  Line(){};
  void SetVertices(float *, float *);
  void SetVerticesv(float v[][3]);
  void Render();

 private:
  float Vertices[2][3];
};

#endif
