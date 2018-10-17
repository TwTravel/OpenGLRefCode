/********************************************************************

	flt_anieff.cpp - ISee图像浏览器—图像处理模块类CFlt_anieff实现代码

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

	作者：楼琇林(leye)
   e-mail:louxiulin@263.net

   功能实现：对外接口函数

	文件版本：
		Build 010317
		Date  2001-03-17

********************************************************************/
// alt_anieff.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "flt_anieff.h"
#include "AnieffConfigDlg.h"
#include <windowsx.h>
#include <algorithm>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CFlt_anieffApp

BEGIN_MESSAGE_MAP(CFlt_anieffApp, CWinApp)
	//{{AFX_MSG_MAP(CFlt_anieffApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlt_anieffApp construction

CFlt_anieffApp::CFlt_anieffApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFlt_anieffApp object

CFlt_anieffApp theApp;

// 接口函数声明 — 第一层，唯一与外界联系的接口
///////////////
int WINAPI AccessAniEffFilter(LPIMAGEPROCSTR lpInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(lpInfo==NULL)
   		return PROCERR_NULLPARAM;

	//该模块只进行设置参数和运行工作
	if((lpInfo->state!=IPDPS_CONFIG) && (lpInfo->state!=IPDPS_RUN))
	{
		lpInfo->result=PR_NULL;
		return PROCERR_FALSE;
	}

	if(lpInfo->state==IPDPS_CONFIG)		//动态效果参数设置
	{
		CAnieffConfigDlg dlg;
		if(IDOK!=dlg.DoModal())
		{
			lpInfo->result=PR_NULL;
			return PROCERR_FALSE;
		}
		//动态效果参数设置
		CFlt_anieffApp* pApp = (CFlt_anieffApp*)AfxGetApp();
		pApp->WriteProfileInt("PCM_ANIEFF","PCM_ALPHASCROLL",dlg.m_bAlphaScroll);
		pApp->WriteProfileInt("PCM_ANIEFF","PCM_BURNUP",dlg.m_bBurnUp);
		pApp->WriteProfileInt("PCM_ANIEFF","PCM_FADEIN",dlg.m_bFadeIn);
		pApp->WriteProfileInt("PCM_ANIEFF","PCM_lINEUP",dlg.m_bLineUp);
		pApp->WriteProfileInt("PCM_ANIEFF","DELAYTIME",dlg.m_nMilliSec);
		pApp->WriteProfileInt("PCM_ANIEFF","FRAMES",dlg.m_nFrames);
		lpInfo->result=PR_SUCCESS;
		return PROCERR_SUCCESS;
	}
	else if(lpInfo->state==IPDPS_RUN)	//运行动态效果
	{
		switch(lpInfo->comm)
		{
   			case PCM_NULL:              //未操作
      			lpInfo->result=PR_NULL;
      			return PROCERR_FALSE;

			case PCM_ALPHASCROLL:		// ALPHA卷轴
				return _fnCMD_ALPHASCROLL(lpInfo);

			case PCM_BURNUP:			// 火焰吞噬
				return _fnCMD_BURNUP(lpInfo);

			case PCM_FADEIN:			// 叠映
				return _fnCMD_FADEIN(lpInfo);

			case PCM_LINEUP:			// 随机直线填充
				return _fnCMD_LINEUP(lpInfo);
		}
		lpInfo->result=PR_ILLCOMM;
	}

	return PROCERR_FALSE;
	// normal function body here
}

// 调用处理函数 — 第二层函数
//  ALPHA卷轴——下一张图片从上下左右四个方向的某个方向卷轴进来
int _fnCMD_ALPHASCROLL(LPIMAGEPROCSTR lpInfo)
{
	lpInfo->result=PR_SUCCESS;
    return PROCERR_SUCCESS;
}

//  火焰吞噬——前图片从下到上被火焰吞噬，显出下一张图片
int _fnCMD_BURNUP(LPIMAGEPROCSTR lpInfo)
{
	lpInfo->result=PR_SUCCESS;
    return PROCERR_SUCCESS;
}

