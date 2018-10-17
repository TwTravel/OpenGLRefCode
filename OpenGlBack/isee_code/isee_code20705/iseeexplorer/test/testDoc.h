// testDoc.h : interface of the CTestDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTDOC_H__E3EA57CA_5C2C_11D5_8853_9341F819D4F2__INCLUDED_)
#define AFX_TESTDOC_H__E3EA57CA_5C2C_11D5_8853_9341F819D4F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DEFAULT_PATH		"c:\\"

class CTestDoc : public CDocument
{
protected: // create from serialization only
	CTestDoc();
	DECLARE_DYNCREATE(CTestDoc)

// Attributes
public:
	cplugin		cpi;				// 插件信息组
	cfiletab	cft;				// 文件信息组
	ctimer		ctr;				// 计时类

	CString		cpath;				// 当前图像文件所在路径

	bool		play_order;			// 播放顺序
	char		ini_path[_MAX_PATH];
	char		err_path[_MAX_PATH];
	char		buff[_MAX_PATH];

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTestDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTDOC_H__E3EA57CA_5C2C_11D5_8853_9341F819D4F2__INCLUDED_)
