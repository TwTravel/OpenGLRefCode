//Node.cc
#include "Node.h"

void 
Node::Traverse()
{
  if(!KeepMatrix)
    glPushMatrix();
 
  Render();
  if(LeftChild!=NULL)
    LeftChild->Traverse();
  if(!KeepMatrix)
    glPopMatrix();
  if(RightSibling!=NULL)
    RightSibling->Traverse();
}

void
Node::AddChild(Node *node)
{
  if(LeftChild==NULL)
    LeftChild=node;
  else
    {
      node->RightSibling=LeftChild;
      LeftChild=node;
    }
}

Node::Node()
{
  KeepMatrix=false;
  LeftChild=NULL;
  RightSibling=NULL;
}

Node::~Node()
{
  if(LeftChild)
    delete LeftChild;
  LeftChild=NULL;
  if(RightSibling)
    delete RightSibling;
  RightSibling=NULL;
}

void
Node::GetColor(Enum Color, float *C)
{
  int i;

  switch(Color)
    {
    case BLACK:
      for(i=0; i<3; i++)
	C[i]=0.0;
      break;
    case WHITE:
      for(i=0; i<3; i++)
	C[i]=1.0;
      break;
    case RED:
      C[0]=1.0;
      C[1]=0.0;
      C[2]=0.0;
      break;
    case GREEN:
      C[0]=0.0;
      C[1]=1.0;
      C[2]=0.0;
      break;
    case YELLOW:
      C[0]=1.0;
      C[1]=1.0;
      C[2]=0.0;
      break;
    case BLUE:
      C[0]=0.0;
      C[1]=0.0;
      C[2]=1.0;
      break;
    case MAGENTA:
      C[0]=1.0;
      C[1]=0.0;
      C[2]=1.0;
      break;
    case CYAN:
      C[0]=0.0;
      C[1]=1.0;
      C[2]=1.0;
      break;
    case GREY:
      C[0]=0.5;
      C[1]=0.5;
      C[2]=0.5;
      break;

    default:
      break;
    }
}


void
Node::Render()
{
}

