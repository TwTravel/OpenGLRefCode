/********************************************************************

	DemoAniEffDlg.cpp - ISeeͼ���������ͼ����ģ�������DemoAniEffDlg
	ʵ���ļ�

    ��Ȩ����(C) VCHelp-coPathway-ISee workgroup 2000 all member's

    ��һ����������������������������������������GNU ͨ�����֤
	�������޸ĺ����·�����һ���򡣻��������֤�ĵڶ��棬���ߣ�������
	��ѡ�����κθ��µİ汾��

    ������һ�����Ŀ����ϣ�������ã���û���κε���������û���ʺ��ض�
	Ŀ�ص������ĵ���������ϸ����������GNUͨ�����֤��

    ��Ӧ���Ѿ��ͳ���һ���յ�һ��GNUͨ�����֤(GPL)�ĸ����������û�У�
	д�Ÿ���
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	�������ʹ�ñ����ʱ��ʲô������飬�����µ�ַ����������ȡ����
	ϵ��
		http://isee.126.com
		http://www.vchelp.net
	��
		iseesoft@china.com

	���ߣ�¥�L��(leye)
   e-mail:louxiulin@263.net

   ����ʵ�֣�ͼ����̬Ч�����ԶԻ���

	�ļ��汾��
		Build 010317
		Date  2001-03-17

********************************************************************/
// DemoAniEffDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DemoAniEff.h"
#include "DemoAniEffDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ��̬���ص���ͼ���дģ��ĺ������壺
BOOL RunDll(LPSTR szDllname,LPINFOSTR pInfo);
// ��̬���ص���ͼ����ģ��ĺ������壺
BOOL RunDll(LPSTR szDllname,LPIMAGEPROCSTR pInfo);
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDemoAniEffDlg dialog

CDemoAniEffDlg::CDemoAniEffDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDemoAniEffDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDemoAniEffDlg)
	m_nTestItem = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	for(int i=0;i<2;i++)
	{
		m_sInfo[i].pImgInfo=NULL;
		m_sInfo[i]._pbdata=NULL;
		m_sInfo[i].pLineAddr=NULL;
		m_sInfo[i].fpProgress=NULL;
	}
	m_nCur=0;
	m_lpInfo=&m_sInfo[m_nCur];

	hdd=NULL;

	m_sIPS._pdbdata=NULL;
	m_sIPS._psbdata=NULL;
	m_sIPS.pdLineAddr=NULL;
	m_sIPS.psLineAddr=NULL;
}

CDemoAniEffDlg::~CDemoAniEffDlg()	
{
	for(int i=0;i<2;i++)
	{
		if(m_sInfo[i].pImgInfo!=NULL)
		{
			GlobalFree(m_sInfo[i].pImgInfo);
			m_sInfo[i].pImgInfo=NULL;
		}
		if(m_sInfo[i]._pbdata!=NULL)
		{
			GlobalFree(m_sInfo[i]._pbdata);
			m_sInfo[i]._pbdata=NULL;
		}
		if(m_sInfo[i].pLineAddr!=NULL)
		{
			GlobalFree(m_sInfo[i].pLineAddr);
			m_sInfo[i].pLineAddr=NULL;
		}
	}
	if (hdd!=NULL)
		DrawDibClose(hdd);
}

void CDemoAniEffDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemoAniEffDlg)
	DDX_Control(pDX, IDC_TEST, m_ctlTest);
	DDX_Control(pDX, IDC_PREVIEW, m_ctlPreview);
	DDX_Radio(pDX, IDC_LINEUP, m_nTestItem);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDemoAniEffDlg, CDialog)
	//{{AFX_MSG_MAP(CDemoAniEffDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_FILE, OnFile)
	ON_BN_CLICKED(IDC_SETUP, OnSetup)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDemoAniEffDlg message handlers

BOOL CDemoAniEffDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	hdd = DrawDibOpen();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDemoAniEffDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDemoAniEffDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
/*********************Drawlib****************************/
		// Use Drawlib to display image 
		if(m_lpInfo->_pbdata==NULL) return;
		HDC hDC = GetDC()->m_hDC;
		CRect rc;
		m_ctlPreview.GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.DeflateRect(1,1);
		UINT wFlags;
		wFlags = DDF_DONTDRAW ;

		if (hdd != NULL) 
		{ 
			DrawDibDraw(hdd,
						hDC,
						rc.left,
						rc.top,
						rc.Width() ,
						rc.Height(),
						&m_lpInfo->sDIBInfo.bmi,
						m_lpInfo->_pbdata,
						0,
						0,
						m_lpInfo->imginfo.width,
						m_lpInfo->imginfo.height,
						wFlags);
		}
