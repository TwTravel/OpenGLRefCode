/**********************************************************************
//							临风多媒体类库 ringDib.lib
//作者：临风
//
//版本：0.1
//
//声明：本类库可以自由使用而不须对作者作出任何回报，但作者希望能得到
//		  你的鼓励和支持。你可以对类库源码作出修改和改进，但希望你能在
//		  修改的同时给作者一份同样的副本。
//		  本类库不得用于任何商业用途，如确实需要，请与作者联系。
//
//e-mail:ringphone@sina.com
//
//原文件名：ringdib.h
//
//本文件说明：ringDib.lib的头文件，使用该类库必须包含本文件
//
**********************************************************************/

#ifndef _INC_WINDOWS
#include <windows.h>
#endif

#include <mmsystem.h>
#include <math.h>
#include <vfw.h>

//使用户无须在IDE中加入"ringdib.lib",当"ringdib.lib"放置路径不同时须修改"#pragma"中指定路径
//#ifndef MAKE_SELF_LIB		//当编译"ringdib.lib"自身时，不须自联接"ringdib.lib"
//#pragma comment(lib, "d:\\develop\\bc5\\lib\\user\\ringdib.lib")
//#endif

#define RC_BMP				0x730001	//资源类型：BMP
#define RC_DATA         0x730002	//资源类型：数据

#define DIB_BMP		6600000
#define DIB_PCX		6600001
#define DIB_JPG		6600002
#define DIB_GIF		6600003
#define DIB_MIX		6600004

#define FAIL_SUCCESS	8800001
#define FAIL_OTHER	8800002

typedef struct tagTRUECOLOR{
	BYTE r;
   BYTE g;
   BYTE b;
}RGBCOLOR,*LPRGBCOLOR;

typedef struct tagAniGifImage{		//动画GIF图象信息
	tagAniGifImage* prev;                //上一幅图象信息
   COLORREF* curr;                  //当前图象数据
   tagAniGifImage* next;                //下一幅图象信息
   BYTE docase;							//图象处理方法
   WORD delay;								//图象延时
   COLORREF keycolor;							//透明色索引
   int x,y,width,height;
}ANIGIFPIC,*LPANIGIFPIC;

extern HINSTANCE r_hInst;
extern HWND r_hWnd;
extern int actFlag;				//记录函数执行错误

extern "C" {
void InitRingDIB(HINSTANCE hInstance,HWND hWnd);
BOOL CreateBack(int x,int y);
BOOL DrawBack(void);
BOOL PaintBack(void);
void QuitRingDIB(void);
LPVOID New(DWORD);
LPVOID Del(LPVOID);
int FAIL(void);
}

class RDib;
class RDraw;

extern RDraw _rDraw;
extern RDib* _rBack;

class RDraw
{
public:
	RDraw();
   ~RDraw();

   void Init();													//初始化，构造函数调用;
   void Destroy();												//析构函数调用;

   HDRAWDIB Open();
	BOOL Close ();

   BOOL Draw(RDib* rdib,UINT wFlags=DDF_HALFTONE);
	BOOL Paint(RDib* rdib,UINT wFlags=DDF_HALFTONE);
   BOOL DrawTo(RDib* src,RDib* dest,BOOL bTrans=TRUE);
   BOOL DrawBack(RDib* rdib,BOOL bTrans=TRUE);
   BOOL CopyToBack(RDib* rdib);

public:
	HDRAWDIB m_hDrawDib;
   BITMAPINFO m_bmif;
};

class RDib
{
public:
	RDib();
   ~RDib();

   void Destroy();

   BOOL Create(int x,int y);
   BOOL Load(LPSTR szPicFilename);

	BOOL LoadGif(HANDLE hFile);
	BOOL LoadBmp(HANDLE hFile,LPSTR szFilename);
	BOOL LoadPcx(HANDLE hFile);
	BOOL LoadJPG(LPSTR szFilename);
//	BOOL LoadHH(HANDLE hFile);

   void SetSrc(int x,int y,int width=0,int height=0);
   void SetDest(int x,int y,int width=0,int height=0);
   void SetColorKey(COLORREF crkey){m_crKey=crkey;};
   void Fill(COLORREF crColor);
   void Fill(BYTE r,BYTE g,BYTE b);

   BOOL Draw(UINT wFlags=DDF_HALFTONE);
   BOOL Paint(UINT wFlags=DDF_HALFTONE);
   BOOL DrawTo(RDib* dest,BOOL bTrans=TRUE);
   BOOL DrawBack(BOOL bTrans=TRUE);
   BOOL CopyToBack();
   
   BOOL GrayScale();

public:
	int m_width,m_height,m_cx,m_cy;
   RECT src,dest;
   COLORREF m_crKey;
   COLORREF* m_Bits;
   LPANIGIFPIC m_aniGifPic;
};


