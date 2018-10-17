/********************************************************************

	BmpModule.cpp - ISeeͼ���������BMPͼ���дģ��ʵ���ļ�
          
    ��Ȩ����(C) 2000 VCHelp-coPathway-ISee workgroup member

    ��һ����������������������������������������GNU ͨ�ù�����
	��֤�������޸ĺ����·�����һ���򡣻��������֤�ĵڶ��棬���ߣ���
	�����ѡ�����κθ��µİ汾��

    ������һ�����Ŀ����ϣ�������ã���û���κε���������û���ʺ��ض�
	Ŀ�ص������ĵ���������ϸ����������GNUͨ�ù������֤��

    ��Ӧ���Ѿ��ͳ���һ���յ�һ��GNUͨ�ù������֤�ĸ����������û�У�
	д�Ÿ���
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	�������ʹ�ñ����ʱ��ʲô������飬�����µ�ַ����������ȡ����
	ϵ��
		http://isee.126.com
		http://www.vchelp.net
	��
		iseesoft@china.com

	��д�ˣ�YZ
	E-Mail��yzfree@sina.com

	�ļ��汾��
		Beta  1.5
		Build 01209
		Date  2000-12-9

********************************************************************/

#include "stdafx.h"
#include "..\public\gol_isee.h"	// ���ļ������˽ӿ����ݰ�
#include "BmpModule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char ModuleProcessImgType[]="BMP,DIB,RLE";		// ��ģ���ܴ����ͼ������
static char WriterList[]="YZ";							// ��ģ��������б�
static char WriterMess[]="û�뵽����BMPģ�黹��ͦ����^_^";	// ��������

static int  RWPROGRESSSIZE;
static const int RWPROGRESSSTEP = 10;

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

//		��ͼ���дģ���У����������ڴ棬��ʹ��API����GlobalAlloc()
//		��������ͷ��ڴ���ʹ��GlobalFree()��������Ҫʹ�����磺new
//		��malloc()�Ⱥ���������Ϊ��ʹ��ģ��֮���������ͷ��ڴ档
//
//

/////////////////////////////////////////////////////////////////////////////
// CBmpModuleApp

BEGIN_MESSAGE_MAP(CBmpModuleApp, CWinApp)
	//{{AFX_MSG_MAP(CBmpModuleApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpModuleApp construction

CBmpModuleApp::CBmpModuleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBmpModuleApp object

CBmpModuleApp theApp;


// �ӿں������� �� ��һ�㣬Ψһ�������ϵ�Ľӿ�
int WINAPI AccessBMPModule(INFOSTR *pInfo)
{
	// ����������Բ����޸ĵ�ʹ�ã�������ķ���ֵ�������֡�

	switch(pInfo->comm)
	{
	case	CMD_GETPROCTYPE:	// ��ȡ��ģ���ܴ����ͼ������
		_fnCMD_GETPROCTYPE(pInfo);
		break;
	case	CMD_GETWRITERS:		// ��ȡ��ģ��������б�����ʱ�ö��ŷָ�
		_fnCMD_GETWRITERS(pInfo);
		break;
	case	CMD_GETWRITERMESS:	// ��ȡ�����ǵ�����
		_fnCMD_GETWRITERMESS(pInfo);
		break;
	case	CMD_GETBUILDID:		// ��ȡͼ��ģ���ڲ��汾��
		_fnCMD_GETBUILDID(pInfo);
		break;
	case	CMD_IS_VALID_FILE:	// �ж�ָ���ļ��Ƿ�����Ч��BMP�ļ�
		_fnCMD_IS_VALID_FILE(pInfo);
		break;
	case	CMD_GET_FILE_INFO:	// ��ȡָ���ļ�����Ϣ
		_fnCMD_GET_FILE_INFO(pInfo);
		break;
	case	CMD_LOAD_FROM_FILE:	// ��ָ��ͼ���ļ��ж�ȡ����
		_fnCMD_LOAD_FROM_FILE(pInfo);
		break;
	case	CMD_SAVE_TO_FILE:	// �����ݱ��浽ָ���ļ���
		_fnCMD_SAVE_TO_FILE(pInfo);
		break;
	case	CMD_IS_SUPPORT:		// ��ѯĳ�������Ƿ�֧��
		_fnCMD_IS_SUPPORT(pInfo);
		break;
	case	CMD_RESIZE:			// ���»�ȡָ���ߴ��ͼ��λ���ݣ�ֻ������ʸ��ͼ��
		_fnCMD_RESIZE(pInfo);
		break;
	default:
		pInfo->result = ER_ILLCOMM; // �Ƿ�����
		ASSERT(FALSE);			// �����ߵĳ������������ :-)
		break;
	}

	// ִ������ɹ�����1, ʧ�ܷ���0
	return (pInfo->result==ER_SUCCESS)? 1:0;
}





// ������ͺ��� �� �ڶ�����ͺ���
//********************************************************************//

// ����������ͺ���---���ͣ�CMD_IS_SUPPORT����
// ��ѯĳ�������Ƿ�֧��
void _fnCMD_IS_SUPPORT(INFOSTR *pInfo)
{
	// ���������Ϊ�ͻ������ѯʱʹ�ã������ʵ���˶�ĳ�������
	// ���ͣ����޸���Ӧ��case�е����ã�ʹ�䷵��ER_SUCCESS�����
	// ��ʾ���ģ���Ѿ�֧�ָ������ˡ�ͬʱ�����ڵ�����ļ����Ѱ�
	// ���˶�ǰ�ĸ�����Ľ��ͣ���ֻ����δ֧�ֵ�����������
	// ���뼴�ɡ�

	ASSERT(pInfo->result == ER_EMPTY);

	switch(pInfo->annexdata.cmAnnData)		 
	{
	case	CMD_GETPROCTYPE:	// ��ȡ��ģ���ܴ����ͼ������
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_GETWRITERS:		// ��ȡ��ģ��������б�����ʱ�ö��ŷָ�
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_GETWRITERMESS:	// ��ȡ�����ǵ�����
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_GETBUILDID:		// ��ȡͼ��ģ���ڲ��汾��
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_IS_VALID_FILE:	// �ж�ָ���ļ��Ƿ�����Ч��WMF�ļ�
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_GET_FILE_INFO:	// ��ȡָ���ļ�����Ϣ
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_LOAD_FROM_FILE:	// ��ָ��ͼ���ļ��ж�ȡ����
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_SAVE_TO_FILE:	// �����ݱ��浽ָ���ļ���
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_IS_SUPPORT:		// ��ѯĳ�������Ƿ�֧��
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_RESIZE:			// ��ȡָ���ߴ��ͼ��ֻ������ʸ��ͼ��
		pInfo->result = ER_NOTSUPPORT;
		break;
	default:
		pInfo->result = ER_NOTSUPPORT;
		break;
	}
}


// ����������ͺ���---���ͣ�CMD_GETPROCTYPE����
// ��ȡ��ģ���ܴ����ͼ�����ͣ��磺BMP��PCX�ȵ�
void _fnCMD_GETPROCTYPE(INFOSTR *pInfo)
{
	// ����Ԥ����ĺ������룬����Բ����޸ĵ�ʹ�á�

	// ���ݽӿڶ��壬��ʱ��������Ӧ�����Ϊ0�������´˶���
	ASSERT(pInfo->annexdata.scAnnData[0] == 0);
	ASSERT(pInfo->result == ER_EMPTY);
	// �����ܴ���������ַ���
	::CopyMemory((PVOID)pInfo->annexdata.scAnnData, (PVOID)ModuleProcessImgType, 
		sizeof(ModuleProcessImgType));

	pInfo->result = ER_SUCCESS;
}



// ����������ͺ���---���ͣ�CMD_GETWRITER����
// ��ȡ��ģ��������б�����ʱ�ö��ŷָ�
void _fnCMD_GETWRITERS(INFOSTR *pInfo)
{
	// ����Ԥ����ĺ������룬����Բ����޸ĵ�ʹ�á�

	// ���ݽӿڶ��壬��ʱ��������Ӧ�����Ϊ0�������´˶���
	ASSERT(pInfo->annexdata.scAnnData[0] == 0);
	ASSERT(pInfo->result == ER_EMPTY);
	// ���ƿ�����������
	::CopyMemory((PVOID)pInfo->annexdata.scAnnData, (PVOID)WriterList, 
		sizeof(WriterList));

	pInfo->result = ER_SUCCESS;
}


// ����������ͺ���---���ͣ�CMD_GETWRITERMESS����
// ��ȡ�����ǵ�����
void _fnCMD_GETWRITERMESS(INFOSTR *pInfo)
{
	// ����Ԥ����ĺ������룬����Բ����޸ĵ�ʹ�á�

	// ���ݽӿڶ��壬��ʱ��������Ӧ�����Ϊ0�������´˶���
	ASSERT(pInfo->annexdata.scAnnData[0] == 0);
	ASSERT(pInfo->result == ER_EMPTY);
	// ���ƿ������ǵ������ַ���
	::CopyMemory((PVOID)pInfo->annexdata.scAnnData, (PVOID)WriterMess, 
		sizeof(WriterMess));

	pInfo->result = ER_SUCCESS;
}



// ����������ͺ���---���ͣ�CMD_GETBUILDID����
// ��ȡͼ��ģ���ڲ��汾��
void _fnCMD_GETBUILDID(INFOSTR *pInfo)
{
	// ����Ԥ����ĺ������룬����Բ����޸ĵ�ʹ�á�

	// ���ݽӿڶ��壬��ʱannexdata.dwAnnDataӦ����Ϊ0�������´˶���
	ASSERT(pInfo->annexdata.dwAnnData == 0);
	ASSERT(pInfo->result == ER_EMPTY);
	// ��д�ڲ��汾����
	pInfo->annexdata.dwAnnData = MODULE_BUILDID;

	pInfo->result = ER_SUCCESS;
}



