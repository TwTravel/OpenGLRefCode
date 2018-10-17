// test.h : main header file for the TEST application
//

#if !defined(AFX_TEST_H__E3EA57C4_5C2C_11D5_8853_9341F819D4F2__INCLUDED_)
#define AFX_TEST_H__E3EA57C4_5C2C_11D5_8853_9341F819D4F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols



/////////////////////////////////////////////////////////////////////////////
// �������������ڲ��Բ�ͬ���ʱӦ����Ӧ�Ķ�
#define TESTLIB		"xpm.dll"				// ��ǰ���Ե�ͼ���дģ������
#define TESTFILE	"d:\\xpm\\*.xpm"		// ����ͼ��·��
#define TESTPIX		".xpm"					// ����ͼ���׺��



/////////////////////////////////////////////////////////////////////////////
// cfiletab:
// 

class FILEINFO
{
public:
	FILEINFO();
	~FILEINFO();

	FILEINFO& operator= (FILEINFO&);

	char name[_MAX_PATH];
	bool played;
	bool avail;
};


class cfiletab
{
public:
	cfiletab();
	~cfiletab();

	int compages(char *path, LPIRWP_INFO pin);
	int disassemble(void);

	bool get_status(void);
	int get_file_nums(void);

	int get_prev_index(void);
	int get_curr_index(void);
	int get_next_index(void);

	int set_curr_index(int index);

	char * get_file_name(int index);
	FILEINFO *get_file_info(int index);

protected:
	bool			init_mark;					// ��ʼ�����
	
	int				cur_index;					// ��ǰ�ļ�����
	CArray <FILEINFO, FILEINFO&>	filearry;	// �ļ���Ϣ����
	
	char			buff[_MAX_PATH];
};


/////////////////////////////////////////////////////////////////////////////
// cplugin:
// 

class PLUGIN_INFO
{
public:
	PLUGIN_INFO();
	~PLUGIN_INFO();

	PLUGIN_INFO& operator= (PLUGIN_INFO&);

	bool		status;
	HMODULE		plugin_handle;
	char		plugin_name[_MAX_PATH];
	LPIRWP_INFO plugin_info;
};


class cplugin
{
public:
	cplugin();
	~cplugin();

	int init(char *path);
	int destroy(void);

	bool get_status(void);
	int  get_plugin_nums(void);
	LPIRWP_INFO	get_cur_plugin_info(void);
	PLUGIN_INFO *get_plugin_tab(void);

	int  get_cur_plugin_index(void);
	int  set_cur_plugin_index(int index);

	IRWP_API_GET_IMG_INFO get_info_fun_addr(void);
	IRWP_API_LOAD_IMG     get_load_fun_addr(void);
	IRWP_API_SAVE_IMG     get_save_fun_addr(void);

protected:
	bool			init_mark;						// ��ʼ�����

	int				cur_plugin_index;				// ��ǰ�������
	LPIRWP_INFO		p_cur_plugin_info;				// ��ǰ�����Ϣָ��
	CArray <PLUGIN_INFO, PLUGIN_INFO&>	piarray;	// �������

	char			buff[_MAX_PATH];
};



/////////////////////////////////////////////////////////////////////////////
// ctimer:
// 

typedef enum _tagPlayStat
{
	PLAYSTAT_STOP,			// ֹͣ����
	PLAYSTAT_PAUSE,			// ��ͣ����
	PLAYSTAT_PLAY
} PLAYSTAT, *LPPLAYSTAT;

class ctimer
{
public:
	ctimer();
	~ctimer();

	int play(HWND hwnd);
	int pause(void);
	int stop(HWND hwnd);

	int get_timeout(void);
	int set_timeout(int new_timeout, HWND hwnd);

	PLAYSTAT get_playmark(void);

	void destroy(HWND hwnd);

protected:
	PLAYSTAT	playmark;

	int			timeout;
	UINT		timer;
};




/////////////////////////////////////////////////////////////////////////////
// CTestApp:
// See test.cpp for the implementation of this class
//

class CTestApp : public CWinApp
{
public:
	CTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CTestApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEST_H__E3EA57C4_5C2C_11D5_8853_9341F819D4F2__INCLUDED_)
