/********************************************************************

	greyscale.cpp - ISeeͼ���������ͼ����ģ��ͼ��Ҷ�ת������ʵ�ִ����ļ�

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

   ����ʵ�֣�ͼ��Ҷ�ת��

	�ļ��汾��
		Build 00617
		Date  2000-6-17

********************************************************************/

#include "p_win.h"
#include "gol_proc.h"
#include "filter.h"

//�Ҷ�ת��
int DoGreyScale(LPIMAGEPROCSTR lpInfo)
{
	//�����ڴ�ͼ��
   lpInfo->_pdbdata = (unsigned char*)New(lpInfo->sImageInfo.width*lpInfo->sImageInfo.height*(lpInfo->sImageInfo.bitperpix/8));
   if(lpInfo->_pdbdata==NULL)
   {
   	lpInfo->result=PR_MEMORYERR;
   	return PROCERR_FALSE;
   }

   //��д�½�ͼ����Ϣ
   lpInfo->dImageInfo=lpInfo->sImageInfo;

	LPBYTE pRed=(LPBYTE)lpInfo->_psbdata,pdBuf=(LPBYTE)lpInfo->_pdbdata;
   LPBYTE pGrn=pRed+1, pBlu=pRed+2;
	UINT loop=(UINT)(lpInfo->sImageInfo.width*lpInfo->sImageInfo.height);
   int lum;
   int skip=lpInfo->sImageInfo.bitperpix/8;

   //�Ҷ�ת��(361ת��)
   for (UINT i=0;i<loop;i++)
   {
		lum = (int)(.299 * (double)(*pRed) + .587 * (double)(*pGrn) + .114 * (double)(*pBlu));

		*pdBuf = (BYTE)lum;
      pdBuf++;
      *pdBuf = (BYTE)lum;
      pdBuf++;
      *pdBuf = (BYTE)lum;
      pdBuf+=(skip-2);

      pRed+=skip;
      pGrn+=skip;
      pBlu+=skip;
	}

   lpInfo->result=PR_SUCCESS;
   return PROCERR_SUCCESS;
}

