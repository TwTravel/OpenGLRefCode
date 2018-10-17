//Geometry.h
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Node.h"
#include "Color.h"
#include "Material.h"
#include "Transformation.h"
#include "DrawStyle.h"

class Geometry: public Node
{
 public:
  Geometry();
  ~Geometry();

  void SetColor(Enum);
  void SetColor(float, float, float);
  void SetColorv(float *);
  void SetColor(Color *);
  void SetMaterial(Enum, float, float, float, float);
  void SetMaterialv(Enum, float *);
  void SetMaterial(Enum, float);
  void SetMaterial(Material *);
  void SetTransform(Enum, float *, int);
  void SetTransform(Enum, float, float, float, int);
  void SetTransform(Enum, float, float, float, float, int);
  void SetTransform(Transformation *);
  void SetStyle(Enum, Enum);
  void SetStyle(Enum, float);
  void SetStyle(DrawStyle *);
  virtual void Render();

 protected:
  Color *ColorNode;
  Material *MatNode;
  Transformation *TransNode;
  DrawStyle *StyleNode;
};

#endif