/*********************Drawlib****************************/
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDemoAniEffDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDemoAniEffDlg::OnFile() 
{
	// TODO: Add your control notification handler code here
	//����λͼ�ļ�
	CString	strBmpFile;
	CString strFilter="λͼ�ļ�(*.bmp)|*.bmp|�����ļ�(*.*)|*.*||";
	CFileDialog fdlg(true,NULL,NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strFilter);
	if(fdlg.DoModal()!=IDOK) return;
	strBmpFile=fdlg.GetPathName();

	//���õ�ǰ������Ϣ
	m_lpInfo=&m_sInfo[m_nCur];
	//���bmp�ļ�
	m_lpInfo->comm=CMD_IS_VALID_FILE;
	m_lpInfo->result=ER_EMPTY;
	m_lpInfo->annexdata.iAnnData=0;
	::strcpy(m_lpInfo->filename,LPCTSTR(strBmpFile));
	m_lpInfo->state=PKST_NOTVER;
	RunDll("BmpModule.dll",m_lpInfo);
	if(m_lpInfo->result!=ER_SUCCESS) return;
	//��ȡbmp�ļ���Ϣ
	m_lpInfo->comm=CMD_GET_FILE_INFO;
	m_lpInfo->result=ER_EMPTY;
	memset(&m_lpInfo->imginfo,0,sizeof(IMAGEINFOSTR));
	ASSERT(m_lpInfo->annexdata.iAnnData==1);
	ASSERT(m_lpInfo->state==PKST_PASSVER);
	RunDll("BmpModule.dll",m_lpInfo);
	if(m_lpInfo->result!=ER_SUCCESS) return;
	//��ȡbmp�ļ�����
	m_lpInfo->comm=CMD_LOAD_FROM_FILE;
	m_lpInfo->result=ER_EMPTY;
	m_lpInfo->pImgInfo=NULL;
	BITMAPINFOHEADER	bmi=
	{
		sizeof(BITMAPINFOHEADER),
		m_lpInfo->imginfo.width,
		m_lpInfo->imginfo.height,
		1,//biPlanes
		ISEE_IMAGE_BITCOUNT,//biBitCount
		BI_RGB,//biCompression
		0,//biSizeImage
		0,
		0,
		0,
		0
	};
	m_lpInfo->sDIBInfo.bmi=bmi;
	m_lpInfo->sDIBInfo.rmask     = 0xff0000;
	m_lpInfo->sDIBInfo.gmask     = 0xff00;
	m_lpInfo->sDIBInfo.bmask     = 0xff;
	m_lpInfo->sDIBInfo.alphaMask = 0;
	// �������ڴ�ű�׼ͼ��λ���ݵ��ڴ��
	if(m_lpInfo->_pbdata!=NULL)
	{
		GlobalFree(m_lpInfo->_pbdata);
		m_lpInfo->_pbdata=NULL;
	}
	m_lpInfo->_pbdata = (PBYTE)::GlobalAlloc(GMEM_FIXED, m_lpInfo->sDIBInfo.bmi.biWidth*(ISEE_IMAGE_BITCOUNT/8)*m_lpInfo->sDIBInfo.bmi.biHeight);
	// �������ڴ��ÿһɨ���е�ַ��ָ�����鲢��ʼ��
	if(m_lpInfo->pLineAddr!=NULL)
	{
		GlobalFree(m_lpInfo->pLineAddr);
		m_lpInfo->pLineAddr=NULL;
	}
	m_lpInfo->pLineAddr = (DWORD**)::GlobalAlloc(GPTR, m_lpInfo->sDIBInfo.bmi.biHeight*sizeof(DWORD*));
	// ��ʼ����������
	for (int y=0;y<m_lpInfo->sDIBInfo.bmi.biHeight;y++)
		m_lpInfo->pLineAddr[y] = (DWORD*)(m_lpInfo->_pbdata+((m_lpInfo->sDIBInfo.bmi.biHeight-y-1)*(m_lpInfo->sDIBInfo.bmi.biWidth*(ISEE_IMAGE_BITCOUNT/8))));
	ASSERT(m_lpInfo->state==PKST_PASSINFO);
	m_lpInfo->fpProgress=NULL;
	// ��ȡ�ļ��е�ͼ������
	RunDll("BmpModule.dll",m_lpInfo);
	if(m_lpInfo->result!=ER_SUCCESS) return;
	//����ָ��(0,1)
	m_nCur++;
	m_nCur%=2;
	//������ͼ
	CRect rc;
	m_ctlPreview.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.DeflateRect(1,1);
	InvalidateRect(rc);
	//����"����"��ť״̬
	m_ctlTest.EnableWindow(m_sInfo[0]._pbdata&&m_sInfo[1]._pbdata);
}

