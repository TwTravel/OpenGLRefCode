/********************************************************************

	iseecompress.h

	----------------------------------------------------------------
    ������֤ �� GPL
	��Ȩ���� (C) 2001 VCHelp coPathway ISee workgroup.
	----------------------------------------------------------------
	��һ����������������������������������������GNU ͨ�ù�����
	��֤�������޸ĺ����·�����һ���򡣻��������֤�ĵڶ��棬���ߣ���
	�����ѡ�����κθ��µİ汾��

    ������һ�����Ŀ����ϣ�������ã���û���κε���������û���ʺ��ض�
	Ŀ�ص������ĵ���������ϸ����������GNUͨ�ù������֤��

    ��Ӧ���Ѿ��ͳ���һ���յ�һ��GNUͨ�ù������֤�ĸ�������Ŀ¼
	GPL.txt�ļ����������û�У�д�Ÿ���
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA
	----------------------------------------------------------------
	�������ʹ�ñ����ʱ��ʲô������飬�������µ�ַ������ȡ����ϵ��

			http://isee.126.com
			http://www.vchelp.net
			http://www.chinafcu.com

	���ŵ���

			yzfree##sina.com
	----------------------------------------------------------------
	���ļ���;��	ISeeͼ���������ѹ������ѹģ��(X-Todo A03)
	
	���ļ���д�ˣ�	wenbobo		wenbobo##263.net
		
	���ļ��汾��	11207
	����޸��ڣ�	2001-12-07
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		  2001-12		��һ�η���


	----------------------------------------------------------------
	ע��������ʹ����UCL(ʵʱ����ѹ����)���ڴ���UCL�Ŀ������Ǳ�ʾ��л��
	----------------------------------------------------------------
  
********************************************************************/


#ifdef WIN32	/* Windows */

#include <windows.h>

#	define CALLAGREEMENT	__cdecl
#	ifndef _DEUBG
#	ifndef NDEBUG
#	define NDEBUG
#	endif
#	endif

#else			/* Linux */

typedef unsigned char * LPBYTE;
typedef unsigned long   DWORD;

#	define CALLAGREEMENT
#	ifndef NDEBUG
#	ifndef _DEBUG
#	define _DEUBG
#	endif
#	endif

#endif	/*WIN32*/


extern int isc_compresslevel;


/* CI */
int CALLAGREEMENT ISeeCompress(LPBYTE, DWORD, LPBYTE *, DWORD *);
int CALLAGREEMENT ISeeDecompress(LPBYTE, DWORD, LPBYTE *, DWORD);

