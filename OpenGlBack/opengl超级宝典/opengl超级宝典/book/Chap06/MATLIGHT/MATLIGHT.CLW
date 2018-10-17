; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMatlightView
LastTemplate=CPropertyPage
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "matlight.h"
LastPage=0

ClassCount=11
Class1=CMatlightApp
Class2=CMatlightDoc
Class3=CMatlightView
Class4=CMainFrame

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDD_SHINE
Class5=CAboutDlg
Resource3=IDD_EDITOR
Class6=CLightPage
Class7=CMaterialPage
Class8=CShineEdit
Resource4=IDD_MATERIAL
Class9=CEditor
Class10=CMatEditor
Class11=CLightEditor
Resource5=IDR_MAINFRAME

[CLS:CMatlightApp]
Type=0
HeaderFile=matlight.h
ImplementationFile=matlight.cpp
Filter=N

[CLS:CMatlightDoc]
Type=0
HeaderFile=matlightDoc.h
ImplementationFile=matlightDoc.cpp
Filter=N
BaseClass=CDocument
VirtualFilter=DC
LastObject=CMatlightDoc

[CLS:CMatlightView]
Type=0
HeaderFile=matlightView.h
ImplementationFile=matlightView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=CMatlightView

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CFrameWnd
VirtualFilter=fWC
LastObject=CMainFrame



[CLS:CAboutDlg]
Type=0
HeaderFile=matlight.cpp
ImplementationFile=matlight.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_PRINT
Command2=ID_FILE_PRINT_PREVIEW
Command3=ID_FILE_PRINT_SETUP
Command4=ID_APP_EXIT
Command5=ID_EDIT_LIGHT
Command6=ID_EDIT_MATERIAL
Command7=ID_VIEW_TOOLBAR
Command8=ID_VIEW_STATUS_BAR
Command9=ID_APP_ABOUT
CommandCount=9

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_PRINT
CommandCount=1

[TB:IDR_MAINFRAME]
Type=1
Command1=ID_EDIT_LIGHT
Command2=ID_EDIT_MATERIAL
Command3=ID_FILE_PRINT
Command4=ID_APP_ABOUT
CommandCount=4

[DLG:IDD_MATERIAL]
Type=1
Class=CMaterialPage
ControlCount=33
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT5,edit,1350631552
Control7=IDC_SPIN4,msctls_updown32,1342177332
Control8=IDC_EDIT6,edit,1350631552
Control9=IDC_SPIN5,msctls_updown32,1342177332
Control10=IDC_EDIT7,edit,1350631552
Control11=IDC_SPIN6,msctls_updown32,1342177332
Control12=IDC_STATIC,button,1342177287
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_EDIT8,edit,1350631552
Control17=IDC_SPIN7,msctls_updown32,1342177332
Control18=IDC_EDIT9,edit,1350631552
Control19=IDC_SPIN8,msctls_updown32,1342177332
Control20=IDC_EDIT10,edit,1350631552
Control21=IDC_SPIN9,msctls_updown32,1342177332
Control22=IDC_STATIC,button,1342177287
Control23=IDC_STATIC,static,1342308352
Control24=IDC_STATIC,static,1342308352
Control25=IDC_STATIC,static,1342308352
Control26=IDC_EDIT11,edit,1350631552
Control27=IDC_SPIN10,msctls_updown32,1342177332
Control28=IDC_EDIT12,edit,1350631552
Control29=IDC_SPIN11,msctls_updown32,1342177332
Control30=IDC_EDIT13,edit,1350631552
Control31=IDC_SPIN12,msctls_updown32,1342177332
Control32=IDC_SLIDER1,msctls_trackbar32,1342242836
Control33=IDC_EDIT1,edit,1350631552

[CLS:CLightPage]
Type=0
HeaderFile=LightPage.h
ImplementationFile=LightPage.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CLightPage

[CLS:CMaterialPage]
Type=0
HeaderFile=MaterialPage.h
ImplementationFile=MaterialPage.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CMaterialPage
VirtualFilter=idWC

[DLG:IDD_EDITOR]
Type=1
Class=CEditor
ControlCount=9
Control1=IDC_STATIC,static,1342308352
Control2=IDC_EDT_RED,edit,1350633600
Control3=IDC_SLD_RED,msctls_trackbar32,1350631429
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDT_GREEN,edit,1350633600
Control6=IDC_SLD_GREEN,msctls_trackbar32,1350631429
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDT_BLUE,edit,1350633600
Control9=IDC_SLD_BLUE,msctls_trackbar32,1350631429

[DLG:IDD_SHINE]
Type=1
Class=CShineEdit
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_EDT_SHINE,edit,1350633600
Control3=IDC_SPN_SHINE,msctls_updown32,1342177335
Control4=IDC_STATIC,static,1342308352

[CLS:CEditor]
Type=0
HeaderFile=Editor.h
ImplementationFile=Editor.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=IDC_EDT_RED
VirtualFilter=idWC

[CLS:CMatEditor]
Type=0
HeaderFile=MatEditor.h
ImplementationFile=MatEditor.cpp
BaseClass=CPropertySheet
Filter=W
LastObject=CMatEditor
VirtualFilter=hWC

[CLS:CLightEditor]
Type=0
HeaderFile=LightEditor.h
ImplementationFile=LightEditor.cpp
BaseClass=CPropertySheet
Filter=W
LastObject=CLightEditor
VirtualFilter=hWC

[CLS:CShineEdit]
Type=0
HeaderFile=ShineEdit.h
ImplementationFile=ShineEdit.cpp
BaseClass=CPropertyPage
Filter=D
VirtualFilter=idWC
LastObject=CShineEdit

