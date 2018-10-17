//Scene.cc

#include "Scene.h"

#define BaseRadius 0.2
#define Radius 0.08
#define BaseLen 1
#define UpLen 0.6
#define LowLen 0.6
#define EyeRadius 0.04
#define ChairLegLen 0.55

int
main(int argc, char **argv)
{

  float v[][3]={{-0.3,-0.2,0.0},{0.3, -0.2, 0.0},
		{0.3, 0.2, 0.0},{-0.3, 0.2, 0.0} };

  //Light nodes
  Light *Light1=new Light;
  Light *Light2=new Light;
  TurnOff *Off1=new TurnOff(Light1);
  TurnOff *Off2=new TurnOff(Light2);

  //Setting Light Values :
  Light1->SetValue(POSITION, -2, -3, 1.5, 1);
  Light1->SetValue(SPOT_DIRECTION, 2, 3, -1.5);
  Light1->SetValue(CUTOFFANGLE, 40.0);
  Light1->TurnOn();

  Light2->SetValue(POSITION, 5, 5, 5, 0);
  Light2->SetValue(SPECULAR, 1.0, 1.0, 1.0, 1.0);
  Light2->SetValue(DIFFUSE, 1.0, 1.0, 1.0, 1.0);
  Light2->TurnOn();


  //Nodes for Camera:
  Camera *Camera1=new Camera(PERSPECTIVE);

  Camera1->SetValue(POSITION, 2.2, 0.9, 3);
  Camera1->SetValue(AIMAT, 0, 0, 0);
  Camera1->SetValue(UPDIRECTION, 0, 1, 0);
  Camera1->SetValue(ASPECT, 1);
  Camera1->SetValue(NEAR, 1);
  Camera1->SetValue(FAR, 20);
  Camera1->SetValue(YANGLE, 50);
  
  //Nodes for Robot:
  Material *RobotMat=new Material;
  Material *EyeMat=new Material;

  Cylinder *Base=new Cylinder;
  Sphere *Head=new Sphere;
  Sphere *EyeL=new Sphere;
  Sphere *EyeR=new Sphere;
  Cylinder *UpperArmL=new Cylinder;
  Cylinder *UpperArmR=new Cylinder;
  Cylinder *LowerArmL=new Cylinder;
  Cylinder *LowerArmR=new Cylinder;
  Cylinder *UpperLegL=new Cylinder;
  Cylinder *UpperLegR=new Cylinder;
  Cylinder *LowerLegL=new Cylinder;
  Cylinder *LowerLegR=new Cylinder;
  Polygon *Paper=new Polygon;

  Transformation *EyeLTrans=new Transformation;
  Transformation *EyeRTrans=new Transformation;
  Transformation *HeadTrans=new Transformation;
  Transformation *UpArmLTrans=new Transformation;
  Transformation *UpArmRTrans=new Transformation;
  Transformation *LowArmTrans=new Transformation;
  Transformation *UpLegLTrans=new Transformation;
  Transformation *UpLegRTrans=new Transformation;
  Transformation *LowLegTrans=new Transformation;
  Transformation *BaseTrans=new Transformation;

  //Robot Value:
  RobotMat->SetValue(DIFFUSE, 0.0, 0.0, 1.0, 1.0);
  RobotMat->SetValue(AMBIENT, 0.0, 0.0, 1.0, 1.0);
  RobotMat->SetValue(SPECULAR, 1.0, 1.0, 1.0, 1.0);
  RobotMat->SetValue(SHININESS, 100.0);

  EyeMat->SetValue(DIFFUSE, 1.0, 1.0, 1.0, 1.0);
  EyeMat->SetValue(AMBIENT, 1.0, 1.0, 1.0, 1.0);
  EyeMat->SetValue(SPECULAR, 1.0, 1.0, 1.0, 1.0);
  EyeMat->SetValue(SHININESS, 100.0);

  Base->SetValue(HEIGHT, BaseLen);
  Base->SetValue(RADIUS, BaseRadius);
  Head->SetValue(RADIUS, BaseRadius);
  EyeL->SetValue(RADIUS, EyeRadius);
  EyeR->SetValue(RADIUS, EyeRadius);
  UpperArmL->SetValue(HEIGHT, UpLen);
  UpperArmL->SetValue(RADIUS, Radius);
  LowerArmL->SetValue(HEIGHT, LowLen);
  LowerArmL->SetValue(RADIUS, Radius);
  UpperArmR->SetValue(HEIGHT, UpLen);
  UpperArmR->SetValue(RADIUS, Radius);
  LowerArmR->SetValue(HEIGHT, LowLen);
  LowerArmR->SetValue(RADIUS, Radius);
  UpperLegL->SetValue(HEIGHT, UpLen);
  UpperLegL->SetValue(RADIUS, Radius);
  LowerLegL->SetValue(HEIGHT, LowLen);
  LowerLegL->SetValue(RADIUS, Radius);
  UpperLegR->SetValue(HEIGHT, UpLen);
  UpperLegR->SetValue(RADIUS, Radius);
  LowerLegR->SetValue(HEIGHT, LowLen);
  LowerLegR->SetValue(RADIUS, Radius);

  Paper->SetVerticesv(v, 4);
  Paper->SetMaterial(EyeMat);

  EyeLTrans->SetValue(TRANSLATION, BaseRadius-EyeRadius/2, 0, 0, 0);
  EyeLTrans->SetValue(ROTATION, 30, 0, 0, 1, 1);
  EyeRTrans->SetValue(TRANSLATION, BaseRadius-EyeRadius/2, 0, 0, 0);
  EyeRTrans->SetValue(ROTATION, -30, 0, 0, 1, 1);
  HeadTrans->SetValue(TRANSLATION, 0, 0, BaseLen+BaseRadius+BaseRadius/3, 0);
  UpArmLTrans->SetValue(TRANSLATION, 0, 0, -UpLen, 0);
  UpArmLTrans->SetValue(ROTATION, -45, 0, 1, 0, 1);
  UpArmLTrans->SetValue(TRANSLATION, 0, BaseRadius+Radius, BaseLen, 2);
  UpArmRTrans->SetValue(TRANSLATION, 0, 0, -UpLen, 0);
  UpArmRTrans->SetValue(ROTATION, -45, 0, 1, 0, 1);
  UpArmRTrans->SetValue(TRANSLATION, 0, -(BaseRadius+Radius), BaseLen, 2);
  LowArmTrans->SetValue(TRANSLATION, 0, 0, -UpLen, 0);
  LowArmTrans->SetValue(ROTATION, -45, 0, 1, 0, 1);
  UpLegLTrans->SetValue(TRANSLATION, 0, 0, -UpLen, 0);
  UpLegLTrans->SetValue(ROTATION, -90, 0, 1, 0, 1);
  UpLegLTrans->SetValue(TRANSLATION, 0, BaseRadius+Radius, 0, 2);
  UpLegRTrans->SetValue(TRANSLATION, 0, 0, -UpLen, 0);
  UpLegRTrans->SetValue(ROTATION, -100, 0, 1, 0, 1);
  UpLegRTrans->SetValue(TRANSLATION, 0, -(BaseRadius+Radius), 0, 2);
  LowLegTrans->SetValue(TRANSLATION, 0, 0, -UpLen, 0);
  LowLegTrans->SetValue(ROTATION, 95, 0, 1, 0, 1);
  BaseTrans->SetValue(ROTATION, -90, 1, 0, 0, 0);
  BaseTrans->SetValue(ROTATION, -10, 0, 0, 1, 1);

  Head->SetTransform(HeadTrans);
  EyeL->SetTransform(EyeLTrans);
  EyeR->SetTransform(EyeRTrans);
  EyeR->SetMaterial(EyeMat);
  EyeL->SetMaterial(EyeMat);
  UpperArmL->SetTransform(UpArmLTrans);
  LowerArmL->SetTransform(LowArmTrans);
  UpperArmR->SetTransform(UpArmRTrans);
  LowerArmR->SetTransform(LowArmTrans);
  UpperLegL->SetTransform(UpLegLTrans);
  LowerLegL->SetTransform(LowLegTrans);
  UpperLegR->SetTransform(UpLegRTrans);
  LowerLegR->SetTransform(LowLegTrans);
  Base->SetTransform(BaseTrans);

  Paper->SetTransform(ROTATION, 20, 0, 1, 0, 0);
  Paper->SetTransform(TRANSLATION, 0.05, -0.15, -0.2, 1);

  //Set Relationship in Robot:
  RobotMat->AddChild(Light1);
  Light1->AddChild(Base);

  Base->AddChild(Off1);
  Off1->AddChild(Light2);
  Light2->AddChild(Head);
  Head->AddChild(Off2);
  Head->AddChild(EyeL);
  Head->AddChild(EyeR);

  Base->AddChild(UpperArmL);
  UpperArmL->AddChild(LowerArmL);
  Base->AddChild(UpperArmR);
  UpperArmR->AddChild(LowerArmR);
  Base->AddChild(UpperLegL);
  UpperLegL->AddChild(LowerLegL);
  Base->AddChild(UpperLegR);
  UpperLegR->AddChild(LowerLegR);
  LowerArmR->AddChild(Paper);

  //Nodes for Chair:
  Cube *Seat=new Cube(0.7, 0.06, 0.7);
  Line *Leg1=new Line;
  Line *Leg2=new Line;
  Line *Leg3=new Line;
  Line *Leg4=new Line;
  DrawStyle *LegStyle=new DrawStyle;
  Color *ChairColor=new Color;
  
  //Chair Value:
  float v1[][3]={{-0.3, 0, 0.3},{-0.35, -1*ChairLegLen, 0.3}};
  float v2[][3]={{0.3, 0, 0.3},{0.35, -1*ChairLegLen, 0.3}};
  float v3[][3]={{0.3, 0, -0.3},{0.35, -1*ChairLegLen, -0.3}};
  float v4[][3]={{-0.3, 0, -0.3},{-0.35, -1*ChairLegLen, -0.3}};

  ChairColor->SetValue(RED);
  Seat->SetTransform(TRANSLATION, 0, -0.15, 0, 0);
  LegStyle->SetValue(LINEWIDTH, 7);
  Leg1->SetStyle(LegStyle);
  Leg1->SetVerticesv(v1);
  Leg2->SetStyle(LegStyle);
  Leg2->SetVerticesv(v2);
  Leg3->SetStyle(LegStyle);
  Leg3->SetVerticesv(v3);
  Leg4->SetStyle(LegStyle);
  Leg4->SetVerticesv(v4);
  Seat->AddChild(Leg1);
  Seat->AddChild(Leg2);
  Seat->AddChild(Leg3);
  Seat->AddChild(Leg4);
  ChairColor->AddChild(Seat);

  //Transformation Nodes for both Robot and Chair:
  Transformation *Trans1=new Transformation;
 
  Trans1->SetValue(TRANSLATION, -0.5, 0, 0, 2);

  Trans1->AddChild(ChairColor);
  Trans1->AddChild(RobotMat);

  //Root Node:
  Node *Root=new Node;

  Root->AddChild(Trans1);
  Root->AddChild(Camera1);
  
  //Viewer:
  GLViewer *MyViewer=new GLViewer;

  MyViewer->Init(argc, argv);
  MyViewer->SetValue(BACKCOLOR, GREY);
  MyViewer->SetValue(BUFFER, DOUBLE);
  MyViewer->CreateWin("Working Hard", 500, 500);

  GLViewer *MyViewer2=new GLViewer;
  
  MyViewer2->Init(argc, argv);
  MyViewer2->SetValue(BACKCOLOR, MAGENTA);
  MyViewer2->CreateWin("Working Hard2", 200, 200);
  MyViewer2->SetValue(BUFFER, DOUBLE);

  MyViewer->Show(Root);
  MyViewer2->Show(Root);
  return 0;
}


