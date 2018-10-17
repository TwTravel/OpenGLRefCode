// ShadowDoc.h : interface of the CShadowDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADOWDOC_H__2F29B08B_8086_11D2_83B3_0080C832FCF3__INCLUDED_)
#define AFX_SHADOWDOC_H__2F29B08B_8086_11D2_83B3_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CShadowDoc : public CDocument
{
protected: // create from serialization only
	CShadowDoc();
	DECLARE_DYNCREATE(CShadowDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShadowDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CShadowDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CShadowDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHADOWDOC_H__2F29B08B_8086_11D2_83B3_0080C832FCF3__INCLUDED_)
