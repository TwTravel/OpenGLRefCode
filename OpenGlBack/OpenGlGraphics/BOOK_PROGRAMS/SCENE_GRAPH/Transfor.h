//Transformation.h
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Node.h"

class Transformation: public Node
{
 public:
  Transformation();
  void SetValuev(Enum Pname, float *, int Order);
  void SetValue(Enum Pname, float x, float y, float z, int Order);
  void SetValue(Enum Pname, float a, float x, float y, float z, int Order);
  void Render();

 private:
  int OpNum;
  int TransformOrder[10];
  float Transform[10][4];
  //float Translation[10][3];
  //float Rotation[10][4];
  //float Scale[10][3];

};

#endif
