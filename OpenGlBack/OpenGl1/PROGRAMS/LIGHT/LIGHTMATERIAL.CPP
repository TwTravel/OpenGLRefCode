// LightMaterial.cpp : implementation file
//

#include "stdafx.h"
#include "Light.h"
#include "LightMaterial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLightMaterial dialog


CLightMaterial::CLightMaterial(CWnd* pParent /*=NULL*/)
	: CDialog(CLightMaterial::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLightMaterial)
	m_colorBlue = 0.0f;
	m_colorGreen = 0.0f;
	m_colorRed = 0.0f;
	m_maskBlue = FALSE;
	m_maskGreen = FALSE;
	m_maskRed = FALSE;
	m_matAmbientB = 0.0f;
	m_matAmbientG = 0.0f;
	m_matAmbientR = 0.0f;
	m_matDiffuseB = 0.0f;
	m_matDiffuseG = 0.0f;
	m_matDiffuseR = 0.0f;
	m_matEmissionB = 0.0f;
	m_matEmissionG = 0.0f;
	m_matEmissionR = 0.0f;
	m_matShininess = 0.0f;
	m_matSpecularB = 0.0f;
	m_matSpecularG = 0.0f;
	m_matSpecularR = 0.0f;
	m_bMaterialOnOff = FALSE;
	//}}AFX_DATA_INIT

}


void CLightMaterial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLightMaterial)
	DDX_Text(pDX, IDC_COLOR_BLUE, m_colorBlue);
	DDX_Text(pDX, IDC_COLOR_GREEN, m_colorGreen);
	DDX_Text(pDX, IDC_COLOR_RED, m_colorRed);
	DDX_Check(pDX, IDC_COLOR_MASK_B, m_maskBlue);
	DDX_Check(pDX, IDC_COLOR_MASK_G, m_maskGreen);
	DDX_Check(pDX, IDC_COLOR_MASK_R, m_maskRed);
	DDX_Text(pDX, IDC_M_AMBIENT_B, m_matAmbientB);
	DDX_Text(pDX, IDC_M_AMBIENT_G, m_matAmbientG);
	DDX_Text(pDX, IDC_M_AMBIENT_R, m_matAmbientR);
	DDX_Text(pDX, IDC_M_DIFFUSE_B, m_matDiffuseB);
	DDX_Text(pDX, IDC_M_DIFFUSE_G, m_matDiffuseG);
	DDX_Text(pDX, IDC_M_DIFFUSE_R, m_matDiffuseR);
	DDX_Text(pDX, IDC_M_EMISSION_B, m_matEmissionB);
	DDX_Text(pDX, IDC_M_EMISSION_G, m_matEmissionG);
	DDX_Text(pDX, IDC_M_EMISSION_R, m_matEmissionR);
	DDX_Text(pDX, IDC_M_SHININESS, m_matShininess);
	DDX_Text(pDX, IDC_M_SPECULAR_B, m_matSpecularB);
	DDX_Text(pDX, IDC_M_SPECULAR_G, m_matSpecularG);
	DDX_Text(pDX, IDC_M_SPECULAR_R, m_matSpecularR);
	DDX_Check(pDX, IDC_MATERIAL_FLAG, m_bMaterialOnOff);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLightMaterial, CDialog)
	//{{AFX_MSG_MAP(CLightMaterial)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLightMaterial message handlers


