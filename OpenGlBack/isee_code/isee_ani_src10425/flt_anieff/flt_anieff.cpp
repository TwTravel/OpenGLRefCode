/********************************************************************

	flt_anieff.cpp - ISeeͼ���������ͼ����ģ����CFlt_anieffʵ�ִ���

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

   ����ʵ�֣�����ӿں���

	�ļ��汾��
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

// �ӿں������� �� ��һ�㣬Ψһ�������ϵ�Ľӿ�
///////////////
int WINAPI AccessAniEffFilter(LPIMAGEPROCSTR lpInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(lpInfo==NULL)
   		return PROCERR_NULLPARAM;

	//��ģ��ֻ�������ò��������й���
	if((lpInfo->state!=IPDPS_CONFIG) && (lpInfo->state!=IPDPS_RUN))
	{
		lpInfo->result=PR_NULL;
		return PROCERR_FALSE;
	}

	if(lpInfo->state==IPDPS_CONFIG)		//��̬Ч����������
	{
		CAnieffConfigDlg dlg;
		if(IDOK!=dlg.DoModal())
		{
			lpInfo->result=PR_NULL;
			return PROCERR_FALSE;
		}
		//��̬Ч����������
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
	else if(lpInfo->state==IPDPS_RUN)	//���ж�̬Ч��
	{
		switch(lpInfo->comm)
		{
   			case PCM_NULL:              //δ����
      			lpInfo->result=PR_NULL;
      			return PROCERR_FALSE;

			case PCM_ALPHASCROLL:		// ALPHA����
				return _fnCMD_ALPHASCROLL(lpInfo);

			case PCM_BURNUP:			// ��������
				return _fnCMD_BURNUP(lpInfo);

			case PCM_FADEIN:			// ��ӳ
				return _fnCMD_FADEIN(lpInfo);

			case PCM_LINEUP:			// ���ֱ�����
				return _fnCMD_LINEUP(lpInfo);
		}
		lpInfo->result=PR_ILLCOMM;
	}

	return PROCERR_FALSE;
	// normal function body here
}

// ���ô����� �� �ڶ��㺯��
//  ALPHA���ᡪ����һ��ͼƬ�����������ĸ������ĳ������������
int _fnCMD_ALPHASCROLL(LPIMAGEPROCSTR lpInfo)
{
	lpInfo->result=PR_SUCCESS;
    return PROCERR_SUCCESS;
}

//  �������ɡ���ǰͼƬ���µ��ϱ��������ɣ��Գ���һ��ͼƬ
int _fnCMD_BURNUP(LPIMAGEPROCSTR lpInfo)
{
	lpInfo->result=PR_SUCCESS;
    return PROCERR_SUCCESS;
}

//  ��ӳ�������뵭��Ч������ǰͼƬ��������һ��ͼƬ����
int _fnCMD_FADEIN(LPIMAGEPROCSTR lpInfo)
{
	// ������ڲ����Ƿ���Ͻӿڶ���
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
	// ��ͼ��������
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
	//������ʱ���ݣ����ڴ�Ŵ�������еĹ������ݣ�Ҳ��Ҫ��ʾ������
	LPBYTE pTmpData=(LPBYTE)GlobalAllocPtr(GHND,height*width*4);
	if(pTmpData==NULL) 
	{
		lpInfo->result=PR_MEMORYERR;
		return PROCERR_FALSE;
	}
	memcpy(pTmpData,lpInfo->_psbdata,height*width*4);
	//��ȡ��̬Ч������ʱ(����)
	UINT nDelay=::AfxGetApp()->GetProfileInt("PCM_ANIEFF","DELAYTIME",2000);
	//��ȡ����֡��
	UINT nFrames=::AfxGetApp()->GetProfileInt("PCM_ANIEFF","FRAMES",24);
	const UINT DPF=nDelay/(nFrames-1);//֡����ʱ(����)

	int i,pos,step;
	DWORD dwTimeBegin,dwTimeEnd;
	const float F=(256-1)/float(nFrames);//֡����
	//�򿪻�ͼ����
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
		dwTimeBegin=timeGetTime();//��ʼ��ʱ
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
		//��ʱ���벿��
		dwTimeEnd=timeGetTime();//������ʱ
		if(DPF>dwTimeEnd-dwTimeBegin)
		Delay(DPF-(dwTimeEnd-dwTimeBegin));
	}
	//�رջ�ͼ����
	DrawDibClose(hdd);

	//�ͷ���ʱ�����ڴ�
	GlobalFreePtr(pTmpData);
	pTmpData=NULL;
	//�ͷ��豸����
	ReleaseDC(lpInfo->hParentWnd,hDC);
	//ԭʼ����û�иı�
	lpInfo->modify=0;
	lpInfo->result=PR_SUCCESS;
    return PROCERR_SUCCESS;
}

// ���ֱ����䡪��ǰͼƬ���������䣬�Գ���һ��ͼƬ
int _fnCMD_LINEUP(LPIMAGEPROCSTR lpInfo)
{
	// ������ڲ����Ƿ���Ͻӿڶ���
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
	// ��ͼ��������
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
	//������������鲢��ʼ��
	int* line=(int*)GlobalAllocPtr(GHND,height*sizeof(int));
	int* colum=(int*)GlobalAllocPtr(GHND,width*sizeof(int));
	for(int i=0;i<height;i++) line[i]=i;
	for(i=0;i<width;i++) colum[i]=i;
	random_shuffle(line, line+height);//ʹ˳�������������
	random_shuffle(colum, colum+width);//ʹ˳�������������
	//������ʱ���ݣ����ڴ�Ŵ�������еĹ������ݣ�Ҳ��Ҫ��ʾ������
	LPBYTE pTmpData=(LPBYTE)GlobalAllocPtr(GHND,height*width*4);
	if(pTmpData==NULL) 
	{
		lpInfo->result=PR_MEMORYERR;
		return PROCERR_FALSE;
	}
	memcpy(pTmpData,lpInfo->_psbdata,height*width*4);
	//��ȡ��̬Ч������ʱ(����)
	UINT nDelay=::AfxGetApp()->GetProfileInt("PCM_ANIEFF","DELAYTIME",2000);
	//��ȡ����֡��
	UINT nFrames=::AfxGetApp()->GetProfileInt("PCM_ANIEFF","FRAMES",24);
	const UINT DPF=nDelay/(nFrames-1);//֡����ʱ(����)
	const float FUD=(height-1)/float(nFrames);//ˮƽ��
	const float FLR=(width-1)/float(nFrames);//��ֱ��
	DWORD dwTimeBegin,dwTimeEnd;
	int x,y,col,row,step;
	//////////////////
	static format=0;//��������ͣ�ˮƽ����ֱ������
	srand(timeGetTime());
	format=rand()%3;
	//�򿪻�ͼ����
	HDRAWDIB hdd = DrawDibOpen();
	if(hdd==NULL) 
	{
		lpInfo->result=PR_FAIL;
		return PROCERR_FALSE;
	}
	/////////////////
	for(step=1;step<=nFrames;step++)
	{
		dwTimeBegin=timeGetTime();//��ʼ��ʱ
		switch(format)
		{
		case 0://ˮƽ��
			y=int(FUD*step+0.5);
			for(row=0;row<=y;row++)
				memcpy(&pTmpData[line[row]*width*byteperpix],&lpInfo->_pdbdata[line[row]*width*byteperpix],width*byteperpix);
			break;
		case 1://��ֱ��
			x=int(FLR*step+0.5);
			for(col=0;col<=x;col++)
				for(row=0;row<height;row++)
					memcpy(&pTmpData[row*width*byteperpix+colum[col]*byteperpix],&lpInfo->_pdbdata[row*width*byteperpix+colum[col]*byteperpix],byteperpix);
			break;
		case 2://ˮƽ�ߺʹ�ֱ��
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
		//��ʱ���벿��
		dwTimeEnd=timeGetTime();//������ʱ
		if(DPF>dwTimeEnd-dwTimeBegin)
		Delay(DPF-(dwTimeEnd-dwTimeBegin));
	}
	//�رջ�ͼ����
	DrawDibClose(hdd);

	//�ͷ���ʱ�����ڴ�
	GlobalFreePtr(line);
	line=NULL;
	GlobalFreePtr(colum);
	colum=NULL;
	GlobalFreePtr(pTmpData);
	pTmpData=NULL;
	//�ͷ��豸����
	ReleaseDC(lpInfo->hParentWnd,hDC);
	//ԭʼ����û�иı�
	lpInfo->modify=0;
	lpInfo->result=PR_SUCCESS;
    return PROCERR_SUCCESS;
}


//////////��������
//	��ʱ�����������ߣ�����
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