void CDemoAniEffDlg::OnSetup() 
{
	// TODO: Add your control notification handler code here
	//���ö�̬�������ò���
	m_sIPS.state=IPDPS_CONFIG;
	RunDll("flt_anieff.dll",&m_sIPS);
}

void CDemoAniEffDlg::OnTest() 
{
	UpdateData(TRUE);
	static int times=0;//���Դ���
	CRect rc;
	m_ctlPreview.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.DeflateRect(1,1);
	// ���ݰ���ڲ�������
	m_sIPS.state=IPDPS_RUN;//���ö�̬�������в���
	if(m_nTestItem==0) m_sIPS.comm=PCM_LINEUP;//���ö�̬Ч�������ֱ����䡪���л�
	else m_sIPS.comm=PCM_FADEIN;//��ӳ�������뵭��Ч��
	m_sIPS.result=PR_NULL;
	m_sIPS.annexdata.pAnnData=&rc;//��ͼ��Χ
	m_sIPS.hParentWnd=GetSafeHwnd();//��ͼ���ھ��
	//����Դͼ����Ϣ������
	m_sIPS.sImageInfo.width=m_sInfo[(m_nCur+1+times)%2].imginfo.width;
	m_sIPS.sImageInfo.height=m_sInfo[(m_nCur+1+times)%2].imginfo.height;
	m_sIPS.sImageInfo.bitperpix=32;
	m_sIPS.sImageInfo.byteperline=m_sInfo[(m_nCur+1+times)%2].imginfo.width*4;
	m_sIPS.sImageInfo.bAlpha=TRUE;
	m_sIPS._psbdata=m_sInfo[(m_nCur+1+times)%2]._pbdata;
	m_sIPS.psLineAddr=m_sInfo[(m_nCur+1+times)%2].pLineAddr;
	//����Ŀ��ͼ����Ϣ������
	m_sIPS.dImageInfo.width=m_sInfo[(m_nCur+times)%2].imginfo.width;
	m_sIPS.dImageInfo.height=m_sInfo[(m_nCur+times)%2].imginfo.height;
	m_sIPS.dImageInfo.bitperpix=32;
	m_sIPS.dImageInfo.byteperline=m_sInfo[(m_nCur+times)%2].imginfo.width*4;
	m_sIPS.dImageInfo.bAlpha=TRUE;
	m_sIPS._pdbdata=m_sInfo[(m_nCur+times)%2]._pbdata;
	m_sIPS.pdLineAddr=m_sInfo[(m_nCur+times)%2].pLineAddr;
	//���ö�̬�������в���
	RunDll("flt_anieff.dll",&m_sIPS);
	times++;
}

// ��̬���ص���ͼ���дģ��ĺ������壺
BOOL RunDll(LPSTR szDllname,LPINFOSTR pInfo)
{
	typedef int (WINAPI* MYPROC)(LPINFOSTR lpInfo);
    HMODULE hLib;
    MYPROC lpFunc;
    char s[MAX_PATH];
            
    hLib=LoadLibrary(szDllname);
    if(hLib==NULL)
    {
		wsprintf(s,"���ض�̬���ӿ�ʧ�ܣ�%s",szDllname);
        MessageBox(GetFocus(),s,"����",MB_OK);
        return FALSE;
    }
            
    lpFunc=(MYPROC)GetProcAddress(hLib,MAKEINTRESOURCE(1));
    if(lpFunc==NULL)
	{
         MessageBox(GetFocus(),"����ʧ�ܣ�δ�ҵ���ں���","����",MB_OK);
         return FALSE;
    }
            
    BOOL bOK=(lpFunc)(pInfo);
    FreeLibrary(hLib);
    return bOK;
}

// ��̬���ص���ͼ����ģ��ĺ���ʵ�֣�
BOOL RunDll(LPSTR szDllname,LPIMAGEPROCSTR pInfo)
{
	typedef int (WINAPI* MYPROC)(LPIMAGEPROCSTR lpInfo);
    HMODULE hLib;
    MYPROC lpFunc;
    char s[MAX_PATH];
            
    hLib=LoadLibrary(szDllname);
    if(hLib==NULL)
    {
		wsprintf(s,"���ض�̬���ӿ�ʧ�ܣ�%s",szDllname);
        MessageBox(GetFocus(),s,"����",MB_OK);
        return FALSE;
    }
            
    lpFunc=(MYPROC)GetProcAddress(hLib,MAKEINTRESOURCE(1));
    if(lpFunc==NULL)
	{
         MessageBox(GetFocus(),"����ʧ�ܣ�δ�ҵ���ں���","����",MB_OK);
         return FALSE;
    }
            
    pInfo->hInst=hLib;
    BOOL bOK=(lpFunc)(pInfo);
    FreeLibrary(hLib);
    return bOK;
}

