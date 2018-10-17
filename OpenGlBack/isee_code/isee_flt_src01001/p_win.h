/********************************************************************

	p_win.h - ISee图像浏览器—图像处理模块实现代码头文件

    版权所有(C) VCHelp-coPathway-ISee workgroup 2000 all member's

    这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用许可证
	条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根据你
	的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用许可证。

    你应该已经和程序一起收到一份GNU通用许可证(GPL)的副本。如果还没有，
	写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	如果你在使用本软件时有什么问题或建议，用以下地址可以与我们取得联
	系：
		http://isee.126.com
		http://www.vchelp.net
	或：
		iseesoft@china.com

	作者：临风
   e-mail:ringphone@sina.com

   功能实现：全局函数定义

	文件版本：
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

