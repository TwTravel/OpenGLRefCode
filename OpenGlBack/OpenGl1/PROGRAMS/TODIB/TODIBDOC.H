// ToDibDoc.h : interface of the CToDibDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TODIBDOC_H__7C3B2DED_805E_11D2_83B4_0080C832FCF3__INCLUDED_)
#define AFX_TODIBDOC_H__7C3B2DED_805E_11D2_83B4_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CToDibDoc : public CDocument
{
protected: // create from serialization only
	CToDibDoc();
	DECLARE_DYNCREATE(CToDibDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToDibDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToDibDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CToDibDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TODIBDOC_H__7C3B2DED_805E_11D2_83B4_0080C832FCF3__INCLUDED_)
