// Lighting.cpp : implementation file
//

#include "stdafx.h"
#include "Light.h"
#include "Lighting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLighting dialog


CLighting::CLighting(CWnd* pParent /*=NULL*/)
	: CDialog(CLighting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLighting)
	m_lightAmbientB = 0.0f;
	m_lightAmbientG = 0.0f;
	m_lightAmbientR = 0.0f;
	m_lightDiffuseB = 0.0f;
	m_lightDiffuseG = 0.0f;
	m_lightDiffuseR = 0.0f;
	m_lightPositionX = 0.0f;
	m_lightPositionY = 0.0f;
	m_lightSpecularB = 0.0f;
	m_lightSpecularG = 0.0f;
	m_lightSpecularR = 0.0f;
	m_lightPositionZ = 0.0f;
	m_spotAmbientB = 0.0f;
	m_spotAmbientG = 0.0f;
	m_spotDirectionX = 0.0f;
	m_spotDiffuseB = 0.0f;
	m_spotDiffuseG = 0.0f;
	m_spotDiffuseR = 0.0f;
	m_spotAmbientR = 0.0f;
	m_spotDirectionY = 0.0f;
	m_spotDirectionZ = 0.0f;
	m_spotPositionX = 0.0f;
	m_spotPositionY = 0.0f;
	m_spotPositionZ = 0.0f;
	m_spotSpecularB = 0.0f;
	m_spotSpecularG = 0.0f;
	m_spotSpecularR = 0.0f;
	m_spotCutoff = 0.0f;
	m_bConstant = FALSE;
	m_bLinear = FALSE;
	m_bQuadratic = FALSE;
	m_bSpotOnOff = FALSE;
	//}}AFX_DATA_INIT
}


void CLighting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLighting)
	DDX_Text(pDX, IDC_L_AMBIENT_B, m_lightAmbientB);
	DDX_Text(pDX, IDC_L_AMBIENT_G, m_lightAmbientG);
	DDX_Text(pDX, IDC_L_AMBIENT_R, m_lightAmbientR);
	DDX_Text(pDX, IDC_L_DIFFUSE_B, m_lightDiffuseB);
	DDX_Text(pDX, IDC_L_DIFFUSE_G, m_lightDiffuseG);
	DDX_Text(pDX, IDC_L_DIFFUSE_R, m_lightDiffuseR);
	DDX_Text(pDX, IDC_L_POSITION_X, m_lightPositionX);
	DDX_Text(pDX, IDC_L_POSITION_Y, m_lightPositionY);
	DDX_Text(pDX, IDC_L_SPECULAR_B, m_lightSpecularB);
	DDX_Text(pDX, IDC_L_SPECULAR_G, m_lightSpecularG);
	DDX_Text(pDX, IDC_L_SPECULAR_R, m_lightSpecularR);
	DDX_Text(pDX, IDC_L_POSITION_Z, m_lightPositionZ);
	DDX_Text(pDX, IDC_S_AMBIENT_B, m_spotAmbientB);
	DDX_Text(pDX, IDC_S_AMBIENT_G, m_spotAmbientG);
	DDX_Text(pDX, IDC_S_DIECTION_X, m_spotDirectionX);
	DDX_Text(pDX, IDC_S_DIFFUSE_B, m_spotDiffuseB);
	DDX_Text(pDX, IDC_S_DIFFUSE_G, m_spotDiffuseG);
	DDX_Text(pDX, IDC_S_DIFFUSE_R, m_spotDiffuseR);
	DDX_Text(pDX, IDC_S_AMBIENT_R, m_spotAmbientR);
	DDX_Text(pDX, IDC_S_DIECTION_Y, m_spotDirectionY);
	DDX_Text(pDX, IDC_S_DIECTION_Z, m_spotDirectionZ);
	DDX_Text(pDX, IDC_S_POSITION_X, m_spotPositionX);
	DDX_Text(pDX, IDC_S_POSITION_Y, m_spotPositionY);
	DDX_Text(pDX, IDC_S_POSITION_Z, m_spotPositionZ);
	DDX_Text(pDX, IDC_S_SPECULAR_B, m_spotSpecularB);
	DDX_Text(pDX, IDC_S_SPECULAR_G, m_spotSpecularG);
	DDX_Text(pDX, IDC_S_SPECULAR_R, m_spotSpecularR);
	DDX_Text(pDX, IDC_SPOT_CUTOFF, m_spotCutoff);
	DDX_Check(pDX, IDC_S_CONSTANT, m_bConstant);
	DDX_Check(pDX, IDC_S_LINEAR, m_bLinear);
	DDX_Check(pDX, IDC_S_QUADRATIC, m_bQuadratic);
	DDX_Check(pDX, IDC_S_ONOFF, m_bSpotOnOff);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLighting, CDialog)
	//{{AFX_MSG_MAP(CLighting)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLighting message handlers

