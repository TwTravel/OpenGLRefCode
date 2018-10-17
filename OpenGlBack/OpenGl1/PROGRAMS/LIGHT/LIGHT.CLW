; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CLighting
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Light.h"
LastPage=0

ClassCount=7
Class1=CLightApp
Class2=CLightDoc
Class3=CLightView
Class4=CMainFrame

ResourceCount=6
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class5=CAboutDlg
Resource3=IDD_LIGHT
Resource4=IDD_ABOUTBOX (English (U.S.))
Class6=CLightMaterial
Resource5=IDD_COLOR_MATERIAL
Class7=CLighting
Resource6=IDR_MAINFRAME (English (U.S.))

[CLS:CLightApp]
Type=0
HeaderFile=Light.h
ImplementationFile=Light.cpp
Filter=N

[CLS:CLightDoc]
Type=0
HeaderFile=LightDoc.h
ImplementationFile=LightDoc.cpp
Filter=N

[CLS:CLightView]
Type=0
HeaderFile=LightView.h
ImplementationFile=LightView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=CLightView

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CFrameWnd
VirtualFilter=fWC



[CLS:CAboutDlg]
Type=0
HeaderFile=Light.cpp
ImplementationFile=Light.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command10=ID_EDIT_PASTE
Command11=ID_APP_ABOUT
CommandCount=11
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_EDIT_COPY

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
CommandCount=13
Command4=ID_EDIT_UNDO
Command13=ID_PREV_PANE


[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_APP_EXIT
Command2=IDM_CUBE
Command3=IDM_SPHERE
Command4=IDM_SMOOTH_CUBE
Command5=IDM_COLOR
Command6=IDM_LIGHT
Command7=ID_APP_ABOUT
CommandCount=7

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[CLS:CLightMaterial]
Type=0
HeaderFile=LightMaterial.h
ImplementationFile=LightMaterial.cpp
BaseClass=CDialog
Filter=D
LastObject=CLightMaterial
VirtualFilter=dWC

[DLG:IDD_LIGHT]
Type=1
Class=CLighting
ControlCount=60
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_L_AMBIENT_R,edit,1350631552
Control5=IDC_L_AMBIENT_G,edit,1350631552
Control6=IDC_L_AMBIENT_B,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_L_DIFFUSE_R,edit,1350631552
Control12=IDC_L_DIFFUSE_G,edit,1350631552
Control13=IDC_L_DIFFUSE_B,edit,1350631552
Control14=IDC_STATIC,static,1342308352
Control15=IDC_L_SPECULAR_R,edit,1350631552
Control16=IDC_L_SPECULAR_G,edit,1350631552
Control17=IDC_L_SPECULAR_B,edit,1350631552
Control18=IDC_STATIC,static,1342308352
Control19=IDC_L_POSITION_X,edit,1350631552
Control20=IDC_L_POSITION_Y,edit,1350631552
Control21=IDC_L_POSITION_Z,edit,1350631552
Control22=IDC_STATIC,static,1342308352
Control23=IDC_STATIC,static,1342308352
Control24=IDC_STATIC,static,1342308352
Control25=IDC_STATIC,static,1342308352
Control26=IDC_STATIC,button,1342177287
Control27=IDC_S_AMBIENT_R,edit,1350631552
Control28=IDC_S_AMBIENT_G,edit,1350631552
Control29=IDC_S_AMBIENT_B,edit,1350631552
Control30=IDC_STATIC,static,1342308352
Control31=IDC_STATIC,static,1342308352
Control32=IDC_STATIC,static,1342308352
Control33=IDC_STATIC,static,1342308352
Control34=IDC_S_DIFFUSE_R,edit,1350631552
Control35=IDC_S_DIFFUSE_G,edit,1350631552
Control36=IDC_S_DIFFUSE_B,edit,1350631552
Control37=IDC_STATIC,static,1342308352
Control38=IDC_S_SPECULAR_R,edit,1350631552
Control39=IDC_S_SPECULAR_G,edit,1350631552
Control40=IDC_S_SPECULAR_B,edit,1350631552
Control41=IDC_STATIC,static,1342308352
Control42=IDC_S_POSITION_X,edit,1350631552
Control43=IDC_S_POSITION_Y,edit,1350631552
Control44=IDC_S_POSITION_Z,edit,1350631552
Control45=IDC_STATIC,static,1342308352
Control46=IDC_STATIC,static,1342308352
Control47=IDC_STATIC,static,1342308352
Control48=IDC_STATIC,static,1342308352
Control49=IDC_STATIC,static,1342308352
Control50=IDC_STATIC,static,1342308352
Control51=IDC_STATIC,static,1342308352
Control52=IDC_STATIC,static,1342308352
Control53=IDC_S_DIECTION_X,edit,1350631552
Control54=IDC_S_DIECTION_Y,edit,1350631552
Control55=IDC_S_DIECTION_Z,edit,1350631552
Control56=IDC_SPOT_CUTOFF,edit,1350631552
Control57=IDC_S_CONSTANT,button,1342242819
Control58=IDC_S_LINEAR,button,1342242819
Control59=IDC_S_QUADRATIC,button,1342242819
Control60=IDC_S_ONOFF,button,1342242819

[CLS:CLighting]
Type=0
HeaderFile=Lighting.h
ImplementationFile=Lighting.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CLighting

[DLG:IDD_COLOR_MATERIAL]
Type=1
Class=CLightMaterial
ControlCount=36
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_COLOR_RED,edit,1350631552
Control6=IDC_COLOR_MASK_R,button,1342242819
Control7=IDC_STATIC,static,1342308352
Control8=IDC_COLOR_GREEN,edit,1350631552
Control9=IDC_COLOR_MASK_G,button,1342242819
Control10=IDC_STATIC,static,1342308352
Control11=IDC_COLOR_BLUE,edit,1350631552
Control12=IDC_COLOR_MASK_B,button,1342242819
Control13=IDC_STATIC,button,1342177287
Control14=IDC_M_AMBIENT_R,edit,1350631552
Control15=IDC_M_AMBIENT_G,edit,1350631552
Control16=IDC_M_AMBIENT_B,edit,1350631552
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_M_DIFFUSE_R,edit,1350631552
Control22=IDC_M_DIFFUSE_G,edit,1350631552
Control23=IDC_M_DIFFUSE_B,edit,1350631552
Control24=IDC_STATIC,static,1342308352
Control25=IDC_M_SPECULAR_R,edit,1350631552
Control26=IDC_M_SPECULAR_G,edit,1350631552
Control27=IDC_M_SPECULAR_B,edit,1350631552
Control28=IDC_STATIC,static,1342308352
Control29=IDC_M_EMISSION_R,edit,1350631552
Control30=IDC_M_EMISSION_G,edit,1350631552
Control31=IDC_M_EMISSION_B,edit,1350631552
Control32=IDC_STATIC,static,1342308352
Control33=IDC_STATIC,static,1342308352
Control34=IDC_M_SHININESS,edit,1350631552
Control35=IDC_MATERIAL_FLAG,button,1342242819
Control36=IDC_STATIC,button,1342177287

