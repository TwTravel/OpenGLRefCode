/**********************************************************************
//							�ٷ��ý����� ringDib.lib
//���ߣ��ٷ�
//
//�汾��0.1
//
//������������������ʹ�ö���������������κλر���������ϣ���ܵõ�
//		  ��Ĺ�����֧�֡�����Զ����Դ�������޸ĺ͸Ľ�����ϣ��������
//		  �޸ĵ�ͬʱ������һ��ͬ���ĸ�����
//		  ����ⲻ�������κ���ҵ��;����ȷʵ��Ҫ������������ϵ��
//
//e-mail:ringphone@sina.com
//
//ԭ�ļ�����ringdib.h
//
//���ļ�˵����ringDib.lib��ͷ�ļ���ʹ�ø�������������ļ�
//
**********************************************************************/

#ifndef _INC_WINDOWS
#include <windows.h>
#endif

#include <mmsystem.h>
#include <math.h>
#include <vfw.h>

//ʹ�û�������IDE�м���"ringdib.lib",��"ringdib.lib"����·����ͬʱ���޸�"#pragma"��ָ��·��
//#ifndef MAKE_SELF_LIB		//������"ringdib.lib"����ʱ������������"ringdib.lib"
//#pragma comment(lib, "d:\\develop\\bc5\\lib\\user\\ringdib.lib")
//#endif

#define RC_BMP				0x730001	//��Դ���ͣ�BMP
#define RC_DATA         0x730002	//��Դ���ͣ�����

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

typedef struct tagAniGifImage{		//����GIFͼ����Ϣ
	tagAniGifImage* prev;                //��һ��ͼ����Ϣ
   COLORREF* curr;                  //��ǰͼ������
   tagAniGifImage* next;                //��һ��ͼ����Ϣ
   BYTE docase;							//ͼ������
   WORD delay;								//ͼ����ʱ
   COLORREF keycolor;							//͸��ɫ����
   int x,y,width,height;
}ANIGIFPIC,*LPANIGIFPIC;

extern HINSTANCE r_hInst;
extern HWND r_hWnd;
extern int actFlag;				//��¼����ִ�д���

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

   void Init();													//��ʼ�������캯������;
   void Destroy();												//������������;

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


