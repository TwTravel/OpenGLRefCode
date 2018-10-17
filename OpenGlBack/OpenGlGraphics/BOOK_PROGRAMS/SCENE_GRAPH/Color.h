//Color.h

#ifndef COLOR_H
#define COLOR_H

#include "Node.h"

class Color: public Node
{
 public:
  Color();
  void SetValuev(float *v);
  void SetValue(float v1, float v2, float v3);
  void SetValue(Enum Type);
  void Render();

 private:
  bool Changed;
  float Color3f[3];
};

#endif
