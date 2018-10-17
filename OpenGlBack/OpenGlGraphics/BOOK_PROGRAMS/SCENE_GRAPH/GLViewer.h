//GLViewer.h
#ifndef GLVIEWER_H
#define GLVIEWER_H

#include "Node.h"

class GLViewer
{
 public:
  GLViewer();
  ~GLViewer();

  void CreateWin(char *Name, int Width, int Height);

  //set buffer, backcolor
  void SetValue(Enum PName, Enum Type);
  void Init(int argc, char **argv);
  void Show(Node *N);

 private:
  void GLInit();
  int ViewerIndex;
  char *WinName;
  float BackColor[3];

  static Node *Root[3];
  static int ViewerNum;
  static int BufType[3];
  static int WinWidth[3];
  static int WinHeight[3];
  static void Reshape0(int w, int h);
  static void Display0();
  static void Reshape1(int w, int h);
  static void Display1();
  static void Reshape2(int w, int h);
  static void Display2();
  
};

#endif
