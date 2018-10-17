// BufferDoc.h : interface of the CBufferDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFFERDOC_H__6BD43DCB_5A2F_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_BUFFERDOC_H__6BD43DCB_5A2F_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CBufferDoc : public CDocument
{
protected: // create from serialization only
	CBufferDoc();
	DECLARE_DYNCREATE(CBufferDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBufferDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBufferDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CBufferDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUFFERDOC_H__6BD43DCB_5A2F_11D2_83B2_0080C832FCF3__INCLUDED_)
