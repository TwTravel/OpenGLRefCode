// UtilityDoc.h : interface of the CUtilityDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILITYDOC_H__A6B1CFAD_4E65_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_UTILITYDOC_H__A6B1CFAD_4E65_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CUtilityDoc : public CDocument
{
protected: // create from serialization only
	CUtilityDoc();
	DECLARE_DYNCREATE(CUtilityDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUtilityDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUtilityDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CUtilityDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UTILITYDOC_H__A6B1CFAD_4E65_11D2_83B2_0080C832FCF3__INCLUDED_)
