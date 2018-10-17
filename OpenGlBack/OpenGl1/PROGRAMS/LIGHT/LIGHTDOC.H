// LightDoc.h : interface of the CLightDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTDOC_H__9F8D41CB_2C7B_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_LIGHTDOC_H__9F8D41CB_2C7B_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CLightDoc : public CDocument
{
protected: // create from serialization only
	CLightDoc();
	DECLARE_DYNCREATE(CLightDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLightDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLightDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CLightDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIGHTDOC_H__9F8D41CB_2C7B_11D2_83B2_0080C832FCF3__INCLUDED_)
