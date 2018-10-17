//Geometry.cc
#include "Geometry.h"

Geometry::Geometry()
{
  ColorNode=NULL;
  MatNode=NULL;
  TransNode=NULL;
  StyleNode=NULL;
}

Geometry::~Geometry()
{
  if(ColorNode)
    {
      delete ColorNode;
      ColorNode=NULL;
    }
  if(MatNode)
    {
      delete MatNode;
      MatNode=NULL;
    }
  if(TransNode)
    {
      delete TransNode;
      TransNode=NULL;
    }
  if(StyleNode)
    {
      delete StyleNode;
      StyleNode=NULL;
    }
}

void 
Geometry::SetColor(Enum C)
{
  if(ColorNode==NULL)
    ColorNode=new Color;

  ColorNode->SetValue(C);
}

void 
Geometry::SetColor(float v1, float v2, float v3)
{
  if(ColorNode==NULL)
    ColorNode=new Color;

  ColorNode->SetValue(v1, v2, v3);
}

void 
Geometry::SetColorv(float *v)
{
  if(ColorNode==NULL)
    ColorNode=new Color;

  ColorNode->SetValuev(v);
}

void 
Geometry::SetColor(Color *C)
{
  if(ColorNode!=NULL)
    delete ColorNode;

  ColorNode=C;
}

void 
Geometry::SetMaterial(Enum PName, float v1, float v2, float v3, float v4)
{
  if(MatNode==NULL)
    MatNode=new Material;

  MatNode->SetValue(PName, v1, v2, v3, v4);
}

void 
Geometry::SetMaterialv(Enum PName, float *v)
{
  if(MatNode==NULL)
    MatNode=new Material;

  MatNode->SetValuev(PName, v);
}

void 
Geometry::SetMaterial(Enum PName, float v)
{
  if(MatNode==NULL)
    MatNode=new Material;

  MatNode->SetValue(PName, v);
}

void 
Geometry::SetMaterial(Material *M)
{
  if(MatNode!=NULL)
    delete MatNode;

  MatNode=M;
}

void 
Geometry::SetTransform(Enum Pname, float *v, int Order)
{
  if(TransNode==NULL)
    TransNode=new Transformation;

  TransNode->SetValuev(Pname, v, Order);
}

void 
Geometry::SetTransform(Enum Pname, float x, float y, float z, int Order)
{
  if(TransNode==NULL)
    TransNode=new Transformation;

  TransNode->SetValue(Pname, x, y, z, Order);
}

void 
Geometry::SetTransform(Enum Pname, float a, float x, float y, float z, 
		       int Order)
{
  if(TransNode==NULL)
    TransNode=new Transformation;

  TransNode->SetValue(Pname, a, x, y, z, Order);
}


void 
Geometry::SetTransform(Transformation *T)
{
  if(TransNode!=NULL)
    delete TransNode;

  TransNode=T;
}

void
Geometry::SetStyle(Enum Pname, Enum v)
{
  if(StyleNode==NULL)
    StyleNode=new DrawStyle;

  StyleNode->SetValue(Pname, v);  
}

void
Geometry::SetStyle(Enum Pname, float v)
{
  if(StyleNode==NULL)
    StyleNode=new DrawStyle;

  StyleNode->SetValue(Pname, v);
}

void
Geometry::SetStyle(DrawStyle *S)
{
  if(StyleNode==NULL)
    StyleNode=new DrawStyle;

  StyleNode=S;  
}

void 
Geometry::Render()
{
}