//  叠映——淡入淡出效果，当前图片淡出，下一张图片淡入
int _fnCMD_FADEIN(LPIMAGEPROCSTR lpInfo)
{
	// 检验入口参数是否符合接口定义
	ASSERT(lpInfo->_psbdata != NULL);
	ASSERT(lpInfo->_pdbdata != NULL);
	ASSERT(lpInfo->sImageInfo.bitperpix==ISEE_IMAGE_BITCOUNT);
	ASSERT(lpInfo->dImageInfo.bitperpix==ISEE_IMAGE_BITCOUNT);
	if(lpInfo->sImageInfo.width!=lpInfo->dImageInfo.width || 
	    lpInfo->sImageInfo.height!=lpInfo->dImageInfo.height)
	{
		lpInfo->result=PR_COMMINFOERR;
		return PROCERR_FALSE;
	}
	// 绘图参数设置
	CRect* pRc=(CRect*)lpInfo->annexdata.pAnnData;
	HDC hDC=::GetDC(lpInfo->hParentWnd);
	BITMAPINFOHEADER	bmi=
	{
		sizeof(BITMAPINFOHEADER),
		lpInfo->sImageInfo.width,
		lpInfo->sImageInfo.height,
		1,//biPlanes
		lpInfo->dImageInfo.bitperpix,//biBitCount
		BI_RGB,//biCompression
		0,//biSizeImage
		0,
		0,
		0,
		0
	};
	const height=lpInfo->sImageInfo.height;
	const width=lpInfo->sImageInfo.width;
	const byteperpix=ISEE_IMAGE_BITCOUNT/8;
	//定义临时数据，用于存放处理过程中的过渡数据，也是要显示的数据
	LPBYTE pTmpData=(LPBYTE)GlobalAllocPtr(GHND,height*width*4);
	if(pTmpData==NULL) 
	{
		lpInfo->result=PR_MEMORYERR;
		return PROCERR_FALSE;
	}
	memcpy(pTmpData,lpInfo->_psbdata,height*width*4);
	//获取动态效果的延时(毫秒)
	UINT nDelay=::AfxGetApp()->GetProfileInt("PCM_ANIEFF","DELAYTIME",2000);
	//获取动画帧数
	UINT nFrames=::AfxGetApp()->GetProfileInt("PCM_ANIEFF","FRAMES",24);
	const UINT DPF=nDelay/(nFrames-1);//帧间延时(毫秒)

	int i,pos,step;
	DWORD dwTimeBegin,dwTimeEnd;
	const float F=(256-1)/float(nFrames);//帧增量
	//打开绘图环境
	HDRAWDIB hdd = DrawDibOpen();
	if(hdd==NULL) 
	{
		lpInfo->result=PR_FAIL;
		return PROCERR_FALSE;
	}
	//////////////
	for(i=1;i<=nFrames;i++)
	{
		step=int(F*i+0.5);
		dwTimeBegin=timeGetTime();//开始记时
		for ( pos=0; pos<height*(width*byteperpix); pos++)
			pTmpData[pos] =(pTmpData[pos]*(255-step)+lpInfo->_pdbdata[pos]*step)/255;
		DrawDibDraw(hdd,
					hDC,
					pRc->left,                 // DestX
					pRc->top,                  // DestY
					pRc->Width(),            // nDestWidth
					pRc->Height(),           // nDestHeight
					&bmi,         // lpBitsInfo
					pTmpData,                      // lpBits
					0,                // SrcX
					0,                 // SrcY
					bmi.biWidth,           // wSrcWidth
					bmi.biHeight,          // wSrcHeight
					DDF_DONTDRAW); 
		//延时代码部分
		dwTimeEnd=timeGetTime();//结束记时
		if(DPF>dwTimeEnd-dwTimeBegin)
		Delay(DPF-(dwTimeEnd-dwTimeBegin));
	}
	//关闭绘图环境
	DrawDibClose(hdd);

	//释放临时变量内存
	GlobalFreePtr(pTmpData);
	pTmpData=NULL;
	//释放设备环境
	ReleaseDC(lpInfo->hParentWnd,hDC);
	//原始数据没有改变
	lpInfo->modify=0;
	lpInfo->result=PR_SUCCESS;
    return PROCERR_SUCCESS;
}

