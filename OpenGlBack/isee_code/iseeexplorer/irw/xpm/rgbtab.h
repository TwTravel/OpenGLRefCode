/********************************************************************

	rgbtab.h

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
	���ļ���;��	ISeeͼ���������XPMͼ���дģ���ɫ�����ļ�

	���ļ���д�ˣ�	YZ			yzfree##yeah.net

	���ļ��汾��	20125
	����޸��ڣ�	2002-1-25

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

			2002-1		��һ�η���


********************************************************************/


#ifndef __XPM_RGBTAB_INC__
#define __XPM_RGBTAB_INC__


#ifdef WIN32	/* Windows */
#	define CALLAGREEMENT	__cdecl
#else			/* Linux */
#	define CALLAGREEMENT
#endif	/*WIN32*/


/* XPM Ԥ������ɫ����ṹ */
typedef struct _tagXPMRGBTAB
{
	unsigned long	rgba;		/* RGB��ɫֵ */
	int 			iqv;		/* ���ƴ����ַ�ASCIIֵ��ӽ�� */
	char			*name;		/* ���ƴ� */
} XPMRGBTAB, LPXPMRGBTAB;


/* ��ɫ�����ܸ��� */
#define RGBTABSIZE 	752


/* XPM ͼ����ɫ�ṹ */
typedef struct _tagXPMCOLOR
{
	char			pix[8];		/* ���������������Ϊ7���� */
	unsigned long	rgb;		/* RGBA���� */
	int				attrib;		/* ��ɫ���ԣ�0����ͨ��ɫ��1��͸��ɫ */
} XPMCOLOR, *LPXPMCOLOR;



#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

/* �ڲ��������� */
int CALLAGREEMENT _search_color(char *name, unsigned long *pcol);
int CALLAGREEMENT _color_to_rgb(char *name, int len, unsigned long *rgb);
int CALLAGREEMENT _search_pix(LPXPMCOLOR pcol, int ncol, char *pix, int cpp);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif

#endif  /* __XPM_RGBTAB_INC__ */
