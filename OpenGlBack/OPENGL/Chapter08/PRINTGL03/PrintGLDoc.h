// PrintGLDoc.h : interface of the CPrintGLDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRINTGLDOC_H__DEF59F0D_6A94_11D2_8A14_00104B09950B__INCLUDED_)
#define AFX_PRINTGLDOC_H__DEF59F0D_6A94_11D2_8A14_00104B09950B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CPrintGLDoc : public CDocument
{
protected: // create from serialization only
	CPrintGLDoc();
	DECLARE_DYNCREATE(CPrintGLDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintGLDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPrintGLDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPrintGLDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTGLDOC_H__DEF59F0D_6A94_11D2_8A14_00104B09950B__INCLUDED_)
