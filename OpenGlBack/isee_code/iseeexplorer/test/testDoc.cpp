// testDoc.cpp : implementation of the CTestDoc class
//

#include "stdafx.h"
#include "test.h"

#include "testDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char buff[MAX_PATH];

/////////////////////////////////////////////////////////////////////////////
// CTestDoc

IMPLEMENT_DYNCREATE(CTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CTestDoc, CDocument)
	//{{AFX_MSG_MAP(CTestDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDoc construction/destruction

CTestDoc::CTestDoc()
{
	char *p;

	// 获取本软件所在路径
	::GetModuleFileName(NULL, (LPSTR)buff, _MAX_PATH);

	if ((p=strrchr((const char *)buff, (int)'\\')) != 0)
	{
		*++p = 0;
		cpi.init(buff);			// 初始化插件数组
	}

	// 初始化图像文件路径
	FILE	*fp;

	strcat(buff, "test.err");
	strcpy((char*)err_path, (const char *)buff);
	*p = 0;
	strcat(buff, "test.ini");
	strcpy((char*)ini_path, (const char *)buff);

	if ((fp=fopen((const char *)buff, "r")) == 0)
	{
		if ((fp=fopen((const char *)buff, "w")) == 0)
		{
			cpath = (LPCTSTR)DEFAULT_PATH;
		}
		else
		{
			fputs((const char *)DEFAULT_PATH, fp);
			cpath = (LPCTSTR)DEFAULT_PATH;
			fclose(fp);
		}
	}
	else
	{
		memset((void*)buff, 0, _MAX_PATH);
		fgets(buff, _MAX_PATH, fp);
		cpath = (LPCTSTR)buff;
		fclose(fp);
	}

	// 初始化文件信息数组
	cft.compages((char*)(LPCTSTR)cpath, cpi.get_cur_plugin_info());

	play_order = 0;				// 播放顺序为 next
}


CTestDoc::~CTestDoc()
{
	cft.disassemble();			// 销毁文件信息数组
	cpi.destroy();				// 销毁插件数组
}

BOOL CTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTestDoc serialization

void CTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTestDoc diagnostics

#ifdef _DEBUG
void CTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestDoc commands
