//Light.h
#ifndef LIGHT_H
#define LIGHT_H

#include "Node.h"

class Light: public Node
{
 public:
  Light();
  void Render();

  void SetValue(Enum PName, Enum color);
  void SetValue(Enum PName, float v1, float v2, float v3, float v4 );
  void SetValue(Enum PName, float v1, float v2, float v3);
  void SetValue(Enum PName, float f);
  void SetValuev(Enum PName, float *);
  void TurnOn();
  void TurnOff();

 private:
  static int LightNum;
  GLenum LightName;
  bool Changed[7];
  bool On;
  float Diffuse[4];
  float Specular[4];
  float Ambient[4];
  float Position[4];
  float SpotDirection[3];
  float DropOffRate;
  float CutOffAngle;

  friend class TurnOff;
};

#endif