// ����������ͺ���---���ͣ�CMD_IS_VALID_FILE����
// �ж�ָ���ļ��Ƿ�����Ч��BMP�ļ�
void _fnCMD_IS_VALID_FILE(INFOSTR *pInfo)
{
	CFile file;
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bmiHeader;
	DWORD dwSize;
	UINT uRet, uRet2;

	ASSERT(pInfo);

	// ������ڲ����Ƿ���Ͻӿڶ���
	ASSERT(pInfo->result == ER_EMPTY);
	ASSERT(pInfo->annexdata.iAnnData == 0);
	ASSERT(::strlen(pInfo->filename));
	ASSERT(pInfo->state == PKST_NOTVER);

	// ���ֵ
	pInfo->result = ER_SUCCESS;
	pInfo->annexdata.iAnnData = 0;	

	// ���ж�ָ�����ļ��Ƿ����
	if (!IsFileExist(pInfo->filename))
		pInfo->result = ER_COMMINFOERR;
	else
	{
		// ��ָ���ļ�
		if (!file.Open(pInfo->filename, CFile::modeRead))
		{
			pInfo->result = ER_FILERWERR;	// ���ļ�ʱ����
			return;
		}

		// ��ȡBMP�ļ��ĳ��ȣ����ֽ�Ϊ��λ��
		dwSize = file.GetLength();

		// �ó����ж�
		if (dwSize < (sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)))
		{
			// �ⲻ��һ��BMP�ļ���BMP�ļ��ĳ�����������ļ�ͷ
			// ����Ϣͷ�ṹ�ĳ���
			file.Close();					
			return;
		}

		// ��ȡBMP���ļ�ͷ����Ϣͷ�ṹ����������ǵ���Ч��
		file.SeekToBegin();
		uRet  = file.Read((LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER));
		uRet2 = file.Read((LPSTR)&bmiHeader, sizeof(BITMAPINFOHEADER));	
		if ((uRet != sizeof(BITMAPFILEHEADER))||(uRet2 != sizeof(BITMAPINFOHEADER)))
		{
			pInfo->result = ER_FILERWERR;	// ���ļ�ʱ����
			file.Close();
			return;
		}

		// �ж��ļ�ͷ����λͼ��־����BM����
		if (bmfHeader.bfType != DIB_HEADER_MARKER)
		{
			file.Close();					
			return;
		}

		// �ж���Ϣ�ṹ�ĳߴ�
		switch(bmiHeader.biSize)
		{
		case	sizeof(BITMAPINFOHEADER):
			break;
		default:
		//case	sizeof(BITMAPCOREHEADER):	// ��֧��OS/2��ʽ��λͼ
		//case	sizeof(BITMAPV5HEADER):		// ��֧��NT5����ϵͳ������λͼ
		//case	sizeof(BITMAPV4HEADER):		// ��֧��Windows4.0ϵͳ������λͼ���Ժ�֧��
			file.Close();					
			return;
		}
		
		// ����λ����ƫ�Ƶ�ַ
		if ((bmfHeader.bfOffBits > dwSize)||
			(bmfHeader.bfOffBits < (bmiHeader.biSize+sizeof(BITMAPFILEHEADER))))
		{
			file.Close();					
			return;
		}

		// ���ˣ����¿��Ա������ļ���һ����Ч��BMP�ļ���iAnnData������Ϊ1
		pInfo->annexdata.iAnnData = 1;
		pInfo->state = PKST_PASSVER;	// ��ʾͨ��У��

		file.Close();
	}
}




// ����������ͺ���---���ͣ�CMD_GET_FILE_INFO����
// ��ȡָ���ļ�����Ϣ
void _fnCMD_GET_FILE_INFO(INFOSTR *pInfo)
{
	CFile file;
	CFileStatus	status;
	LPBITMAPINFO pDIB;
	BITMAPFILEHEADER bmfHeader;
	UINT uRet, uDibInfoSize;

	// ������ڲ����Ƿ���Ͻӿڶ���
	ASSERT(pInfo->result == ER_EMPTY);
	ASSERT(::strlen(pInfo->filename));
	// ��ʱ�����ļ�������һ���Ѵ��ڵġ���������Ч��BMP�ļ�
	ASSERT(pInfo->state == PKST_PASSVER);
	// �ͻ�ģ�����Ҫ�Ƚ�imginfo���Ϊ0
	ASSERT(pInfo->imginfo.imgtype == IMT_NULL);

	// ��ָ���ļ�
	if (!file.Open(pInfo->filename, CFile::modeRead))
	{
		pInfo->result = ER_FILERWERR;
		return;
	}

	// ��ȡBMP���ļ�ͷ�ṹ
	file.SeekToBegin();
	uRet  = file.Read((LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER));

	if (uRet != sizeof(BITMAPFILEHEADER))
	{
		file.Close();
		pInfo->result = ER_FILERWERR;
		return;
	}

	uDibInfoSize = bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER);

	// �����ڴ棬���ڴ����Ϣͷ����ɫ������У�
	pDIB = (LPBITMAPINFO)::GlobalAlloc(GPTR, uDibInfoSize);
	if (!pDIB)
	{
		file.Close();
		pInfo->result = ER_MEMORYERR;
		return;
	}

	// ��ȡͼ����Ϣ����
	file.Seek((LONG)sizeof(BITMAPFILEHEADER), CFile::begin);
	uRet = file.Read((void *)pDIB, uDibInfoSize);
	if (uRet != uDibInfoSize)
	{
		::GlobalFree((void*)pDIB);
		file.Close();
		pInfo->result = ER_FILERWERR;
		return;
	}

	LPIMAGEINFOSTR lpImgInfoStr = &pInfo->imginfo;

	// ��ȡ�ļ��ĳ��ȡ�ͼ��Ŀ�ȡ��߶ȵ���Ϣ
	lpImgInfoStr->imgtype = IMT_RESSTATIC;
	lpImgInfoStr->imgformat = IMF_BMP;
	lpImgInfoStr->filesize = file.GetLength();
	lpImgInfoStr->width = pDIB->bmiHeader.biWidth;
	// ͼ��ĸ߶�ֵ��ʱ�����Ǹ�ֵ������ʹ����abs()����
	lpImgInfoStr->height = (DWORD)::abs(pDIB->bmiHeader.biHeight);
	lpImgInfoStr->bitcount = (DWORD)pDIB->bmiHeader.biBitCount;
	switch(pDIB->bmiHeader.biCompression)
	{
	case	BI_RGB:		//0L
		lpImgInfoStr->compression = ICS_RGB; break;
	case	BI_RLE8:	//1L
		lpImgInfoStr->compression = ICS_RLE8; break;
	case	BI_RLE4:	//2L
		lpImgInfoStr->compression = ICS_RLE4; break;
	case	BI_BITFIELDS://3L
		lpImgInfoStr->compression = ICS_BITFIELDS; break;
	default:
		//case	BI_PNG:				// �Ժ�İ汾��֧��������λ��ʽ
		//case	BI_JPEG:
		ASSERT(FALSE);
	}
	// ÿһͼ������ռ���ֽ�����DWORD���룬����ֻ�Է�ѹ��λͼ��Ч��
	lpImgInfoStr->linesize = DIBSCANLINE_WIDTHBYTES(pDIB->bmiHeader.biWidth*pDIB->bmiHeader.biBitCount);
	lpImgInfoStr->imgnumbers = 1;	// BMP�ļ���ֻ��һ��ͼ��
	lpImgInfoStr->imgchang = 0;		// ��ʾ���Ա��༭

	// ��ȡ�ļ������޸����ڣ����ڸ��ֽڣ����ڵ��ֽڣ�
	file.GetStatus(status);
	lpImgInfoStr->year = (WORD)status.m_mtime.GetYear();
	lpImgInfoStr->monday = (WORD)status.m_mtime.GetMonth();
	lpImgInfoStr->monday <<= 8;
	lpImgInfoStr->monday |= (WORD)status.m_mtime.GetDay();

	// ��ȡ�ļ������޸�ʱ�䣨������ߡ�0, 2��ʱ��1���֣�0���룩
	lpImgInfoStr->time = status.m_mtime.GetHour();
	lpImgInfoStr->time <<= 8;
	lpImgInfoStr->time |= status.m_mtime.GetMinute();
	lpImgInfoStr->time <<= 8;
	lpImgInfoStr->time |= status.m_mtime.GetSecond();
	lpImgInfoStr->time &= 0xffffff;

	::GlobalFree((void*)pDIB);
	file.Close();

	// ���ó�������
	pInfo->state = PKST_PASSINFO;
	pInfo->result = ER_SUCCESS;
}



// ����������ͺ���---���ͣ�CMD_LOAD_FROM_FILE����
// ��ָ��ͼ���ļ��ж�ȡ����
void _fnCMD_LOAD_FROM_FILE(INFOSTR *pInfo)
{
	// ������ڲ����Ƿ���Ͻӿڶ���
	ASSERT(pInfo->result == ER_EMPTY);
	ASSERT(::strlen(pInfo->filename));
	// ��ʱ�����ļ�������һ���Ѵ��ڵġ���Ч��BMP�ļ����������ݰ���
	// ���и��ļ�����Ϣ��imginfo�ṹ�У�
	ASSERT(pInfo->state == PKST_PASSINFO);
	ASSERT(pInfo->imginfo.imgformat == IMF_BMP);
	ASSERT(pInfo->pImgInfo == NULL);
	// �������ñ�׼ͼ���ʽ��Ϣ
	ASSERT(pInfo->sDIBInfo.bmi.biSize == sizeof(BITMAPINFOHEADER));
	ASSERT(pInfo->pLineAddr != NULL);
	ASSERT(pInfo->_pbdata != NULL);

	CFile file;

	// ��ͼ��ĸ߶�ֵ��Ϊ������ֵ
	RWPROGRESSSIZE = (int)pInfo->imginfo.height;

	if (pInfo->fpProgress)
	{
		if ((*pInfo->fpProgress)(RWPROGRESSSIZE, 0))
		{	
			// ������Ⱥ�������1����˵���û����жϲ��������ء�
			pInfo->result = ER_USERBREAK;
			return;
		}
	}

	// ��ָ���ļ�
	if (!file.Open(pInfo->filename, CFile::modeRead))
	{
		pInfo->result = ER_FILERWERR;
		return;
	}
	file.Seek(0, CFile::begin);

	if (pInfo->fpProgress)
	{
		if ((*pInfo->fpProgress)(RWPROGRESSSIZE, 0))
		{	
			// ������Ⱥ�������1����˵���û����жϲ��������ء�
			file.Close();
			pInfo->result = ER_USERBREAK;
			return;
		}
	}

	// ��ȡ��ת��ͼ�����ݰ���
	if (_Read(file, pInfo) == 0)
	{
		// �ɹ�
		pInfo->state = PKST_INFOANDBITS;
		pInfo->modify = 0;
		pInfo->result = ER_SUCCESS;
	}
	
	file.Close();

	if (pInfo->fpProgress)	// �������������˵��ò���֧���û��ж�
		(*pInfo->fpProgress)(RWPROGRESSSIZE, RWPROGRESSSIZE);

}



// ����������ͺ���---���ͣ�CMD_SAVE_TO_FILE����
// �����ݱ��浽ָ���ļ���

