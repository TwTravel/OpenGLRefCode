//DrawStyle.cc
#include "DrawStyle.h"


DrawStyle::DrawStyle()
{
  int i;
  for(i=0; i<3; i++)
    {
      Changed[i]=false;
    }
}


void
DrawStyle::SetValue(Enum Pname, Enum v)
{
  switch(Pname)
    {
    case STYLE:
      Changed[0]=true;
      Style=v;
      break;
    default:
      break;
    }
}


void
DrawStyle::SetValue(Enum Pname, float v)
{
  switch(Pname)
    {
    case POINTSIZE:
      Changed[1]=true;
      PointSize=v;
      break;
    case LINEWIDTH:
      Changed[2]=true;
      LineWidth=v;
      break;
    default:
      break;
    }
}

void
DrawStyle::Render()
{
  GLenum GlStyle=GL_FILL;

  if(Changed[0])
    {
      switch(Style)
	{
	case FILLED:
	  GlStyle=GL_FILL;
	  break;
	case LINE:
	  GlStyle=GL_LINE;
	  break;
	case POINT:
	  GlStyle=GL_POINT;
	  break;
	default:
	  break;
	}
      glPolygonMode(GL_FRONT_AND_BACK, GlStyle);
    }
  if(Changed[1])
    glPointSize(PointSize);
  if(Changed[2])
    glLineWidth(LineWidth);
}


