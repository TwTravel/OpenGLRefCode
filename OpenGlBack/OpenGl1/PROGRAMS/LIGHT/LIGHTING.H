#if !defined(AFX_LIGHTING_H__D179CBA1_2E05_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_LIGHTING_H__D179CBA1_2E05_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Lighting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLighting dialog

class CLighting : public CDialog
{
// Construction
public:
	CLighting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLighting)
	enum { IDD = IDD_LIGHT };
	float	m_lightAmbientB;
	float	m_lightAmbientG;
	float	m_lightAmbientR;
	float	m_lightDiffuseB;
	float	m_lightDiffuseG;
	float	m_lightDiffuseR;
	float	m_lightPositionX;
	float	m_lightPositionY;
	float	m_lightSpecularB;
	float	m_lightSpecularG;
	float	m_lightSpecularR;
	float	m_lightPositionZ;
	float	m_spotAmbientB;
	float	m_spotAmbientG;
	float	m_spotDirectionX;
	float	m_spotDiffuseB;
	float	m_spotDiffuseG;
	float	m_spotDiffuseR;
	float	m_spotAmbientR;
	float	m_spotDirectionY;
	float	m_spotDirectionZ;
	float	m_spotPositionX;
	float	m_spotPositionY;
	float	m_spotPositionZ;
	float	m_spotSpecularB;
	float	m_spotSpecularG;
	float	m_spotSpecularR;
	float	m_spotCutoff;
	BOOL	m_bConstant;
	BOOL	m_bLinear;
	BOOL	m_bQuadratic;
	BOOL	m_bSpotOnOff;
	//}}AFX_DATA

	


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLighting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CLighting)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIGHTING_H__D179CBA1_2E05_11D2_83B2_0080C832FCF3__INCLUDED_)
