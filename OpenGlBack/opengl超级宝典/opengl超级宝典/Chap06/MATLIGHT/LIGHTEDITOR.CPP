// LightEditor.cpp : implementation file
//

#include "stdafx.h"
#include "matlight.h"

#include "MainFrm.h"
#include "matlight.h"
#include "LightEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLightEditor

IMPLEMENT_DYNAMIC(CLightEditor, CPropertySheet)

// This constructor is not used
CLightEditor::CLightEditor(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

// We are only using this one
CLightEditor::CLightEditor(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
m_pAmbientPage = new CEditor(IDD_EDITOR,IDS_AMBIENT);
m_pDiffusePage = new CEditor(IDD_EDITOR,IDS_DIFFUSE);
m_pSpecularPage = new CEditor(IDD_EDITOR,IDS_SPECULAR);
AddPage(m_pAmbientPage);
AddPage(m_pDiffusePage);
AddPage(m_pSpecularPage);
}

CLightEditor::~CLightEditor()
{
delete m_pSpecularPage;
delete m_pDiffusePage;
delete m_pAmbientPage;
}


BEGIN_MESSAGE_MAP(CLightEditor, CPropertySheet)
	//{{AFX_MSG_MAP(CLightEditor)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE,updateFunc)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLightEditor message handlers

BOOL CLightEditor::OnInitDialog() 
	{
	m_pAmbientPage->SetValues(m_pDocumentPointer->fLight0Ambient);
	m_pDiffusePage->SetValues(m_pDocumentPointer->fLight0Diffuse);
	m_pSpecularPage->SetValues(m_pDocumentPointer->fLight0Specular);

	m_pAmbientPage->m_pParent = this;
	m_pDiffusePage->m_pParent = this;
	m_pSpecularPage->m_pParent = this;

	return CPropertySheet::OnInitDialog();
	}

LONG CLightEditor::updateFunc(UINT, LONG)
	{
	// Pull the values and update the document
	m_pAmbientPage->GetValues(m_pDocumentPointer->fLight0Ambient);
	m_pDiffusePage->GetValues(m_pDocumentPointer->fLight0Diffuse);
	m_pSpecularPage->GetValues(m_pDocumentPointer->fLight0Specular);
	m_pDocumentPointer->UpdateAllViews(NULL,0,NULL);


	
	return 0L;
	}


void CLightEditor::Update()
	{
	updateFunc(0,0);
	}