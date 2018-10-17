/********************************************************************

	ce.h

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
			http://iseeexplorer.cosoft.org.cn
			
	���ŵ���

			yzfree##sina.com
	----------------------------------------------------------------
	���ļ���;��	ISee ͼ��ת������
	���ļ���д�ˣ�	YZ			yzfree##sina.com

	���ļ��汾��	20416
	����޸��ڣ�	2002-4-16

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
		��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��

		2002-4		������ʽת����������ɫ�����ص���BUG����������
					С���Ա tearshark��
		2001-12		С���Ա menghui ���ָ�ʽת��������ȱ�ٶԶ�ҳ
					ͼ���֧�֡�����BUG��δ������
		2001-7		��һ�����԰淢��


********************************************************************/



#ifndef __CE_INC__
#define __CE_INC__


#ifndef WIN32
#if defined(_WIN32) || defined(_WINDOWS)
#define WIN32
#endif
#endif /* WIN32 */


#ifdef WIN32	/* Windows OS */

#	ifdef CE_EXPORTS
#	define CE_API __declspec(dllexport)
#	else
#	define CE_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux OS */

#	define CE_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"
#endif


#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif


CE_API void CALLAGREEMENT _1_to_cross(LPINFOSTR psou);
CE_API void CALLAGREEMENT _4_to_cross(LPINFOSTR psou);

CE_API int CALLAGREEMENT _v_conv(LPINFOSTR psou);

CE_API LPINFOSTR CALLAGREEMENT _23to4(LPINFOSTR psou);
CE_API LPINFOSTR CALLAGREEMENT _567to8(LPINFOSTR psou);

CE_API LPINFOSTR CALLAGREEMENT _9_10_11_12_13_14_15to16(LPINFOSTR psou);
CE_API LPINFOSTR CALLAGREEMENT _17_18_19_20_21_22_23to24(LPINFOSTR psou);
CE_API LPINFOSTR CALLAGREEMENT _25_26_27_28_29_30_31to32(LPINFOSTR psou);


#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __CE_INC__ */
