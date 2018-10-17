// JgfzDoc.h : interface of the CJgfzDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_JGFZDOC_H__AD0CFFCB_8176_11D2_83B3_0080C832FCF3__INCLUDED_)
#define AFX_JGFZDOC_H__AD0CFFCB_8176_11D2_83B3_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CJgfzDoc : public CDocument
{
protected: // create from serialization only
	CJgfzDoc();
	DECLARE_DYNCREATE(CJgfzDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJgfzDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CJgfzDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CJgfzDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JGFZDOC_H__AD0CFFCB_8176_11D2_83B3_0080C832FCF3__INCLUDED_)
