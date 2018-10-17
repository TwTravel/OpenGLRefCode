//DrawStyle.h
#ifndef DSTYLE_H
#define DSTYLE_H

#include "Node.h"

class DrawStyle: public Node
{
 public:
  DrawStyle();
  void SetValue(Enum Pname, Enum v);
  void SetValue(Enum Pname, float v);
  void Render();

 private:
  bool Changed[3];
  Enum Style;
  float PointSize;
  float LineWidth;
};

#endif
