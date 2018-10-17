// ModelDoc.h : interface of the CModelDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELDOC_H__3009D04B_7F26_11D2_83B3_0080C832FCF3__INCLUDED_)
#define AFX_MODELDOC_H__3009D04B_7F26_11D2_83B3_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CModelDoc : public CDocument
{
protected: // create from serialization only
	CModelDoc();
	DECLARE_DYNCREATE(CModelDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CModelDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CModelDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELDOC_H__3009D04B_7F26_11D2_83B3_0080C832FCF3__INCLUDED_)
