/********************************************************************

	p_win.h - ISeeͼ���������ͼ����ģ��ʵ�ִ���ͷ�ļ�

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

	���ߣ��ٷ�
   e-mail:ringphone@sina.com

   ����ʵ�֣�ȫ�ֺ�������

	�ļ��汾��
		Build 00617
		Date  2000-6-17

********************************************************************/

#ifndef _INC_WINDOWS
#include <windows.h>
#endif

#define THREADPROC		LPTHREAD_START_ROUTINE

#define WIN9X				1995
#define NT					1994
#define WIN3X				1993

#define MAX_X	GetSystemMetrics(SM_CXSCREEN)
#define MAX_Y	GetSystemMetrics(SM_CYSCREEN)

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif	/* __cplusplus */
extern int ClientX(int x);
extern int ClientY(int y);

extern void Errmsg(LPSTR format,...);
extern void ShowHelpInfo(HINSTANCE hInst,LPSTR szText,LPSTR szCaption,WORD nIconID);
extern void PaintNull(HWND hWnd);
extern int GetFileName(HINSTANCE hInst,HWND hWnd,LPSTR szFileType,LPSTR szFileName,LPSTR szTitle);
extern HANDLE Open(LPSTR szFilename,DWORD dwCreate);
extern HANDLE OpenForRead(LPSTR szFilename,DWORD dwCreate);
extern BOOL FailOpen(HANDLE hFile);
extern int OSType(void);

extern BOOL SaveBmp(HBITMAP hbm,LPSTR szFilename);
extern BOOL GetBmpSize(HBITMAP hbm,SIZE *size);
extern BOOL Read(HANDLE hFile,LPVOID lpBuf,DWORD dwNum);
extern BOOL Write(HANDLE hFile,LPVOID lpBuf,DWORD dwNum);

extern DWORD GetFilePointer(HANDLE hFile);
extern HANDLE NewThread(LPTHREAD_START_ROUTINE lpStartAddress,LPDWORD lpThreadId);
extern int WinWidth(HWND hWnd);
extern int WinHeight(HWND hWnd);

extern LPVOID New(DWORD dwSize);
extern LPVOID Del(LPVOID lpv);

#ifdef __cplusplus
}
#endif	/* __cplusplus */