// pInfo��ָ������ݰ��У�annexdata.siAnnData[]�����ĺ�����ͣ�
//		[0] �� λ��ȣ�������1��4��8��16(555)��24��32(888)������ֵ
//				�е�һ��������������ֵ�������ܴ洢ΪRLE��ʽ
//		[1] �� �Ƿ�ʹ��ԭʼ��ɫ�塣0 �� ��ʾʹ�ã�1 �� ��ʾʹ�õ�ǰ
//				�����ȱʡ��ɫ��
void _fnCMD_SAVE_TO_FILE(INFOSTR *pInfo)
{
	// ������ڲ����Ƿ���Ͻӿڶ���
	ASSERT(pInfo->result == ER_EMPTY);
	// ���ļ���������һ���Ѵ��ڵ��ļ���Ҳ�����ǲ����ڵ��ļ�������������������
	ASSERT(::strlen(pInfo->filename));
	ASSERT(pInfo->state == PKST_INFOANDBITS);
	// �������ñ�׼ͼ���ʽ��Ϣ
	ASSERT(pInfo->sDIBInfo.bmi.biSize == sizeof(BITMAPINFOHEADER));
	ASSERT(pInfo->pLineAddr != NULL);
	ASSERT(pInfo->_pbdata != NULL);

	CFile file;
	CFileStatus	status;
	BITMAPFILEHEADER	bmf;
	int	uDibInfoSize;
	LPBITMAPINFO pDIB;
	int uRet;

	// ��ʱimginfo�е����ݿ�����sDIBInfo��һ������sDIBInfoΪ׼
	RWPROGRESSSIZE = (int)::abs(pInfo->sDIBInfo.bmi.biHeight);

	if (pInfo->fpProgress)
	{
		if ((*pInfo->fpProgress)(RWPROGRESSSIZE, 0))
		{	// ������Ⱥ�������1����˵���û����жϲ��������ء�
			pInfo->result = ER_USERBREAK;
			return;
		}
	}

	// ��ָ���ļ�����������ھʹ�����������ڣ����ض�Ϊ0���ȣ�����
	// ������������д���ļ�
	if (!file.Open(pInfo->filename, CFile::modeCreate|CFile::modeNoTruncate|
			CFile::modeReadWrite|CFile::shareDenyWrite))
	{
		pInfo->result = ER_FILERWERR;
		return;
	}
	file.Seek(0, CFile::begin);

	LPIMAGEINFOSTR lpImgInfoStr = &pInfo->imginfo;

	// ��ȡ��ת��ͼ�����ݰ���
	if (_Save(file, pInfo) == 0)
	{
		file.Flush();	// ˢ���ļ������ݡ��ߴ硢����޸�����

		file.Seek(0, CFile::begin);
		if (file.Read((void*)&bmf, sizeof(BITMAPFILEHEADER)) != sizeof(BITMAPFILEHEADER))
		{
			file.Close();
			pInfo->result = ER_FILERWERR;
			return;
		}

		uDibInfoSize = bmf.bfOffBits-sizeof(BITMAPFILEHEADER);

		// �����ڴ棬���ڴ����Ϣͷ����ɫ������У�
		pDIB = (LPBITMAPINFO)::GlobalAlloc(GPTR, uDibInfoSize);
		if (!pDIB)
		{
			file.Close();
			pInfo->result = ER_MEMORYERR;
			return;
		}

		// ��ȡͼ����Ϣ����
		file.Seek((LONG)sizeof(BITMAPFILEHEADER), CFile::begin);
		uRet = file.Read((void *)pDIB, uDibInfoSize);
		if (uRet != uDibInfoSize)
		{
			::GlobalFree((void*)pDIB);
			file.Close();
			pInfo->result = ER_FILERWERR;
			return;
		}

		if (pInfo->pImgInfo)	// �ͷ�ͼ���ԭʼ��Ϣ��
			::GlobalFree((void*)pInfo->pImgInfo);

		pInfo->pImgInfo = (void*)pDIB;	// ����ԭʼ��Ϣ��

		// ��ȡ�ļ��ĳ��ȡ�ͼ��Ŀ�ȡ��߶ȵ���Ϣ
		lpImgInfoStr->imgtype = IMT_RESSTATIC;
		lpImgInfoStr->imgformat = IMF_BMP;
		lpImgInfoStr->filesize = file.GetLength();
		lpImgInfoStr->width = pInfo->sDIBInfo.bmi.biWidth;
		// ͼ��ĸ߶�ֵ��ʱ�����Ǹ�ֵ������ʹ����abs()����
		lpImgInfoStr->height = (DWORD)::abs(pInfo->sDIBInfo.bmi.biHeight);
		lpImgInfoStr->bitcount = (DWORD)pInfo->annexdata.siAnnData[0];	// �µ�λ���ֵ
		lpImgInfoStr->compression = ICS_RGB;	// д������ǰֻ֧��BI_RGB��ʽ

		// ÿһͼ������ռ���ֽ�����DWORD���룬����ֻ�Է�ѹ��λͼ��Ч��
		lpImgInfoStr->linesize = DIBSCANLINE_WIDTHBYTES(lpImgInfoStr->width*lpImgInfoStr->bitcount);
		lpImgInfoStr->imgnumbers = 1;	// BMP�ļ���ֻ��һ��ͼ��
		lpImgInfoStr->imgchang = 0;		// ��ʾ���Ա��༭

		// ��ȡ�ļ������޸����ڣ����ڸ��ֽڣ����ڵ��ֽڣ�
		file.GetStatus(status);
		lpImgInfoStr->year = (WORD)status.m_mtime.GetYear();
		lpImgInfoStr->monday = (WORD)status.m_mtime.GetMonth();
		lpImgInfoStr->monday <<= 8;
		lpImgInfoStr->monday |= (WORD)status.m_mtime.GetDay();

		// ��ȡ�ļ������޸�ʱ�䣨������ߡ�0, 2��ʱ��1���֣�0���룩
		lpImgInfoStr->time = status.m_mtime.GetHour();
		lpImgInfoStr->time <<= 8;
		lpImgInfoStr->time |= status.m_mtime.GetMinute();
		lpImgInfoStr->time <<= 8;
		lpImgInfoStr->time |= status.m_mtime.GetSecond();
		lpImgInfoStr->time &= 0xffffff;

		// �ɹ�
		pInfo->result = ER_SUCCESS;
	}
	
	file.Close();

	if (pInfo->fpProgress)	// �������������˵��ò���֧���û��ж�
		(*pInfo->fpProgress)(RWPROGRESSSIZE, RWPROGRESSSIZE);
}




// ����������ͺ���---���ͣ�CMD_RESIZE����
// ���»�ȡָ���ߴ��ͼ��λ���ݣ�ֻ������ʸ��ͼ��
void _fnCMD_RESIZE(INFOSTR *pInfo)
{
	// �������һ���ͼ���дģ�鲻��Ҫ֧�֣���ֻ������ʸ��ͼ��
	// ����WMF��EMF֮�ࡣ
	// ���������͸�����������������룬���޸�pInfo->result�ķ���ֵ��
	// ----------------------------------------------------------------->

	pInfo->result = ER_NOTSUPPORT;
}



/*************************************************************************
 *
 * IsFileExist()
 * 
 * ����˵����
 *
 * char *lpFileName		- ���жϵ��ļ�·�������ƣ��ļ�����
 *
 * ����ֵ��
 *
 * BOOL					- ���ָ�����ļ����ڷ���TRUE�����򷵻�FALSE��
 * 
 * ������
 *
 * �ж�ָ�����ļ��Ƿ����
 * 
 * ���ļ�������Ա�����д
 *
 ************************************************************************/


BOOL IsFileExist(char *lpFileName)
{
	CFile	file;
	BOOL	bExist = FALSE;	// �ļ�������TRUE����������FALSE
	CFileException	e;

	// ȷ��ָ�����ļ��Ƿ����
	if (file.Open(lpFileName, CFile::modeReadWrite|CFile::shareDenyNone, &e))
	{
		bExist = TRUE;
		file.Close();
	}
	else
	{
		// �����������������ڴ�����ļ�
		switch(e.m_cause)
		{
		case	CFileException::tooManyOpenFiles:
		case	CFileException::accessDenied:
		case	CFileException::sharingViolation:
		case	CFileException::lockViolation:
			return TRUE;
		case	CFileException::fileNotFound:
		case	CFileException::badPath:
		case	CFileException::invalidFile:
		case	CFileException::hardIO:
		default:
			return FALSE;
		}
	}

	return bExist;
}


// ��ָ���ļ�����Ŀ�껺����
int _Read(CFile& file, LPINFOSTR pInfo)
{
	RGBQUAD *pPalette;
	int imgorg;
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bmiHeader;
	PBYTE pBits;
	PBYTE pImgInfo;
	DWORD dwSize, dwRet;
	UINT uRet;

	/* ��ȡDIB���ļ�ͷ�ṹ */
	file.SeekToBegin();
	uRet   = file.Read((LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER));
	uRet  += file.Read((LPSTR)&bmiHeader, sizeof(BITMAPINFOHEADER));
	if (uRet != (sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)))
	{
		pInfo->result = ER_FILERWERR;
		return 1;	// ���ļ�ʱ����
	}

	// pImgInfo������λͼ��Ϣ�ṹ����ɫ����Ϣ������еĻ���
	pImgInfo = (PBYTE)::GlobalAlloc(GPTR, bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER));
	if (pImgInfo == NULL)
	{
		pInfo->result = ER_MEMORYERR;
		return 2;	// �ڴ治��
	}

	// ��ȡλͼ�ļ���ԭʼ��Ϣ
	file.Seek(sizeof(BITMAPFILEHEADER), CFile::begin);
	uRet = file.Read((void*)pImgInfo, bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER));
	if (uRet != (bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER)))
	{
		::GlobalFree(pImgInfo);
		pInfo->result = ER_FILERWERR;
		return 1;
	}

	// ��������£��߶�ֵ�����Ǹ�ֵ��0
	ASSERT(RWPROGRESSSIZE > 0);

	if (pInfo->fpProgress)
	{
		if ((*pInfo->fpProgress)(RWPROGRESSSIZE, 0))
		{	
			// ������Ⱥ�������1����˵���û����жϲ��������ء�
			::GlobalFree(pImgInfo);
			pInfo->result = ER_USERBREAK;
			return 4;	// �û��жϲ���
		}
	}

	// ��ȡ�ļ�����
	dwSize = file.GetLength();

	// �����ڴ棬���ڴ��λ����
	pBits = (PBYTE)::GlobalAlloc(GPTR, dwSize-bmfHeader.bfOffBits);
	if (!pBits)
	{
		::GlobalFree(pImgInfo);
		pInfo->result = ER_MEMORYERR;
		return 2;	// �ڴ治��
	}

	// ��ȡλ����
	file.Seek(bmfHeader.bfOffBits, CFile::begin);
	dwRet = file.ReadHuge((void*)pBits, (dwSize-bmfHeader.bfOffBits));

	if (dwRet != (dwSize-bmfHeader.bfOffBits))
	{
		::GlobalFree(pImgInfo);
		::GlobalFree(pBits);
		pInfo->result = ER_FILERWERR;
		return 1;
	}

	if (pInfo->fpProgress)
	{
		if ((*pInfo->fpProgress)(RWPROGRESSSIZE, 0))
		{	
			// ������Ⱥ�������1����˵���û����жϲ��������ء�
			::GlobalFree(pImgInfo);
			::GlobalFree(pBits);
			pInfo->result = ER_USERBREAK;
			return 4;	// �û��жϲ���
		}
	}

	// ����Ķ�ȡ������Ҫ��ָ�룬�����ڴ˴���ֵ
	pInfo->pImgInfo = pImgInfo;

	pPalette = (RGBQUAD*)(pImgInfo+bmiHeader.biSize);
	imgorg = (bmiHeader.biHeight > 0) ? 0:1;	// 0-Ϊ����λͼ  1-Ϊ����
	int result = 0;		// ���ո������ķ���ֵ

	// ����λͼ�ļ���λ��ȵĲ�ͬ��������Ӧ�Ķ�ȡ����
	switch(_get_souformat((LPBITMAPINFO)pImgInfo))
	{
	case	SF_1:
		result = _from_1_read(pInfo, pBits, imgorg, pPalette);
		break;
	case	SF_4:
		result = _from_4_read(pInfo, pBits, imgorg, pPalette);
		break;
	case	SF_4_RLE:
		result = _from_4_RLE_read(pInfo, pBits, imgorg, pPalette);
		break;
	case	SF_8:
		result = _from_8_read(pInfo, pBits, imgorg, pPalette);
		break;
	case	SF_8_RLE:
		result = _from_8_RLE_read(pInfo, pBits, imgorg, pPalette);
		break;
	case	SF_16_555:
		result = _from_16_555_read(pInfo, pBits, imgorg);
		break;
	case	SF_16_565:
		result = _from_16_565_read(pInfo, pBits, imgorg);
		break;
	case	SF_24:
		result = _from_24_read(pInfo, pBits, imgorg);
		break;
	case	SF_32:
		result = _from_32_read(pInfo, pBits, imgorg);
		break;
	default:
		::GlobalFree(pImgInfo);
		::GlobalFree(pBits);
		pInfo->pImgInfo = NULL;
		pInfo->result = ER_SYSERR;
		return 3;		// ͼ���ļ���ʽ�쳣
	}

	// �ͷŴ��ļ��ж�ȡ��λ���ݣ��������ѱ�ת�������浽�����ݰ��У�
	::GlobalFree(pBits);
	
	if (result == 0)	// ִ�гɹ�
	{
		pInfo->result = ER_SUCCESS;
	}
	else
	{
		::GlobalFree(pImgInfo);
		pInfo->pImgInfo = NULL;
		return 4;		// ִ�й����в����Ĵ������pInfo->result
	}

	return 0;
}



