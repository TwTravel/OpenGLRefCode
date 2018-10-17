// MatEditor.cpp : implementation file
//

#include "stdafx.h"
#include "matlight.h"
#include "MatEditor.h"
#include "ShineEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMatEditor

IMPLEMENT_DYNAMIC(CMatEditor, CPropertySheet)

CMatEditor::CMatEditor(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CMatEditor::CMatEditor(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
m_pAmbientPage = new CEditor(IDD_EDITOR,IDS_AMBIENT);
m_pDiffusePage = new CEditor(IDD_EDITOR,IDS_DIFFUSE);
m_pSpecularPage = new CEditor(IDD_EDITOR,IDS_SPECULAR);
m_pShinePage = new CShineEdit();

AddPage(m_pAmbientPage);
AddPage(m_pDiffusePage);
AddPage(m_pSpecularPage);
AddPage(m_pShinePage);
}

CMatEditor::~CMatEditor()
{
delete m_pShinePage;
delete m_pSpecularPage;
delete m_pDiffusePage;
delete m_pAmbientPage;
}


BEGIN_MESSAGE_MAP(CMatEditor, CPropertySheet)
	//{{AFX_MSG_MAP(CMatEditor)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE,updateFunc)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMatEditor message handlers

BOOL CMatEditor::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();

	m_pAmbientPage->SetValues(m_pDocumentPointer->fMatAmbient);
	m_pDiffusePage->SetValues(m_pDocumentPointer->fMatDiffuse);
	m_pSpecularPage->SetValues(m_pDocumentPointer->fMatSpecular);
	m_pShinePage->SetValue(int(m_pDocumentPointer->fShine));

	m_pAmbientPage->m_pParent = this;
	m_pDiffusePage->m_pParent = this;
	m_pSpecularPage->m_pParent = this;
	m_pShinePage->m_pParent = this;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



LONG CMatEditor::updateFunc(UINT, LONG)
	{
	// Pull the values and update the document
	m_pAmbientPage->GetValues(m_pDocumentPointer->fMatAmbient);
	m_pDiffusePage->GetValues(m_pDocumentPointer->fMatDiffuse);
	m_pSpecularPage->GetValues(m_pDocumentPointer->fMatSpecular);
	m_pDocumentPointer->fShine = (float) m_pShinePage->GetValue();

	m_pDocumentPointer->UpdateAllViews(NULL,0,NULL);
	return 0L;
	}


void CMatEditor::Update()
	{
	updateFunc(0,0);
	}