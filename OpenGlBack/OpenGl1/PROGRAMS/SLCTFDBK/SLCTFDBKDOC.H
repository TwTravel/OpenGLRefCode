// SlctFdbkDoc.h : interface of the CSlctFdbkDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLCTFDBKDOC_H__BD34BF0B_5C2D_11D2_83B3_0080C832FCF3__INCLUDED_)
#define AFX_SLCTFDBKDOC_H__BD34BF0B_5C2D_11D2_83B3_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CSlctFdbkDoc : public CDocument
{
protected: // create from serialization only
	CSlctFdbkDoc();
	DECLARE_DYNCREATE(CSlctFdbkDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlctFdbkDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSlctFdbkDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSlctFdbkDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLCTFDBKDOC_H__BD34BF0B_5C2D_11D2_83B3_0080C832FCF3__INCLUDED_)
