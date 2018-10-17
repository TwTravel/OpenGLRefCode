//Light.cc
#include "Light.h"

Light::Light()
{
  int i;

  for(i=0; i<7; i++)
    Changed[i]=false;

  On=false;

  switch(LightNum)
    {
    case 0:
      LightName=GL_LIGHT0;
      break;
    case 1:
      LightName=GL_LIGHT1;
      break;
    case 2:
      LightName=GL_LIGHT2;
      break;
    case 3:
      LightName=GL_LIGHT3;
      break;
    case 4:
      LightName=GL_LIGHT4;
      break;
    case 5:
      LightName=GL_LIGHT5;
      break;
    case 6:
      LightName=GL_LIGHT6;
      break;
    case 7:
      LightName=GL_LIGHT7;
      break;
    default:
      break;
    }
  LightNum++;
}

void 
Light::TurnOn()
{
  On=true;
}

void 
Light::TurnOff()
{
  On=false;
}

void 
Light::SetValuev(Enum pname, float *Value)
{
  int i;
  float *temp=NULL;

  switch(pname)
    {
    case AMBIENT:
      Changed[0]=true;
      temp=Ambient;
      break;
    case DIFFUSE:
      Changed[1]=true;
      temp=Diffuse;
      break;
    case SPECULAR:
      Changed[2]=true;
      temp=Specular;
      break;
    case POSITION:
      Changed[3]=true;
      temp=Position;
      break;
    case SPOT_DIRECTION:
      Changed[4]=true;
      temp=SpotDirection;
      break;
    default:
      break;
    }
  for(i=0; i<3; i++)
    temp[i]=Value[i];

  if(pname!=SPOT_DIRECTION)
      temp[3]=Value[3];
}

void 
Light::SetValue(Enum pname, float v1, float v2, float v3, float v4)
{
  float temp[]={v1, v2, v3, v4};
  SetValuev(pname, temp);
}

void 
Light::SetValue(Enum pname, float v1, float v2, float v3)
{
  float temp[]={v1, v2, v3};
  SetValuev(pname, temp);
}

void 
Light::SetValue(Enum pname, float f)
{
  switch(pname)
    {
    case DROPOFFRATE:
      Changed[5]=true;
      DropOffRate=f;
      break;
    case CUTOFFANGLE:
      Changed[6]=true;
      CutOffAngle=f;
      break;
    default:
      break;
    }
}

void 
Light::Render()
{
  if(On)
    {
      glEnable(GL_LIGHTING);
      glEnable(LightName);
    }
  else
    {
      glDisable(GL_LIGHTING);
      glDisable(LightName);
    }

  if(Changed[0])
    glLightfv(LightName, GL_AMBIENT, Ambient);
  if(Changed[1])
    glLightfv(LightName, GL_DIFFUSE, Diffuse);
  if(Changed[2])
    glLightfv(LightName, GL_SPECULAR, Specular);
  if(Changed[3])
    glLightfv(LightName, GL_POSITION, Position);
  if(Changed[4])
    glLightfv(LightName, GL_SPOT_DIRECTION, SpotDirection);
  if(Changed[6])
    glLightf(LightName, GL_SPOT_CUTOFF, CutOffAngle);
}