// ��1λ������ɫ��λͼת��Ϊָ���ı�׼��ʽλͼ
// pInfo�е�Ŀ��ͼ�񻺳��������Ѿ������䣬�����е�Դͼ����Ϣ�ṹ�ѱ���ȷ��д
// imgorg������Դͼ���ɨ���д����־��0-Ϊ���� 1-Ϊ����
int _from_1_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg, RGBQUAD *lpPalette)
{
	ASSERT(pInfo);
	ASSERT(lpPalette);	// ��ɫλͼӦ��������ɫ����
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSou);

	int sImgOrg = imgorg;	// Դͼ��ɨ���д���ı�־��0-Ϊ���� 1-Ϊ����
							// Դͼ��Ĵ����ɵ����ߴ���

	int imgheight = pInfo->imginfo.height;	// ͼ��ĸ߶�
	int imgwidth  = pInfo->imginfo.width;	// ͼ��Ŀ��
	int scansize  = pInfo->imginfo.linesize;// ÿһɨ���еĿ��(in byte)
	int heigCount, widthCount, i, j, isou;
	ASSERT(pInfo->pImgInfo);
	int palnum = ((BITMAPINFOHEADER*)(pInfo->pImgInfo))->biClrUsed;
	PBYTE pCurPixAddr;
	BYTE pix;	// ��ǰ8������

	palnum = (palnum == 0) ? 2:palnum;	// �����ɫ�����ʵ�ʸ���
	ASSERT(palnum <= 2);

	switch(_get_desformat(pInfo))	// ��׼ͼ���ʽ��Ŀ���ʽ��
	{
	case	DF_16_555:
		{
		// ��555λ��ʽ�ĵ�ɫ������
		WORD descol555_0 = 0;
		WORD descol555_1 = 0;

		if (palnum > 0)
			descol555_0 = _cnv_rgb_to_555(lpPalette[0].rgbRed, lpPalette[0].rgbGreen, lpPalette[0].rgbBlue);
		if (palnum > 1)
			descol555_1 = _cnv_rgb_to_555(lpPalette[1].rgbRed, lpPalette[1].rgbGreen, lpPalette[1].rgbBlue);

		WORD *pDesPixAddr555;

		if (sImgOrg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						// ���λ���������������
						if (((BYTE)(0x80>>i))&pix)	// ��λ������
						{
							*pDesPixAddr555++ = descol555_1;
						}
						else
						{
							*pDesPixAddr555++ = descol555_0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						if (((BYTE)(0x80>>i))&pix)	// ��λ������
						{
							*pDesPixAddr555++ = descol555_1;
						}
						else
						{
							*pDesPixAddr555++ = descol555_0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_16_565:
		{
		// ��565λ��ʽ�ĵ�ɫ������
		WORD descol565_0 = 0;
		WORD descol565_1 = 0;

		if (palnum > 0)
			descol565_0 = _cnv_rgb_to_565(lpPalette[0].rgbRed, lpPalette[0].rgbGreen, lpPalette[0].rgbBlue);
		if (palnum > 1)
			descol565_1 = _cnv_rgb_to_565(lpPalette[1].rgbRed, lpPalette[1].rgbGreen, lpPalette[1].rgbBlue);

		WORD *pDesPixAddr565;

		if (sImgOrg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						if (((BYTE)(0x80>>i))&pix)	// ��λ������
						{
							*pDesPixAddr565++ = descol565_1;
						}
						else
						{
							*pDesPixAddr565++ = descol565_0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						if (((BYTE)(0x80>>i))&pix)	// ��λ������
						{
							*pDesPixAddr565++ = descol565_1;
						}
						else
						{
							*pDesPixAddr565++ = descol565_0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_24:
		{
		// 24λ��ʽ�ĵ�ɫ������
		BYTE red0, red1, green0, green1, blue0, blue1;

		if (palnum > 0)
		{
			red0 = lpPalette[0].rgbRed;
			green0 = lpPalette[0].rgbGreen;
			blue0 = lpPalette[0].rgbBlue;
		}
		if (palnum > 1)
		{
			red1 = lpPalette[1].rgbRed;
			green1 = lpPalette[1].rgbGreen;
			blue1 = lpPalette[1].rgbBlue;
		}

		PBYTE pDesPixAddr24;

		if (sImgOrg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						if (((BYTE)(0x80>>i))&pix)	// ��λ������
						{
							*pDesPixAddr24++ = blue1;
							*pDesPixAddr24++ = green1;
							*pDesPixAddr24++ = red1;
						}
						else
						{
							*pDesPixAddr24++ = blue0;
							*pDesPixAddr24++ = green0;
							*pDesPixAddr24++ = red0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						if (((BYTE)(0x80>>i))&pix)	// ��λ������
						{
							*pDesPixAddr24++ = blue1;
							*pDesPixAddr24++ = green1;
							*pDesPixAddr24++ = red1;
						}
						else
						{
							*pDesPixAddr24++ = blue0;
							*pDesPixAddr24++ = green0;
							*pDesPixAddr24++ = red0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_32:
		{
		// ��888λ��ʽ�ĵ�ɫ������
		DWORD descol888_0 = 0;
		DWORD descol888_1 = 0;

		if (palnum > 0)
			descol888_0 = _cnv_rgb_to_888(lpPalette[0].rgbRed, lpPalette[0].rgbGreen, lpPalette[0].rgbBlue);
		if (palnum > 1)
			descol888_1 = _cnv_rgb_to_888(lpPalette[1].rgbRed, lpPalette[1].rgbGreen, lpPalette[1].rgbBlue);

		DWORD *pDesPixAddr888;

		if (sImgOrg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						if (((BYTE)(0x80>>i))&pix)	// ��λ������
						{
							*pDesPixAddr888++ = descol888_1;
						}
						else
						{
							*pDesPixAddr888++ = descol888_0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						if (((BYTE)(0x80>>i))&pix)	// ��λ������
						{
							*pDesPixAddr888++ = descol888_1;
						}
						else
						{
							*pDesPixAddr888++ = descol888_0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_NULL:
	default:
		ASSERT(FALSE);
		pInfo->result = ER_ILLCOMM;
		return -1;
		break;
	}
	return 0;
}



// ��4λ����16ɫ��λͼת��Ϊָ���ı�׼��ʽλͼ
// pInfo�е�Ŀ��ͼ�񻺳��������Ѿ������䣬�����е�Դͼ����Ϣ�ṹ�ѱ���ȷ��д
// imgorg������Դͼ���ɨ���д����־��0-Ϊ���� 1-Ϊ����
int _from_4_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg, RGBQUAD *lpPalette)
{
	ASSERT(pInfo);
	ASSERT(lpPalette);	// 16ɫλͼӦ�е�ɫ��
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSou);

	int imgheight = pInfo->imginfo.height;	// ͼ��ĸ߶�
	int imgwidth  = pInfo->imginfo.width;	// ͼ��Ŀ��
	int scansize  = pInfo->imginfo.linesize;// ÿһɨ���еĿ��(in byte)
	int heigCount, widthCount, i, j, isou;
	ASSERT(pInfo->pImgInfo);
	int palnum = ((BITMAPINFOHEADER*)(pInfo->pImgInfo))->biClrUsed;
	PBYTE pCurPixAddr;
	BYTE pix;	// ��ǰ2������

	palnum = (palnum == 0) ? 16:palnum;	// �����ɫ�����ʵ�ʸ���
	ASSERT(palnum <= 16);

	switch(_get_desformat(pInfo))	// ��׼ͼ���ʽ��Ŀ���ʽ��
	{
	case	DF_16_555:
		{
		// ��555λ��ʽ�ĵ�ɫ������
		WORD descol555[16];
		for (i=0;i<palnum;i++)	// ��ʼ��ɫ����ɫ����
			descol555[i] = _cnv_rgb_to_555(lpPalette[i].rgbRed, lpPalette[i].rgbGreen, lpPalette[i].rgbBlue);

		WORD *pDesPixAddr555;

		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 2;

					for (i=0;i<j;i++)
					{
						if (i==0)
							*pDesPixAddr555++ = descol555[pix>>4];
						else
							*pDesPixAddr555++ = descol555[pix&0xf];
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 2;

					for (i=0;i<j;i++)
					{
						if (i==0)
							*pDesPixAddr555++ = descol555[pix>>4];
						else
							*pDesPixAddr555++ = descol555[pix&0xf];
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_16_565:
		{
		// ��565λ��ʽ�ĵ�ɫ������
		WORD descol565[16];
		for (i=0;i<palnum;i++)	// ��ʼ��ɫ����ɫ����
			descol565[i] = _cnv_rgb_to_565(lpPalette[i].rgbRed, lpPalette[i].rgbGreen, lpPalette[i].rgbBlue);

		WORD *pDesPixAddr565;

		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 2;

					for (i=0;i<j;i++)
					{
						if (i==0)
							*pDesPixAddr565++ = descol565[pix>>4];
						else
							*pDesPixAddr565++ = descol565[pix&0xf];
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 2;

					for (i=0;i<j;i++)
					{
						if (i==0)
							*pDesPixAddr565++ = descol565[pix>>4];
						else
							*pDesPixAddr565++ = descol565[pix&0xf];
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_24:
		{
		int index24_0, index24_1;

		PBYTE pDesPixAddr24;

		if (imgorg  == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 2;

					index24_0 = (int)(pix>>4);
					index24_1 = (int)(pix&0xf);

					for (i=0;i<j;i++)
					{
						if (i==0)
						{
							*pDesPixAddr24++ = lpPalette[index24_0].rgbBlue;
							*pDesPixAddr24++ = lpPalette[index24_0].rgbGreen;
							*pDesPixAddr24++ = lpPalette[index24_0].rgbRed;
						}
						else
						{
							*pDesPixAddr24++ = lpPalette[index24_1].rgbBlue;
							*pDesPixAddr24++ = lpPalette[index24_1].rgbGreen;
							*pDesPixAddr24++ = lpPalette[index24_1].rgbRed;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 2;

					index24_0 = (int)(pix>>4);
					index24_1 = (int)(pix&0xf);

					for (i=0;i<j;i++)
					{
						if (i==0)
						{
							*pDesPixAddr24++ = lpPalette[index24_0].rgbBlue;
							*pDesPixAddr24++ = lpPalette[index24_0].rgbGreen;
							*pDesPixAddr24++ = lpPalette[index24_0].rgbRed;
						}
						else
						{
							*pDesPixAddr24++ = lpPalette[index24_1].rgbBlue;
							*pDesPixAddr24++ = lpPalette[index24_1].rgbGreen;
							*pDesPixAddr24++ = lpPalette[index24_1].rgbRed;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_32:
		{
		// ��888λ��ʽ�ĵ�ɫ������
		DWORD descol888[16];
		for (i=0;i<palnum;i++)	// ��ʼ��ɫ����ɫ����
			descol888[i] = _cnv_rgb_to_888(lpPalette[i].rgbRed, lpPalette[i].rgbGreen, lpPalette[i].rgbBlue);

		DWORD *pDesPixAddr888;

		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 2;

					for (i=0;i<j;i++)
					{
						// ע����4λλͼ�У�ÿ���ֽڵĸ�4λ������������
						//     ��������Ǹ����أ�����ĳɨ���еĵ�һ����
						//     ���Ǻ�ɫ���ڶ��������ǰ�ɫ�����������A,
						//     �׵�������9����ô��Ӧ��д��9A��������д��
						//     A9����Ȼϰ�ߡ��߼�����������
						if (i == 0)
							*pDesPixAddr888++ = descol888[pix>>4];
						else
							*pDesPixAddr888++ = descol888[pix&0xf];
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// ��β�ж�
						j = imgwidth-widthCount;
					else
						j = 2;

					for (i=0;i<j;i++)
					{
						if (i == 0)
							*pDesPixAddr888++ = descol888[pix>>4];
						else
							*pDesPixAddr888++ = descol888[pix&0xf];
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_NULL:
	default:
		ASSERT(FALSE);
		pInfo->result = ER_ILLCOMM;
		return -1;
		break;
	}
	return 0;
}



// ��4λRLEλͼת��Ϊָ���ı�׼��ʽλͼ
// pInfo�е�Ŀ��ͼ�񻺳��������Ѿ������䣬�����е�Դͼ����Ϣ�ṹ�ѱ���ȷ��д
// imgorg������Դͼ���ɨ���д����־��0-Ϊ���� 1-Ϊ����
int _from_4_RLE_read(LPINFOSTR pInfo, PBYTE lpSouData, int imgorg, RGBQUAD *lpPalette)
{
	ASSERT(pInfo);
	ASSERT(lpPalette);	// 16ɫλͼӦ�е�ɫ��
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSouData);
	ASSERT(imgorg == 0);	// ѹ��λͼ�����ǵ����

	int imgheight = pInfo->imginfo.height;	// ͼ��ĸ߶�
	int imgwidth  = pInfo->imginfo.width;	// ͼ��Ŀ��
	PBYTE lpSou = lpSouData;
	BYTE pix, pix2;	// ��ǰֵ
	int result, i;

	BYTE **lppLineAddr = (BYTE**)::GlobalAlloc(GPTR, sizeof(PBYTE)*imgheight);
	if (lppLineAddr == NULL)
	{
		pInfo->result = ER_MEMORYERR;
		return -2;	// �ڴ治�㣬����
	}

	int linesize = ((imgwidth*4+31)/32)*4;

	PBYTE lpDes = (PBYTE)::GlobalAlloc(GPTR, linesize*imgheight);
	if (lpDes == NULL)
	{
		::GlobalFree(lppLineAddr);
		pInfo->result = ER_MEMORYERR;
		return -2;	// �ڴ治�㣬����
	}

	// ��ʼ���е�ַ����
	for (i=0;i<imgheight;i++)
		lppLineAddr[i] = (PBYTE)(lpDes+linesize*i);

	BOOL bEOF = FALSE;		// ͼ���β��־
	int  line = 0;			// ��ǰ��

	_write2buff4RLE(0, 0, 0, lppLineAddr[line]);	// ��ʼ��д����

	// �Ƚ�RLE�����ݽ�ѹ�ɱ�׼��BMP����
	while(!bEOF)
	{
		pix = *lpSou++;

		if (pix == 0)	// ���Է�ʽ
		{
			pix2 = *lpSou++;
			switch(pix2)
			{
			case	0:		// �н�β
				line++;
				_write2buff4RLE(2, 0, 0, lppLineAddr[line]);// ָ���µ�ƫ�Ƶ�ַ
				break;
			case	1:		// ͼ���β
				bEOF = TRUE;
				break;
			case	2:		// ��ǰ��ַƫ��
				pix = *lpSou++; pix2 = *lpSou++;
				line += pix2;	// ����pix2��
				_write2buff4RLE(2, pix%2, 0, (PBYTE)(lppLineAddr[line]+(DWORD)pix/2));
				break;
			default:		// ����ѹ��������
				for (i=0;i<pix2/2;i++)
				{
					pix = *(lpSou+i);
					_write2buff4RLE(1, 2, pix, NULL);
				}
				if (pix2%2)
				{	// ������©�ĵ�������
					pix = *(lpSou+i);
					_write2buff4RLE(1, 1, pix, NULL);
				}
				// (pix2+1)/2�ǽ����ض��뵽�ֽڱ߽��ϣ��ټ�1����2�Ƕ��뵽�ֱ߽���
				lpSou = (PBYTE)(lpSou + ((pix2+1)/2+1)/2*2);	// �ֶ���
				break;
			}
		}
		else			// ���뷽ʽ
		{
			// pix == ��ͬ���صĸ���
			pix2 = *lpSou++;	// pix2 == ����
			_write2buff4RLE(1, (int)pix, pix2, NULL);
		}
	}

	_write2buff4RLE(3, 0, 0, NULL);	// ����д����

	::GlobalFree(lppLineAddr);

	// �ñ�׼4λת������ת����Ŀ���ʽ
	result = _from_4_read(pInfo, lpDes, 0, lpPalette);

	::GlobalFree(lpDes);

	return result;
}



// ��8λ����256ɫ��λͼת��Ϊָ���ı�׼��ʽλͼ
// pInfo�е�Ŀ��ͼ�񻺳��������Ѿ������䣬�����е�Դͼ����Ϣ�ṹ�ѱ���ȷ��д
// imgorg������Դͼ���ɨ���д����־��0-Ϊ���� 1-Ϊ����
int _from_8_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg, RGBQUAD *lpPalette)
{
	ASSERT(pInfo);
	ASSERT(lpPalette);	// 256ɫλͼӦ�е�ɫ��
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSou);

	int imgheight = pInfo->imginfo.height;	// ͼ��ĸ߶�
	int imgwidth  = pInfo->imginfo.width;	// ͼ��Ŀ��
	int scansize  = pInfo->imginfo.linesize;// ÿһɨ���еĿ��(in byte)
	ASSERT(pInfo->pImgInfo);
	int palnum = ((BITMAPINFOHEADER*)(pInfo->pImgInfo))->biClrUsed;
	int heigCount, widthCount, i, isou;
	PBYTE pCurPixAddr;
	BYTE pix;	// ��ǰ����

	palnum = (palnum == 0) ? 256:palnum;	// �����ɫ�����ʵ�ʸ���
	ASSERT(palnum <= 256);

	switch(_get_desformat(pInfo))	// ��׼ͼ���ʽ��Ŀ���ʽ��
	{
	case	DF_16_555:
		{
		// ��555λ��ʽ�ĵ�ɫ������
		WORD descol555[256];
		for (i=0;i<palnum;i++)	// ��ʼ��ɫ����ɫ����
			descol555[i] = _cnv_rgb_to_555(lpPalette[i].rgbRed, lpPalette[i].rgbGreen, lpPalette[i].rgbBlue);

		WORD *pDesPixAddr555;

		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr555++ = descol555[*pCurPixAddr++];
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr555++ = descol555[*pCurPixAddr++];
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_16_565:
		{
		// ��565λ��ʽ�ĵ�ɫ������
		WORD descol565[256];
		for (i=0;i<palnum;i++)	// ��ʼ��ɫ����ɫ����
			descol565[i] = _cnv_rgb_to_565(lpPalette[i].rgbRed, lpPalette[i].rgbGreen, lpPalette[i].rgbBlue);

		WORD *pDesPixAddr565;

		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr565++ = descol565[*pCurPixAddr++];
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr565++ = descol565[*pCurPixAddr++];
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_24:
		{
		PBYTE pDesPixAddr24;
		if (imgorg  == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix = *pCurPixAddr++;
					*pDesPixAddr24++ = lpPalette[pix].rgbBlue;
					*pDesPixAddr24++ = lpPalette[pix].rgbGreen;
					*pDesPixAddr24++ = lpPalette[pix].rgbRed;
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix = *pCurPixAddr++;
					*pDesPixAddr24++ = lpPalette[pix].rgbBlue;
					*pDesPixAddr24++ = lpPalette[pix].rgbGreen;
					*pDesPixAddr24++ = lpPalette[pix].rgbRed;
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_32:
		{
		// ��888λ��ʽ�ĵ�ɫ������
		DWORD descol888[256];
		for (i=0;i<palnum;i++)	// ��ʼ��ɫ����ɫ����
			descol888[i] = _cnv_rgb_to_888(lpPalette[i].rgbRed, lpPalette[i].rgbGreen, lpPalette[i].rgbBlue);

		DWORD *pDesPixAddr888;

		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr888++ = descol888[*pCurPixAddr++];
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr888++ = descol888[*pCurPixAddr++];
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_NULL:
	default:
		ASSERT(FALSE);
		pInfo->result = ER_ILLCOMM;
		return -1;
		break;
	}
	return 0;
}




// ��8λRLEλͼת��Ϊָ���ı�׼��ʽλͼ
// pInfo�е�Ŀ��ͼ�񻺳��������Ѿ������䣬�����е�Դͼ����Ϣ�ṹ�ѱ���ȷ��д
// imgorg������Դͼ���ɨ���д����־��0-Ϊ���� 1-Ϊ����
int _from_8_RLE_read(LPINFOSTR pInfo, PBYTE lpSouData, int imgorg, RGBQUAD *lpPalette)
{
	ASSERT(pInfo);
	ASSERT(lpPalette);	// 256ɫλͼӦ�е�ɫ��
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSouData);
	ASSERT(imgorg == 0);	// ѹ��λͼ�����ǵ����

	int imgheight = pInfo->imginfo.height;	// ͼ��ĸ߶�
	int imgwidth  = pInfo->imginfo.width;	// ͼ��Ŀ��
	PBYTE lpSou = lpSouData;
	BYTE pix, pix2;	// ��ǰֵ
	int result, i;

	BYTE **lppLineAddr = (BYTE**)::GlobalAlloc(GPTR, sizeof(PBYTE)*imgheight);
	if (lppLineAddr == NULL)
	{
		pInfo->result = ER_MEMORYERR;
		return -2;	// �ڴ治�㣬����
	}

	int linesize = ((imgwidth+3)/4)*4;	// ɨ���гߴ�(in byte)

	PBYTE lpDes = (PBYTE)::GlobalAlloc(GPTR, linesize*imgheight);
	if (lpDes == NULL)
	{
		::GlobalFree(lppLineAddr);
		pInfo->result = ER_MEMORYERR;
		return -2;	// �ڴ治�㣬����
	}

	// ��ʼ���е�ַ����
	for (i=0;i<imgheight;i++)
		lppLineAddr[i] = (PBYTE)(lpDes+linesize*i);

	BOOL bEOF = FALSE;		// ͼ���β��־
	int  line = 0;			// ��ǰ��

	_write2buff8RLE(0, 0, 0, lppLineAddr[line]);	// ��ʼ��д����

	while(!bEOF)
	{
		pix = *lpSou++;

		if (pix == 0)	// ���Է�ʽ
		{
			pix2 = *lpSou++;
			switch(pix2)
			{
			case	0:		// �н�β
				line++;
				_write2buff8RLE(2, 0, 0, lppLineAddr[line]);// ָ���µ�ƫ�Ƶ�ַ
				break;
			case	1:		// ͼ���β
				bEOF = TRUE;
				break;
			case	2:		// ��ǰ��ַƫ��
				pix = *lpSou++; pix2 = *lpSou++;
				line += pix2;	// ����pix2��
				_write2buff8RLE(2, 0, 0, (PBYTE)(lppLineAddr[line]+(DWORD)pix));
				break;
			default:		// ����ѹ��������
				for (i=0;i<pix2;i++)
				{
					pix = *(lpSou+i);
					_write2buff8RLE(1, 1, pix, NULL);
				}
				// (pix2+1)/2�ǽ����ض��뵽�ֽڱ߽��ϣ��ټ�1����2�Ƕ��뵽�ֱ߽���
				lpSou = (PBYTE)(lpSou + ((pix2+1)/2)*2);	// �ֶ���
				break;
			}
		}
		else			// ���뷽ʽ
		{
			// pix == ��ͬ���صĸ���
			pix2 = *lpSou++;	// pix2 == ����
			_write2buff8RLE(1, (int)pix, pix2, NULL);
		}
	}

	_write2buff8RLE(3, 0, 0, NULL);	// ����д����

	::GlobalFree(lppLineAddr);

	// �ñ�׼8λת������ת����Ŀ���ʽ
	result = _from_8_read(pInfo, lpDes, 0, lpPalette);

	::GlobalFree(lpDes);

	return result;
}




// ��16λ555λͼת��Ϊָ���ı�׼��ʽλͼ
// pInfo�е�Ŀ��ͼ�񻺳��������Ѿ������䣬�����е�Դͼ����Ϣ�ṹ�ѱ���ȷ��д
// imgorg������Դͼ���ɨ���д����־��0-Ϊ���� 1-Ϊ����
int _from_16_555_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg)
{
	ASSERT(pInfo);
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSou);

	int imgheight = pInfo->imginfo.height;	// ͼ��ĸ߶�
	int imgwidth  = pInfo->imginfo.width;	// ͼ��Ŀ��
	int scansize  = pInfo->imginfo.linesize;// ÿһɨ���еĿ��(in byte)
	int heigCount, widthCount, isou;
	WORD *pCurPixAddr;
	WORD pix;	// ��ǰ����

	switch(_get_desformat(pInfo))	// ��׼ͼ���ʽ��Ŀ���ʽ��
	{
	case	DF_16_555:
		{
		WORD *pDesPixAddr555;
		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr555++ = *pCurPixAddr++;
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr555++ = *pCurPixAddr++;
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_16_565:
		{
		WORD *pDesPixAddr565;
		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)	// ʵ�ִ�555��565��ת��
					*pDesPixAddr565++ = (((*pCurPixAddr)&0xffe0)<<1)|((*pCurPixAddr++)&0x1f);
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)	// ʵ�ִ�555��565��ת��
					*pDesPixAddr565++ = (((*pCurPixAddr)&0xffe0)<<1)|((*pCurPixAddr++)&0x1f);
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_24:
		{
		PBYTE pDesPixAddr24;
		BYTE  red, green, blue;

		if (imgorg  == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix = *pCurPixAddr++;
					_cnv_555_to_rgb(pix, &red, &green, &blue);
					*pDesPixAddr24++ = blue;
					*pDesPixAddr24++ = green;
					*pDesPixAddr24++ = red;
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix = *pCurPixAddr++;
					_cnv_555_to_rgb(pix, &red, &green, &blue);
					*pDesPixAddr24++ = blue;
					*pDesPixAddr24++ = green;
					*pDesPixAddr24++ = red;
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_32:
		{
		DWORD *pDesPixAddr888;
		DWORD pix32;

		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix32 = (DWORD)*pCurPixAddr++;	// ʵ��555��888��ת��
					*pDesPixAddr888++ = ((pix32<<3)&0xf8)|((pix32<<6)&0xf800)|((pix32<<9)&0xf80000);
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix32 = (DWORD)*pCurPixAddr++;	// ʵ��555��888��ת��
					*pDesPixAddr888++ = ((pix32<<3)&0xf8)|((pix32<<6)&0xf800)|((pix32<<9)&0xf80000);
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_NULL:
	default:
		ASSERT(FALSE);
		pInfo->result = ER_ILLCOMM;
		return -1;
		break;
	}
	return 0;
}




// ��16λ565λͼת��Ϊָ���ı�׼��ʽλͼ
// pInfo�е�Ŀ��ͼ�񻺳��������Ѿ������䣬�����е�Դͼ����Ϣ�ṹ�ѱ���ȷ��д
// imgorg������Դͼ���ɨ���д����־��0-Ϊ���� 1-Ϊ����
int _from_16_565_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg)
{
	ASSERT(pInfo);
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSou);

	int imgheight = pInfo->imginfo.height;	// ͼ��ĸ߶�
	int imgwidth  = pInfo->imginfo.width;	// ͼ��Ŀ��
	int scansize  = pInfo->imginfo.linesize;// ÿһɨ���еĿ��(in byte)
	int heigCount, widthCount, isou;
	WORD *pCurPixAddr;
	WORD pix;	// ��ǰ����

	switch(_get_desformat(pInfo))	// ��׼ͼ���ʽ��Ŀ���ʽ��
	{
	case	DF_16_555:
		{
		WORD *pDesPixAddr555;
		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++) // ʵ�ִ�565��555��ת��
					*pDesPixAddr555++ = (((*pCurPixAddr)>>1)&0x7fe0)|((*pCurPixAddr++)&0x1f);
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++) // ʵ�ִ�565��555��ת��
					*pDesPixAddr555++ = (((*pCurPixAddr)>>1)&0x7fe0)|((*pCurPixAddr++)&0x1f);
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_16_565:
		{
		WORD *pDesPixAddr565;
		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr565++ = *pCurPixAddr++;
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr565++ = *pCurPixAddr++;
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_24:
		{
		PBYTE pDesPixAddr24;
		BYTE  red, green, blue;

		if (imgorg  == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix = *pCurPixAddr++;
					_cnv_565_to_rgb(pix, &red, &green, &blue);
					*pDesPixAddr24++ = blue;
					*pDesPixAddr24++ = green;
					*pDesPixAddr24++ = red;
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix = *pCurPixAddr++;
					_cnv_565_to_rgb(pix, &red, &green, &blue);
					*pDesPixAddr24++ = blue;
					*pDesPixAddr24++ = green;
					*pDesPixAddr24++ = red;
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_32:
		{
		DWORD *pDesPixAddr888;
		DWORD pix32;

		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix32 = (DWORD)*pCurPixAddr++;	// ʵ��565��888��ת��
					*pDesPixAddr888++ = ((pix32<<3)&0xf8)|((pix32<<5)&0xfc00)|((pix32<<8)&0xf80000);
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix32 = (DWORD)*pCurPixAddr++;	// ʵ��565��888��ת��
					*pDesPixAddr888++ = ((pix32<<3)&0xf8)|((pix32<<5)&0xfc00)|((pix32<<8)&0xf80000);
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_NULL:
	default:
		ASSERT(FALSE);
		pInfo->result = ER_ILLCOMM;
		return -1;
		break;
	}
	return 0;
}



// ��24λλͼת��Ϊָ���ı�׼��ʽλͼ
// pInfo�е�Ŀ��ͼ�񻺳��������Ѿ������䣬�����е�Դͼ����Ϣ�ṹ�ѱ���ȷ��д
// imgorg������Դͼ���ɨ���д����־��0-Ϊ���� 1-Ϊ����
int _from_24_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg)
{
	ASSERT(pInfo);
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSou);

	int imgheight = pInfo->imginfo.height;	// ͼ��ĸ߶�
	int imgwidth  = pInfo->imginfo.width;	// ͼ��Ŀ��
	int scansize  = pInfo->imginfo.linesize;// ÿһɨ���еĿ��(in byte)
	int heigCount, widthCount, isou;
	PBYTE pCurPixAddr;
	BYTE  red, green, blue;

	switch(_get_desformat(pInfo))	// ��׼ͼ���ʽ��Ŀ���ʽ��
	{
	case	DF_16_555:
		{
		WORD *pDesPixAddr555;
		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++) // ʵ�ִ�24��555��ת��
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					*pDesPixAddr555++ = _cnv_rgb_to_555(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++) // ʵ�ִ�24��555��ת��
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					*pDesPixAddr555++ = _cnv_rgb_to_555(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_16_565:
		{
		WORD *pDesPixAddr565;
		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					*pDesPixAddr565++ = _cnv_rgb_to_565(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					*pDesPixAddr565++ = _cnv_rgb_to_565(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_24:
		{
		PBYTE pDesPixAddr24;
		if (imgorg  == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				::CopyMemory((PVOID)pDesPixAddr24, (PVOID)pCurPixAddr, imgwidth*3);
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				::CopyMemory((PVOID)pDesPixAddr24, (PVOID)pCurPixAddr, imgwidth*3);
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_32:
		{
		DWORD *pDesPixAddr888;
		DWORD pix32;

		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix32 = *((DWORD*)pCurPixAddr);
					pix32 &= 0xffffff;	// 888��ʽ���������λ
					pCurPixAddr += 3;
					*pDesPixAddr888++ = pix32;
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix32 = *((DWORD*)pCurPixAddr);
					pix32 &= 0xffffff;	// 888��ʽ���������λ
					pCurPixAddr += 3;
					*pDesPixAddr888++ = pix32;
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_NULL:
	default:
		ASSERT(FALSE);
		pInfo->result = ER_ILLCOMM;
		return -1;
		break;
	}
	return 0;
}



// ��32λλͼת��Ϊָ���ı�׼��ʽλͼ
// pInfo�е�Ŀ��ͼ�񻺳��������Ѿ������䣬�����е�Դͼ����Ϣ�ṹ�ѱ���ȷ��д
// imgorg������Դͼ���ɨ���д����־��0-Ϊ���� 1-Ϊ����
int _from_32_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg)
{
	ASSERT(pInfo);
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSou);

	int imgheight = pInfo->imginfo.height;	// ͼ��ĸ߶�
	int imgwidth  = pInfo->imginfo.width;	// ͼ��Ŀ��
	int scansize  = pInfo->imginfo.linesize;// ÿһɨ���еĿ��(in byte)
	int heigCount, widthCount, isou;
	PBYTE pCurPixAddr;
	BYTE  red, green, blue;

	switch(_get_desformat(pInfo))	// ��׼ͼ���ʽ��Ŀ���ʽ��
	{
	case	DF_16_555:
		{
		WORD *pDesPixAddr555;
		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++) // ʵ�ִ�24��555��ת��
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					pCurPixAddr++;	// �������λ
					*pDesPixAddr555++ = _cnv_rgb_to_555(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++) // ʵ�ִ�24��555��ת��
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					pCurPixAddr++;	// �������λ
					*pDesPixAddr555++ = _cnv_rgb_to_555(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_16_565:
		{
		WORD *pDesPixAddr565;
		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					*pCurPixAddr++;
					*pDesPixAddr565++ = _cnv_rgb_to_565(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					*pCurPixAddr++;
					*pDesPixAddr565++ = _cnv_rgb_to_565(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_24:
		{
		PBYTE pDesPixAddr24;
		if (imgorg  == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					*pDesPixAddr24++ = *pCurPixAddr++;
					*pDesPixAddr24++ = *pCurPixAddr++;
					*pDesPixAddr24++ = *pCurPixAddr++;
					*pCurPixAddr++;
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					*pDesPixAddr24++ = *pCurPixAddr++;
					*pDesPixAddr24++ = *pCurPixAddr++;
					*pDesPixAddr24++ = *pCurPixAddr++;
					*pCurPixAddr++;
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_32:
		{
		PBYTE pDesPixAddr888;
		if (imgorg == 0)	// Դͼ�ǵ���λͼ
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr888  = (PBYTE)pInfo->pLineAddr[isou];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				::CopyMemory((PVOID)pDesPixAddr888, (PVOID)pCurPixAddr, imgwidth*4);
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		else	// ����λͼ��top-down��
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// Դͼ��ǰ�еĵ�һ�����ص�ַ
				pDesPixAddr888  = (PBYTE)pInfo->pLineAddr[heigCount];// Ŀ�껺������ǰ�е�һ�����صĵ�ַ
				::CopyMemory((PVOID)pDesPixAddr888, (PVOID)pCurPixAddr, imgwidth*4);
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// ������Ⱥ�������1����˵���û����жϲ��������ء�
						pInfo->result = ER_USERBREAK;
						return 1;	// �û��жϲ���
					}
				}
			}
		}
		}
		break;
	case	DF_NULL:
	default:
		ASSERT(FALSE);
		pInfo->result = ER_ILLCOMM;
		return -1;
		break;
	}
	return 0;
}




// pInfo��ָ������ݰ��У�annexdata.siAnnData[]�����ĺ�����ͣ�
//		[0] �� λ��ȣ�������1��4��8��16(555)��24��32(888)������ֵ
//				�е�һ��������������ֵ�������ܴ洢ΪRLE��ʽ
//		[1] �� �Ƿ�ʹ��ԭʼ��ɫ�塣0 �� ��ʾʹ�ã�1 �� ��ʾʹ�õ�ǰ
//				�����ȱʡ��ɫ��
int _Save(CFile& pf, LPINFOSTR pInfo)
{
	PBYTE lpBits;
	HBITMAP hBmp;

	ASSERT(pInfo);
	int dibitcount = pInfo->annexdata.siAnnData[0];	// Ŀ���ļ���λ���
	int palnum = _get_palette_size(dibitcount);		// ��ɫ������Ŀ

	BITMAPINFO *pbmi = (BITMAPINFO*)::GlobalAlloc(GPTR, sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*palnum);
	RGBQUAD *ppal = (RGBQUAD*)((PBYTE)pbmi+sizeof(BITMAPINFOHEADER));

	if (!pbmi)
	{
		pInfo->result = ER_MEMORYERR;
		return 2;	// �ڴ治��
	}

	HWND hWnd = ::GetDesktopWindow();
	HDC hDC = ::GetDC(hWnd);

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biBitCount = dibitcount;
	pbmi->bmiHeader.biCompression = BI_RGB;
	pbmi->bmiHeader.biSizeImage = 0;
	pbmi->bmiHeader.biWidth = pInfo->sDIBInfo.bmi.biWidth;
	pbmi->bmiHeader.biHeight = pInfo->sDIBInfo.bmi.biHeight;
	pbmi->bmiHeader.biClrImportant = 0;
	pbmi->bmiHeader.biClrUsed = 0;
	pbmi->bmiHeader.biXPelsPerMeter = (int)(::GetDeviceCaps(hDC,LOGPIXELSX)*(float)39.373);
	pbmi->bmiHeader.biYPelsPerMeter = (int)(::GetDeviceCaps(hDC,LOGPIXELSY)*(float)39.373);

	if (palnum)
	{
		switch(pInfo->annexdata.siAnnData[1])
		{
		case	0:	// ʹ��ԭλͼ��ɫ�壨��ȡ��ԭʼͼ���ļ�����Ҳ��BMPλͼ��
			if (pInfo->imginfo.imgformat == IMF_BMP)
			{
				if (pInfo->imginfo.bitcount == (unsigned int)dibitcount) // ֻ��ԭʼͼ��ĸ�ʽ��Ŀ���ļ���λ�����ͬʱ����������
				{
					ASSERT(pInfo->pImgInfo);	// ԭʼ���ݱ�����Ч
					ASSERT(((BITMAPINFOHEADER*)(pInfo->pImgInfo))->biSize == sizeof(BITMAPINFOHEADER));

					RGBQUAD *porg = (RGBQUAD*)(((BITMAPINFOHEADER*)(pInfo->pImgInfo))+1);
					int palnum2 = ((BITMAPINFOHEADER*)(pInfo->pImgInfo))->biClrUsed;
					palnum2 = (palnum2) ? palnum2:palnum;
					ASSERT(palnum2 <= palnum);

					for (int i=0;i<palnum2;i++)
					{
						ppal[i].rgbBlue = porg[i].rgbBlue;
						ppal[i].rgbGreen = porg[i].rgbGreen;
						ppal[i].rgbRed = porg[i].rgbRed;
						ppal[i].rgbReserved = 0;
					}
					break;
				}
			}
			else
			{
				;	// ��ʹ��ϵͳȱʡ��ɫ��
			}
		case	1:	// ʹ��ϵͳȱʡ��ɫ��
			{
			LPPALETTEENTRY pal = (LPPALETTEENTRY)::GlobalAlloc(GPTR, sizeof(PALETTEENTRY)*palnum);
			if (!pal)
			{
				::GlobalFree(pbmi);
				::ReleaseDC(hWnd, hDC);
				pInfo->result = ER_MEMORYERR;
				return 2;	// �ڴ治��
			}
			::GetSystemPaletteEntries(hDC, 0, palnum, pal);
			for (int i=0;i<palnum;i++)
			{
				ppal[i].rgbBlue = pal[i].peBlue;
				ppal[i].rgbGreen = pal[i].peGreen;
				ppal[i].rgbRed = pal[i].peRed;
				ppal[i].rgbReserved = 0;
			}
			::GlobalFree(pal);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}

	hBmp = CreateDIBSection(hDC, pbmi, DIB_RGB_COLORS, (void**)&lpBits, NULL, 0);
	if (hBmp == NULL)
	{
		::GlobalFree(pbmi);
		::ReleaseDC(hWnd, hDC);
		pInfo->result = ER_SYSERR;
		return 5;		// ����ϵͳ���ȶ�
	}

	if (!SetDIBits(hDC, hBmp, 0, pInfo->imginfo.height, pInfo->_pbdata, (const BITMAPINFO*)&pInfo->sDIBInfo, DIB_RGB_COLORS))
	{
		::GlobalFree(pbmi);
		::ReleaseDC(hWnd, hDC);
		::DeleteObject(hBmp);
		pInfo->result = ER_SYSERR;
		return 5;
	}

	::ReleaseDC(hWnd, hDC);

	BITMAPFILEHEADER	bmf;
				 
	bmf.bfType = DIB_HEADER_MARKER;
	bmf.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+\
		sizeof(RGBQUAD)*palnum+((pInfo->imginfo.width*dibitcount+31)/32)*4*pInfo->imginfo.height;
	bmf.bfReserved1 = 0;
	bmf.bfReserved2 = 0;
	bmf.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+\
		sizeof(RGBQUAD)*palnum;


	TRY
	{
		// writing bitmap file header
		pf.SeekToBegin();
		pf.Write((const void *)&bmf, sizeof(BITMAPFILEHEADER));
		
		// writing bitmap info header
		pf.Write((const void *)pbmi, sizeof(BITMAPINFOHEADER));
	}
	CATCH(CFileException, e)
	{
		::GlobalFree(pbmi);
		::DeleteObject(hBmp);
		pInfo->result = ER_FILERWERR;
		return 1;	// д�ļ�ʱ����
	}
	END_CATCH

	::GlobalFree(pbmi);
	RGBQUAD *palette = NULL;

	TRY
	{
		if (palnum)
		{
			palette = (RGBQUAD*)::GlobalAlloc(GPTR, sizeof(RGBQUAD)*palnum);
			if (!palette)
			{
				::DeleteObject(hBmp);
				pInfo->result = ER_MEMORYERR;
				return 2;	// �ڴ治��
			}

			HDC hCompDC = ::CreateCompatibleDC(NULL);
			HBITMAP hOldBmp = (HBITMAP)::SelectObject(hCompDC, hBmp);

			::GetDIBColorTable(hCompDC, 0, palnum, palette);

			::SelectObject(hCompDC, hOldBmp);
			::DeleteDC(hCompDC);

			// writing color table.
			pf.Write((const void *)palette, sizeof(RGBQUAD)*palnum);

			::GlobalFree(palette);
		}

		pf.WriteHuge((const void *)lpBits, ((pInfo->imginfo.width*dibitcount+31)/32)*4*pInfo->imginfo.height);
	}
	CATCH(CFileException, e)
	{
		if (palette) ::GlobalFree(palette);
		::DeleteObject(hBmp);
		pInfo->result = ER_FILERWERR;
		return 1;	// д�ļ�ʱ����
	}
	END_CATCH

	::DeleteObject(hBmp);

	pInfo->result = ER_SUCCESS;

	return 0;	// д�ļ��ɹ�
}


// �жϱ�׼ͼ���λ��ʽ
DESFORMAT _get_desformat(LPINFOSTR pInfo)
{
	ASSERT(pInfo);
	// ASSERT(pInfo->state >= PKST_PASSVER);
	ASSERT(pInfo->sDIBInfo.bmi.biPlanes == 1);

	DESFORMAT result;

	switch(pInfo->sDIBInfo.bmi.biBitCount)
	{
	case	32:
		/******************************************************
						32λ����ʾ��ͼ
			    ��               ->                 ��
		0000 0000   0000 0000   0000 0000   0000 0000 	888��ʽ
		            1111 1111  ------------------------R
		                        1111 1111 -------------G
		                                    1111 1111--B
		* Win95 ϵͳֻ֧����һ�ָ�ʽ
		******************************************************/
		if (pInfo->sDIBInfo.bmi.biCompression == BI_RGB)
		{
			result = DF_32;
			break;
		}

		if ((pInfo->sDIBInfo.rmask == 0xff0000)&&
			(pInfo->sDIBInfo.gmask == 0xff00)&&
			(pInfo->sDIBInfo.bmask == 0xff))
			result = DF_32;
		else
		{
			ASSERT(FALSE);	// ֻ֧��888��ʽ
			result = DF_NULL;
		}
		break;
	case	24:
		result = DF_24;
		break;
	case	16:
		/*******************************************
				16λ����ʾ��ͼ

			  ���ֽ�      ���ֽ�
			0000 0000   0000 0000 
						   1 1111--B	// 555��ʽ
				   11	111 -------G
			 111 11	 --------------R
			0
						   1 1111--B	// 565��ʽ
				  111   111 -------G
			1111 1   --------------R

		* Win95 ϵͳֻ֧���������ָ�ʽ
		*******************************************/
		if (pInfo->sDIBInfo.bmi.biCompression == BI_RGB)
		{
			result = DF_16_555;
			break;
		}

		if ((pInfo->sDIBInfo.rmask == 0x7c00)&&
			(pInfo->sDIBInfo.gmask == 0x3e0)&&
			(pInfo->sDIBInfo.bmask == 0x1f))
			result = DF_16_555;
		else if ((pInfo->sDIBInfo.rmask == 0xf800)&&
			(pInfo->sDIBInfo.gmask == 0x7e0)&&
			(pInfo->sDIBInfo.bmask == 0x1f))
			result = DF_16_565;
		else
			result = DF_NULL;
		break;
	default:
		ASSERT(FALSE);		// ������������ʽ
		result = DF_NULL;
		break;
	}

	return result;
}




// �жϱ�׼ͼ���λ��ʽ
SOUFORMAT _get_souformat(LPBITMAPINFO pInfo)
{
	ASSERT(pInfo->bmiHeader.biSize);

	SOUFORMAT result;

	switch(pInfo->bmiHeader.biBitCount)
	{
	case	1:
		ASSERT(pInfo->bmiHeader.biCompression == BI_RGB);
		result = SF_1;
		break;
	case	4:
		if (pInfo->bmiHeader.biCompression == BI_RGB)
			result = SF_4;
		else if ((pInfo->bmiHeader.biCompression == BI_RLE4)&&\
				 (pInfo->bmiHeader.biHeight > 0))	// �����λͼ���ܱ�ѹ��
			result = SF_4_RLE;
		else
			result = SF_NULL;
		break;
	case	8:
		if (pInfo->bmiHeader.biCompression == BI_RGB)
			result = SF_8;
		else if ((pInfo->bmiHeader.biCompression == BI_RLE8)&&\
				 (pInfo->bmiHeader.biHeight > 0))
			result = SF_8_RLE;
		else
			result = SF_NULL;
		break;
	case	16:
		{
		/*******************************************
				16λ����ʾ��ͼ

			  ���ֽ�      ���ֽ�
			0000 0000   0000 0000 
						   1 1111--B	// 555��ʽ
				   11	111 -------G
			 111 11	 --------------R
			0
						   1 1111--B	// 565��ʽ
				  111   111 -------G
			1111 1   --------------R

		* Win95 ϵͳֻ֧���������ָ�ʽ
		*******************************************/
		if (pInfo->bmiHeader.biCompression == BI_RGB)
		{
			result = SF_16_555;
			break;
		}

		DWORD *pMark = (DWORD*)((LPBITMAPINFOHEADER)pInfo+1);

		if ((pMark[0] == 0x7c00)&&(pMark[1] == 0x3e0)&&(pMark[2] == 0x1f))
			result = SF_16_555;
		else if ((pMark[0] == 0xf800)&&(pMark[1] == 0x7e0)&&(pMark[2] == 0x1f))
			result = SF_16_565;
		else
			result = SF_NULL;
		break;
		}
	case	24:
		ASSERT(pInfo->bmiHeader.biCompression == BI_RGB);
		result = SF_24;
		break;
	case	32:
		{
		/******************************************************
						32λ����ʾ��ͼ
			    ��               ->                 ��
		0000 0000   0000 0000   0000 0000   0000 0000 	888��ʽ
		            1111 1111  ------------------------R
		                        1111 1111 -------------G
		                                    1111 1111--B
		* Win95 ϵͳֻ֧����һ�ָ�ʽ
		******************************************************/
		if (pInfo->bmiHeader.biCompression == BI_RGB)
		{
			result = SF_32;
			break;
		}

		DWORD *pMark = (DWORD*)((LPBITMAPINFOHEADER)pInfo+1);

		if ((pMark[0] == 0xff0000)&&(pMark[1] == 0x00ff00)&&(pMark[2] == 0xff))
			result = SF_32;
		else
			result = SF_NULL;	// ֻ֧��888��ʽ
		break;
		}
	default:
		result = SF_NULL;
		break;
	}

	return result;
}

// ��ָ����RGB��ɫ����ת����555��ʽ��WORD��ֵ���أ�
WORD _cnv_rgb_to_555(BYTE red, BYTE green, BYTE blue)
{
	WORD result = 0;

	result = (((WORD)red>>3)<<10)|(((WORD)green>>3)<<5)|((WORD)blue>>3);

	return result;
}


// ��ָ����555��ʽ����ɫת����RGB��ɫ����
void _cnv_555_to_rgb(WORD col, PBYTE red, PBYTE green, PBYTE blue)
{
	// ��555ת����RGBʱ�������ص����ȵ������
	*red = (BYTE)((col>>7)&0xf8);
	*green = (BYTE)((col>>2)&0xf8);
	*blue = (BYTE)(col<<3);
}


// ��ָ����RGB��ɫ����ת����565��ʽ��WORD��ֵ���أ�
WORD _cnv_rgb_to_565(BYTE red, BYTE green, BYTE blue)
{
	WORD result = 0;

	result = (((WORD)red>>3)<<11)|(((WORD)green>>2)<<5)|((WORD)blue>>3);

	return result;
}


// ��ָ����565��ʽ����ɫת����RGB��ɫ����
void _cnv_565_to_rgb(WORD col, PBYTE red, PBYTE green, PBYTE blue)
{
	// ��565ת����RGBʱ�������ص����ȵ������
	*red = (BYTE)((col>>8)&0xf8);
	*green = (BYTE)((col>>3)&0xfc);
	*blue = (BYTE)(col<<3);
}


// ��ָ����RGB��ɫ����ת����888��ʽ��DWORD��ֵ���أ�
DWORD _cnv_rgb_to_888(BYTE red, BYTE green, BYTE blue)
{
	DWORD result = 0;

	result = ((DWORD)red<<16)|((DWORD)green<<8)|(DWORD)blue;

	return result;
}



// ��4λ������д���أ�ֻ��_from_4_RLE_read()�����ڲ����ã�
// �β�mark���⣺0-��ʼ������  1-д����  2-�ƶ���ָ����  3-��������  4-��ȡ��ǰ��ַ
// ע����mark����2ʱ������num����Ϊ���س�2���������룬����iCurOff��ֵ
PBYTE _write2buff4RLE(int mark, int num, BYTE pix, PBYTE lpDesData)
{
	static PBYTE lpDes;		// ����������ַ
	static PBYTE lpCur;		// ��������ǰд��ַ
	static int   iCurOff;	// ���ֽ��еĵ�ǰλ�ã�0-�߰��ֽ�  1-�Ͱ��ֽ�
	int i;
	BYTE pixa[2];

	switch(mark)
	{
	case	0:		// ��ʼ������
		lpDes = lpDesData;
		lpCur = lpDes;
		iCurOff = 0;
		break;
	case	1:		// д����
		pixa[0] = pix>>4;
		pixa[1] = pix&0xf;
		for (i=0;i<num;i++)
		{
			if (iCurOff == 0)	// ������
			{
				*lpCur = pixa[i%2]<<4;
				iCurOff = 1;
			}
			else				// ������
			{
				ASSERT(iCurOff == 1);
				*lpCur |= pixa[i%2];
				lpCur++;
				iCurOff = 0;
			}
		}
		break;
	case	2:		// �ƶ���ָ����
		lpCur = lpDesData;
		iCurOff = num;
		break;
	case	3:		// ��������
		lpDes = NULL;
		lpCur = lpDes;
		iCurOff = 0;
		break;
	case	4:		// ��ȡ��ǰ��ַ
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	return lpCur;
}



// ��8λ������д���أ�ֻ��_from_8_RLE_read()�����ڲ����ã�
// �β�mark���⣺0-��ʼ������  1-д����  2-�ƶ���ָ����  3-��������  4-��ȡ��ǰ��ַ
PBYTE _write2buff8RLE(int mark, int num, BYTE pix, PBYTE lpDesData)
{
	static PBYTE lpDes;		// ����������ַ
	static PBYTE lpCur;		// ��������ǰд��ַ
	int i;

	switch(mark)
	{
	case	0:		// ��ʼ������
		lpDes = lpDesData;
		lpCur = lpDes;
		break;
	case	1:		// д����
		for (i=0;i<num;i++)
			*lpCur++ = pix;
		break;
	case	2:		// �ƶ���ָ����
		lpCur = lpDesData;
		break;
	case	3:		// ��������
		lpDes = NULL;
		lpCur = lpDes;
		break;
	case	4:		// ��ȡ��ǰ��ַ
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return lpCur;
}


// �����ɫ��ĳߴ�
int _get_palette_size(int bitcount)
{
	int result;

	switch(bitcount)
	{
	case	1:
		result = 2;
		break;
	case	4:
		result = 16;
		break;
	case	8:
		result = 256;
		break;
	case	16:
	case	24:
	case	32:
		result = 0;
		break;
	default:
		ASSERT(FALSE);	// �Ƿ����
		result = 0;
		break;
	}
	return result;
}

