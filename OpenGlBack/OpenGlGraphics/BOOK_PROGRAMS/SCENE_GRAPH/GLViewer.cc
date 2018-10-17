//GLViewer.cc
#include "GLViewer.h"

GLViewer::GLViewer()
{
  int i;

  ViewerIndex=ViewerNum;
  ViewerNum++;

  WinName="";
  BufType[ViewerIndex]=GLUT_SINGLE;
  Root[ViewerIndex]=NULL;
  for(i=0; i<3; i++)
    BackColor[i]=0.0;
}

GLViewer::~GLViewer()
{
  if(Root[ViewerIndex])
    {
      delete Root[ViewerIndex];
      Root[ViewerIndex]=NULL;
    }
}

void
GLViewer::Init(int argc, char **argv)
{
  if(ViewerIndex==0)
    glutInit(&argc, argv);
}

void 
GLViewer::CreateWin(char *Name, int Width, int Height)
{
  WinName=Name;
  WinWidth[ViewerIndex]=Width;
  WinHeight[ViewerIndex]=Height;
}

void
GLViewer::SetValue(Enum PName, Enum Type)
{
  switch(PName)
    {
    case BUFFER:
      if(Type==DOUBLE)
	BufType[ViewerIndex]=GLUT_DOUBLE;
      else if(Type==SINGLE)
	BufType[ViewerIndex]=GLUT_SINGLE;
      break;
    case BACKCOLOR:
      Root[ViewerIndex]->GetColor(Type, BackColor);
      break;
    default:
      break;
    }
}

void
GLViewer::Show(Node *N)
{
  GLInit();
  Root[ViewerIndex]=N;
  if(ViewerIndex==(ViewerNum-1))
    glutMainLoop();
}

//private
void 
GLViewer::GLInit()
{
  glutInitDisplayMode(BufType[ViewerIndex] | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WinWidth[ViewerIndex], WinHeight[ViewerIndex]);
  glutCreateWindow(WinName);
  switch(ViewerIndex)
    {
    case 0:
      glutReshapeFunc(Reshape0);
      glutDisplayFunc(Display0);
      break;
    case 1:
      glutReshapeFunc(Reshape1);
      glutDisplayFunc(Display1);
      break;
    case 2:
      glutReshapeFunc(Reshape2);
      glutDisplayFunc(Display2);
      break;
    default:
      break;
    }

  glEnable(GL_DEPTH_TEST);
  glClearColor(BackColor[0], BackColor[1], BackColor[2], 1.0);
}

void
GLViewer::Display0()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Root[0]->Traverse();
  if(BufType[0]==GLUT_DOUBLE)
    glutSwapBuffers();
  glFlush();
}


void
GLViewer::Reshape0(int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  WinWidth[0]=w;
  WinHeight[0]=h;
}

void
GLViewer::Display1()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Root[1]->Traverse();
  if(BufType[1]==GLUT_DOUBLE)
    glutSwapBuffers();
  glFlush();
}

void
GLViewer::Reshape1(int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  WinWidth[1]=w;
  WinHeight[1]=h;
}

void
GLViewer::Display2()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Root[2]->Traverse();
  if(BufType[2]==GLUT_DOUBLE)
    glutSwapBuffers();
  glFlush();
}

void
GLViewer::Reshape2(int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  WinWidth[2]=w;
  WinHeight[2]=h;
}


