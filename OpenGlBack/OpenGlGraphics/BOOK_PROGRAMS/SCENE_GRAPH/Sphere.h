//Sphere.h
#include "Geometry.h"

class Sphere: public Geometry
{
 public:
  Sphere(){};
  Sphere(float R);
  void SetValue(Enum Pname, float v);
  void Render();

 private:
  float Radius;
};