// 随机直线填充——前图片被随机线填充，显出下一张图片
int _fnCMD_LINEUP(LPIMAGEPROCSTR lpInfo)
{
	// 检验入口参数是否符合接口定义
	ASSERT(lpInfo->_psbdata != NULL);
	ASSERT(lpInfo->_pdbdata != NULL);
	ASSERT(lpInfo->sImageInfo.bitperpix==ISEE_IMAGE_BITCOUNT);
	ASSERT(lpInfo->dImageInfo.bitperpix==ISEE_IMAGE_BITCOUNT);
	if(lpInfo->sImageInfo.width!=lpInfo->dImageInfo.width || 
	    lpInfo->sImageInfo.height!=lpInfo->dImageInfo.height)
	{
		lpInfo->result=PR_COMMINFOERR;
		return PROCERR_FALSE;
	}
	// 绘图参数设置
	CRect* pRc=(CRect*)lpInfo->annexdata.pAnnData;
	HDC hDC=::GetDC(lpInfo->hParentWnd);
	BITMAPINFOHEADER	bmi=
	{
		sizeof(BITMAPINFOHEADER),
		lpInfo->sImageInfo.width,
		lpInfo->sImageInfo.height,
		1,//biPlanes
		lpInfo->dImageInfo.bitperpix,//biBitCount
		BI_RGB,//biCompression
		0,//biSizeImage
		0,
		0,
		0,
		0
	};
	const height=lpInfo->sImageInfo.height;
	const width=lpInfo->sImageInfo.width;
	const byteperpix=ISEE_IMAGE_BITCOUNT/8;
	//设置随机线数组并初始化
	int* line=(int*)GlobalAllocPtr(GHND,height*sizeof(int));
	int* colum=(int*)GlobalAllocPtr(GHND,width*sizeof(int));
	for(int i=0;i<height;i++) line[i]=i;
	for(i=0;i<width;i++) colum[i]=i;
	random_shuffle(line, line+height);//使顺序数据随机排列
	random_shuffle(colum, colum+width);//使顺序数据随机排列
	//定义临时数据，用于存放处理过程中的过渡数据，也是要显示的数据
	LPBYTE pTmpData=(LPBYTE)GlobalAllocPtr(GHND,height*width*4);
	if(pTmpData==NULL) 
	{
		lpInfo->result=PR_MEMORYERR;
		return PROCERR_FALSE;
	}
	memcpy(pTmpData,lpInfo->_psbdata,height*width*4);
	//获取动态效果的延时(毫秒)
	UINT nDelay=::AfxGetApp()->GetProfileInt("PCM_ANIEFF","DELAYTIME",2000);
	//获取动画帧数
	UINT nFrames=::AfxGetApp()->GetProfileInt("PCM_ANIEFF","FRAMES",24);
	const UINT DPF=nDelay/(nFrames-1);//帧间延时(毫秒)
	const float FUD=(height-1)/float(nFrames);//水平线
	const float FLR=(width-1)/float(nFrames);//垂直线
	DWORD dwTimeBegin,dwTimeEnd;
	int x,y,col,row,step;
	//////////////////
	static format=0;//随机线类型：水平、垂直或两者
	srand(timeGetTime());
	format=rand()%3;
	//打开绘图环境
	HDRAWDIB hdd = DrawDibOpen();
	if(hdd==NULL) 
	{
		lpInfo->result=PR_FAIL;
		return PROCERR_FALSE;
	}
	/////////////////
	for(step=1;step<=nFrames;step++)
	{
		dwTimeBegin=timeGetTime();//开始记时
		switch(format)
		{
		case 0://水平线
			y=int(FUD*step+0.5);
			for(row=0;row<=y;row++)
				memcpy(&pTmpData[line[row]*width*byteperpix],&lpInfo->_pdbdata[line[row]*width*byteperpix],width*byteperpix);
			break;
		case 1://垂直线
			x=int(FLR*step+0.5);
			for(col=0;col<=x;col++)
				for(row=0;row<height;row++)
					memcpy(&pTmpData[row*width*byteperpix+colum[col]*byteperpix],&lpInfo->_pdbdata[row*width*byteperpix+colum[col]*byteperpix],byteperpix);
			break;
		case 2://水平线和垂直线
			y=int(FUD*step+0.5);
			x=int(FLR*step+0.5);
			for(row=0;row<=y;row++)
				memcpy(&pTmpData[line[row]*width*byteperpix],&lpInfo->_pdbdata[line[row]*width*byteperpix],width*byteperpix);
			for(col=0;col<=x;col++)
				for(row=0;row<height;row++)
					memcpy(&pTmpData[row*width*byteperpix+colum[col]*byteperpix],&lpInfo->_pdbdata[row*width*byteperpix+colum[col]*byteperpix],byteperpix);
			break;
		}
		DrawDibDraw(hdd,
					hDC,
					pRc->left,                 // DestX
					pRc->top,                  // DestY
					pRc->Width(),            // nDestWidth
					pRc->Height(),           // nDestHeight
					&bmi,         // lpBitsInfo
					pTmpData,                      // lpBits
					0,                // SrcX
					0,                 // SrcY
					bmi.biWidth,           // wSrcWidth
					bmi.biHeight,          // wSrcHeight
					DDF_DONTDRAW); 
		//延时代码部分
		dwTimeEnd=timeGetTime();//结束记时
		if(DPF>dwTimeEnd-dwTimeBegin)
		Delay(DPF-(dwTimeEnd-dwTimeBegin));
	}
	//关闭绘图环境
	DrawDibClose(hdd);

	//释放临时变量内存
	GlobalFreePtr(line);
	line=NULL;
	GlobalFreePtr(colum);
	colum=NULL;
	GlobalFreePtr(pTmpData);
	pTmpData=NULL;
	//释放设备环境
	ReleaseDC(lpInfo->hParentWnd,hDC);
	//原始数据没有改变
	lpInfo->modify=0;
	lpInfo->result=PR_SUCCESS;
    return PROCERR_SUCCESS;
}


//////////辅助函数
//	延时函数——作者：马翔
//  e-mail:hugesoft@yeah.net
void Delay(DWORD dwDelayTime)
{
	DWORD dwTimeBegin,dwTimeEnd;
	dwTimeBegin=timeGetTime();
	do
	{
		dwTimeEnd=timeGetTime();
	}while(dwTimeEnd-dwTimeBegin<dwDelayTime);
}
