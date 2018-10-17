#if !defined(AFX_LIGHTMATERIAL_H__0F817C01_2D0B_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_LIGHTMATERIAL_H__0F817C01_2D0B_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LightMaterial.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLightMaterial dialog

class CLightMaterial : public CDialog
{
// Construction
public:
	CLightMaterial(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLightMaterial)
	enum { IDD = IDD_COLOR_MATERIAL };
	float	m_colorBlue;
	float	m_colorGreen;
	float	m_colorRed;
	BOOL	m_maskBlue;
	BOOL	m_maskGreen;
	BOOL	m_maskRed;
	float	m_matAmbientB;
	float	m_matAmbientG;
	float	m_matAmbientR;
	float	m_matDiffuseB;
	float	m_matDiffuseG;
	float	m_matDiffuseR;
	float	m_matEmissionB;
	float	m_matEmissionG;
	float	m_matEmissionR;
	float	m_matShininess;
	float	m_matSpecularB;
	float	m_matSpecularG;
	float	m_matSpecularR;
	BOOL	m_bMaterialOnOff;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLightMaterial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL m_bEditFlag;

	// Generated message map functions
	//{{AFX_MSG(CLightMaterial)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIGHTMATERIAL_H__0F817C01_2D0B_11D2_83B2_0080C832FCF3__INCLUDED_)
