//Camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include "Node.h"

class Camera: public Node
{
 public:
  Camera(Enum CType);

  void Render();
  void SetValuev(Enum PName, float *v);
  void SetValue(Enum PName, float v1, float v2, float v3);
  void SetValue(Enum PName, float v);
  
 private:
  Enum Type;
  bool Changed[2];
  float Position[3];
  float AimAt[3];
  float UpDirection[3];
  float Near;
  float Far;
  float Height;
  float Aspect;
  float YAngle;
  
};

#endif
