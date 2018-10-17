//Material.cc
#include "Material.h"

Material::Material()
{
  int i;

  for(i=0; i<5; i++)
    {
      Changed[i]=false;
    }
}

void 
Material::SetValuev(Enum Pname, float Value[])
{
  float *temp=NULL;
  int i;

  switch(Pname)
    {
    case AMBIENT:
      temp=Ambient;
      Changed[0]=true;
      break;
    case DIFFUSE:
      temp=Diffuse;
      Changed[1]=true;
      break;
    case SPECULAR:
      temp=Specular;
      Changed[2]=true;
      break;
    case EMISSION:
      temp=Emission;
      Changed[3]=true;
      break;
    default:
      break;
    }
  
  for(i=0; i<4; i++)
    temp[i]=Value[i];
}

void 
Material::SetValue(Enum Pname, float v1, float v2, float v3, float v4)
{
  float temp[]={v1, v2, v3, v4};
  
  SetValuev(Pname, temp);
}

void
Material::SetValue(Enum Pname, float Value)
{
  if(Pname==SHININESS)
    {
      Changed[4]=true;
      Shininess=Value;
    }
}

void
Material::Render()
{
  if(Changed[0])
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Ambient);
  if(Changed[1])
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Diffuse);
  if(Changed[2])
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Specular);
  if(Changed[3])
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Emission);
  if(Changed[4])
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, Shininess);
}



