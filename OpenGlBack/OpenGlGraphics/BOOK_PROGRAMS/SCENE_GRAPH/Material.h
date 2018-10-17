//Material.h
#ifndef MAT_H
#define MAT_H

#include "Node.h"

class Material: public Node
{
 public:
  Material();
  void SetValuev(Enum Pname, float *);
  void SetValue(Enum Pname, float v1, float v2, float v3, float v4);
  void SetValue(Enum Pname, float Value);
  void Render();

 private:
  bool Changed[5];
  float Ambient[4];
  float Diffuse[4];
  float Specular[4];
  float Emission[4];
  float Shininess;
};

#endif
