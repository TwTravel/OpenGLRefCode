/********************************************************************

	allolink.h

	----------------------------------------------------------------
    ������֤ �� GPL
	��Ȩ���� (C) 2002 VCHelp coPathway ISee workgroup.
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
			http://cosoft.org.cn/projects/iseeexplorer

	���ŵ���

			isee##vip.163.com
	----------------------------------------------------------------
	���ļ���;��	ISeeͼ����������ڴ����ϵͳ�ѷ����ڴ����������ļ�
	
	  
	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net
		
	���ļ��汾��	20618
	����޸��ڣ�	2002-6-18
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		2002-6		�ڶ��淢��
		2002-5		��һ�淢��
			  
********************************************************************/



#ifndef __ALLOLINK_INC__
#define __ALLOLINK_INC__


/* �ѷ����ڴ��ڵ��ǩ */
#define IM_ALLOLINK_MAGIC			(0x474d4c41)			/* ��ALMG�� */


typedef struct _tagAlloLinkNode		ALLOLINKNODE;
typedef struct _tagAlloLinkNode		*LPALLOLINKNODE;

/* �ѷ����ڴ�����ڵ� */
struct _tagAlloLinkNode
{
	unsigned long	magic;				/* ��ǩ */

	unsigned char	*guard_addr;		/* ����ҳ��ʼ��ַ */
	unsigned long	guard_length;		/* ��ռ�����ڴ�ҳ��������ͬǰ�÷���ҳ��*/

	LPFGUARDSTRUCT	fg_addr;			/* �ύ�ڴ����ַ��Ҳ��ǰ�����ṹ��ַ�� */
	LPBGUARDSTRUCT	bg_addr;			/* ������ṹ��ַ */
	unsigned long	begin_length;		/* �����ݳ��ȣ�����ǰ��������ṹ���û����ݣ�*/

	unsigned char	*user_addr;			/* �û����ݻ���ַ */
	unsigned long	user_length;		/* �û����ݿ鳤�ȣ��ֽڵ�λ��*/
	
	unsigned char	*filename;			/* ������ڴ���Դ�������ļ� */
	unsigned long	linenum;			/* ������ڴ���Դ�������к� */

	unsigned long	attrib;				/* ��Ԫ���ԣ�0���ɶ�д����0��ֻ����*/

	unsigned long	rev0;				/* �����������Ժ���չ */
	unsigned long	rev1;
	unsigned long	rev2;
	
	LPALLOLINKNODE	prev;				/* ǰһ��������ַ */
	LPALLOLINKNODE	next;				/* ��һ��������ַ */
}; /* 64�ֽ� */




#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif
	
	
void CALLAGREEMENT ima_create(unsigned char *, int, HANDLE);
void CALLAGREEMENT ima_destroy(void);
	
LPALLOLINKNODE CALLAGREEMENT ima_push(unsigned char *, int, int, long,unsigned char *, unsigned long);
LPALLOLINKNODE CALLAGREEMENT ima_search(unsigned char *);
void CALLAGREEMENT ima_pop(LPALLOLINKNODE);

int  CALLAGREEMENT ima_get_count(void);

	
#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif



#endif	/* __ALLOLINK_INC__ */
