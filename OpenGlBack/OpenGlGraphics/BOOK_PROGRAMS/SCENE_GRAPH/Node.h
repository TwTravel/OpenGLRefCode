//Node.h 
#include <GL/glut.h>
#include <iostream>

#ifndef NODE_H
#define NODE_H

enum Enum
{
  PERSPECTIVE, ORTHO, POSITION, AIMAT, UPDIRECTION, ASPECT,
  NEAR, FAR, YANGLE,
  BLACK, WHITE, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, GREY,
  WIDTH, HEIGHT,DEPTH,
  AMBIENT, DIFFUSE, SPECULAR, SPOT_DIRECTION, DROPOFFRATE,
  CUTOFFANGLE, EMISSION, SHININESS,
  TRANSLATION, ROTATION, SCALE,
  BUFFER, SINGLE, DOUBLE,
  RADIUS,
  STYLE, POINTSIZE, LINEWIDTH, FILLED, LINE, POINT,
  BACKCOLOR
};

class Node
{
 public:
  Node();
  virtual ~Node();
  virtual void Render();
  void AddChild(Node *);

 protected:
  void GetColor(Enum, float *);
  bool KeepMatrix; 

 private:
  void Traverse();
  Node *LeftChild;
  Node *RightSibling;

  friend class GLViewer;
};


#endif  
