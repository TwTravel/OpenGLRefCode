//Transformation.cc
#include "Transformation.h"

Transformation::Transformation()
{
  int i;

  OpNum=0;
  for(i=0; i<10; i++)
    TransformOrder[i]=-1;
}

void 
Transformation::SetValuev(Enum Pname, float *Value, int Order)
{
  //float *temp=NULL;
  int i;

  if(OpNum<Order+1)
    OpNum=Order+1;

  switch(Pname)
    {
    case TRANSLATION:
      TransformOrder[Order]=TRANSLATION;
      //temp=Translation;
      break;
    case ROTATION:
      TransformOrder[Order]=ROTATION;
      //temp=Rotation;
      break;
    case SCALE:
      TransformOrder[Order]=SCALE;  
      //temp=Scale;
      break;
    default:
      break;
    }

  for(i=0; i<3; i++)
    Transform[Order][i]=Value[i];
  if(Pname==ROTATION)
    Transform[Order][3]=Value[3];
}

void
Transformation::SetValue(Enum Pname, float x, float y, float z, int Order)
{
  float temp[]={x, y, z};

  SetValuev(Pname, temp, Order);
}

void 
Transformation::SetValue(Enum Pname, float a, float x, float y, float z, 
			 int Order)
{
  float temp[]={a, x, y, z};

  SetValuev(Pname, temp, Order);
}

void
Transformation::Render()
{
  int i;

  for(i=OpNum-1; i>=0; i--)
    {
      switch(TransformOrder[i])
	{
	case TRANSLATION:
	  glTranslatef(Transform[i][0], Transform[i][1], Transform[i][2]);
	  break;
	case ROTATION:
	  glRotatef(Transform[i][0], Transform[i][1], Transform[i][2], 
		    Transform[i][3]);
	  break;
	case SCALE:
	  glScalef(Transform[i][0], Transform[i][1], Transform[i][2]);
	  break;
	default:
	  break;
	}
    }
}
