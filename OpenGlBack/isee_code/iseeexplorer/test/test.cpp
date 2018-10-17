// test.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "test.h"

#include "MainFrm.h"
#include "testDoc.h"
#include "testView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestApp

BEGIN_MESSAGE_MAP(CTestApp, CWinApp)
	//{{AFX_MSG_MAP(CTestApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestApp construction

CTestApp::CTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTestApp object

CTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTestApp initialization

BOOL CTestApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("ISee Workgroup"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CTestDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CTestView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


int CTestApp::ExitInstance() 
{
	return CWinApp::ExitInstance();
}

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
		// No message handlers
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
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CTestApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CTestApp message handlers





/////////////////////////////////////////////////////////////////////////////
// FILEINFO class implement

FILEINFO::FILEINFO()
{
	memset((void*)name, 0, _MAX_PATH);
	played = false;
	avail  = false;
}


FILEINFO::~FILEINFO()
{
}


FILEINFO& FILEINFO::operator= (FILEINFO& sou)
{
	memcpy((void*)name, (const void *)sou.name, _MAX_PATH);
	played = sou.played;
	avail  = sou.avail;

	return *this;
}


/////////////////////////////////////////////////////////////////////////////
// cfiletab class implement

cfiletab::cfiletab()
{
	init_mark = false;
	cur_index = 0;
	filearry.SetSize(0, 256);
	buff[0] = 0;
}


cfiletab::~cfiletab()
{
	if (get_status())
		disassemble();
}


int cfiletab::compages(char *path, LPIRWP_INFO pin)
{
	CFileFind	ffd;
	BOOL		findmark;
	FILEINFO	fin;
	char		*p;
	int			count;
	
	if (init_mark)
		return 1;				// ��ֹ�ظ���ʼ��
	
	strcpy(buff, path);
	if (buff[strlen(path)-1] != '\\')
		strcat(buff, "\\");

	p = &(buff[strlen(buff)]);	// ����������ʱ�ĸ�����ʼ��
	
	ASSERT(pin->init_tag);		// �����Ϣ�����Ѿ������ģ���ʼ������

	strcat(buff, "*.");
	strcat(buff, pin->irwp_desc_info.idi_currency_name);

	count = 0;

	do
	{
		findmark = ffd.FindFile((LPCTSTR)buff, 0);
		if (findmark)
		{
			while (findmark)
			{
				findmark = ffd.FindNextFile();

				strcpy(fin.name, (const char *)ffd.GetFilePath());

				filearry.Add(fin);
			}
		}
		ffd.Close();

		*p = '\0';
		strcat(buff, "*.");		// ������һ����չ����
		strcat(buff, pin->irwp_desc_info.idi_synonym[count]);

	} while (count++ < (int)pin->irwp_desc_info.idi_synonym_count);

	if (filearry.GetSize())
	{
		cur_index = 0;
		init_mark = true;
	}

	return filearry.GetSize();
}


int cfiletab::disassemble()
{
	if (init_mark == false)
		return 1;			// �ļ���Ϣ�����ѱ��ֽ�
	
	filearry.RemoveAll();
	
	cur_index = 0;
	init_mark = false;
	
	return 0;
}


bool cfiletab::get_status()
{
	return init_mark;
}


int cfiletab::get_file_nums()
{
	return filearry.GetSize();
}


int cfiletab::get_prev_index()
{
	if (cur_index == 0)
		cur_index = filearry.GetUpperBound();
	else
		cur_index--;

	return cur_index;
}


int cfiletab::get_curr_index()
{
	return cur_index;
}


int cfiletab::get_next_index()
{
	if (cur_index == filearry.GetUpperBound())
		cur_index = 0;
	else
		cur_index++;
	
	return cur_index;
}


int cfiletab::set_curr_index(int index)
{
	ASSERT(index >= 0);
	ASSERT(index < filearry.GetSize());

	int i = cur_index;

	cur_index = index;

	return i;
}


char * cfiletab::get_file_name(int index)
{
	ASSERT(index < filearry.GetSize());

	return filearry[index].name;
}


FILEINFO * cfiletab::get_file_info(int index)
{
	ASSERT(index < filearry.GetSize());

	return (FILEINFO *)&(filearry.GetData()[index]);
}


/////////////////////////////////////////////////////////////////////////////
// PLUGIN_INFO class implement

PLUGIN_INFO::PLUGIN_INFO()
{
	status = false;
	plugin_handle = 0;
	memset((void*)plugin_name, 0, _MAX_PATH);
	plugin_info = 0;
}


PLUGIN_INFO::~PLUGIN_INFO()
{
	if (status)
	{
		plugin_info = 0;
		FreeLibrary(plugin_handle);
		status = false;
	}
}


// PLUGIN_INFO �ำֵ����������
PLUGIN_INFO& PLUGIN_INFO::operator= (PLUGIN_INFO& sou)
{
	// �����ֵǰ�����������������Ƚ�������
	if (this->status == true)
	{
		plugin_info = 0;
		FreeLibrary(plugin_handle);
		status = false;
	}

	plugin_handle = sou.plugin_handle;
	plugin_info   = sou.plugin_info;
	strcpy((char*)plugin_name, (const char *)sou.plugin_name);
	status = sou.status;

	// ��ֵ����Դ����ʧȥ����
	sou.plugin_handle = 0;
	sou.plugin_info   = 0;
	sou.status        = false;

	return *this;
}



/////////////////////////////////////////////////////////////////////////////
// cplugin class implement

cplugin::cplugin()
{
	init_mark = false;
	
	cur_plugin_index  = -1;
	p_cur_plugin_info = 0;
	piarray.SetSize(0, 32);
}


cplugin::~cplugin()
{
	if (get_status())
		destroy();
}


// ��ʼ���������ģ��
int cplugin::init(char *path)
{
	CFileFind	ffd;
	PLUGIN_INFO	pi;
	HMODULE		hmod;
	unsigned long isxp;
	BOOL		findmark;
	CString		fpath;

	// ��ֹ�ظ���ʼ��
	if (init_mark)
		return -1;

	strcpy(buff, path);
	if (buff[strlen(path)-1] != '\\')
		strcat(buff, "\\");
	strcat(buff, "*.dll");

	findmark = ffd.FindFile((LPCTSTR)buff, 0);

	// ��������DLL�ļ�
	while (findmark)
	{
		findmark = ffd.FindNextFile();

		fpath = ffd.GetFilePath();
		hmod = LoadLibrary((LPCSTR)fpath);
		if (hmod)
		{
			// �˳��ǲ��DLL
			if ((isxp=isirw_test_plugin(hmod)) != 0)
			{
				pi.plugin_info   = isirw_get_info(isxp);
				pi.plugin_handle = hmod;
				strcpy(pi.plugin_name, (const char *)ffd.GetFilePath());
				pi.status = true;

				piarray.Add(pi);

				pi.status = false;
			}
			else
				FreeLibrary(hmod);
		}
	}

	ffd.Close();

	if (piarray.GetSize())
	{
		cur_plugin_index = 0;
		p_cur_plugin_info = (piarray.GetData())[cur_plugin_index].plugin_info;
		init_mark = true;
	}

	return piarray.GetSize();		// �ҵ��Ĳ��ģ�����
}


// ���ٲ������ģ��
int cplugin::destroy()
{
	if (init_mark == false)
		return 1;			// ģ���Ѿ���������

	piarray.RemoveAll();

	cur_plugin_index = 0;
	p_cur_plugin_info = 0;
	init_mark = false;

	return 0;
}


// ��ò������ģ��ĵ�ǰ״̬
bool cplugin::get_status()
{
	return init_mark;
}


// ��ȡ�����Ϣ������׵�ַ
PLUGIN_INFO * cplugin::get_plugin_tab()
{
	return piarray.GetData();
}


// ��ȡ�������
int cplugin::get_plugin_nums()
{
	return piarray.GetSize();
}


// ��ȡ��ǰ�����Ϣ�ṹ�ĵ�ַ
LPIRWP_INFO cplugin::get_cur_plugin_info()
{
	return p_cur_plugin_info;
}


// ��ȡ��ǰ�������ֵ
int cplugin::get_cur_plugin_index()
{
	return cur_plugin_index;
}


// ���õ�ǰ���
int cplugin::set_cur_plugin_index(int index)
{
	if (index > piarray.GetUpperBound())
		return -1;							// ָ��������ֵ�Ƿ�

	int i = cur_plugin_index;

	cur_plugin_index  = index;
	p_cur_plugin_info = (piarray.GetData())[cur_plugin_index].plugin_info;

	return i;
}


// ��ȡ��ǰ�����ͼ����Ϣ������ַ
IRWP_API_GET_IMG_INFO cplugin::get_info_fun_addr()
{
	return (IRWP_API_GET_IMG_INFO)isirw_get_api_addr(piarray[cur_plugin_index].plugin_handle, piarray[cur_plugin_index].plugin_info, get_image_info);
}


// ��ȡ��ǰ�����ͼ�����ݺ�����ַ
IRWP_API_LOAD_IMG cplugin::get_load_fun_addr()
{
	return (IRWP_API_LOAD_IMG)isirw_get_api_addr(piarray[cur_plugin_index].plugin_handle, piarray[cur_plugin_index].plugin_info, load_image);
}


// ��ȡ��ǰ���дͼ�����ݺ�����ַ
IRWP_API_SAVE_IMG cplugin::get_save_fun_addr()
{
	return (IRWP_API_SAVE_IMG)isirw_get_api_addr(piarray[cur_plugin_index].plugin_handle, piarray[cur_plugin_index].plugin_info, save_image);
}




/////////////////////////////////////////////////////////////////////////////
// ctimer class implement

ctimer::ctimer()
{
	playmark = PLAYSTAT_STOP;
	timeout  = 1000;
	timer = 0;
}


ctimer::~ctimer()
{
	playmark = PLAYSTAT_STOP;
}


void ctimer::destroy(HWND hwnd)
{
	if (timer)
	{
		KillTimer(hwnd, IDT_TIMEOUT);
		timer = 0;
	}
}


int ctimer::play(HWND hwnd)
{
	if ((playmark == PLAYSTAT_PAUSE)||(playmark == PLAYSTAT_STOP))
	{
		if (!timer)
			timer = SetTimer(hwnd, IDT_TIMEOUT, timeout, NULL);

		playmark = PLAYSTAT_PLAY;

		return 0;
	}

	return 1;
}


int ctimer::pause()
{
	playmark = PLAYSTAT_PAUSE;

	return 0;
}


int ctimer::stop(HWND hwnd)
{
	if (timer)
	{
		KillTimer(hwnd, IDT_TIMEOUT);
		timer = 0;
	}
	playmark = PLAYSTAT_STOP;

	return 0;
}


PLAYSTAT ctimer::get_playmark()
{
	return playmark;
}


int ctimer::get_timeout()
{
	return timeout;
}


int ctimer::set_timeout(int new_timeout, HWND hwnd)
{
	int i = timeout;

	timeout = new_timeout;
	if (timer)
		KillTimer(hwnd, IDT_TIMEOUT);

	timer = ::SetTimer(hwnd, IDT_TIMEOUT, timeout, NULL);

	return i;
}



