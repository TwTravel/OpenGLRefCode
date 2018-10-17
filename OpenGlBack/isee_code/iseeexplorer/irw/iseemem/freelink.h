/********************************************************************

	freelink.h

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
	���ļ���;��	ISeeͼ����������ڴ����ϵͳ�����ڴ����������ļ�
	
	  
	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net
		
	���ļ��汾��	20626
	����޸��ڣ�	2002-6-26
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		  2002-6		�ڶ��淢��
		  2002-5		��һ�淢��

********************************************************************/


#ifndef __FREELINK_INC__
#define __FREELINK_INC__


/* �����ڴ��ڵ��ǩ */
#define IM_FREELINK_MAGIC			(0x474d4c46)			/* ��FLMG�� */


typedef struct _tagFreeLinkNode		FREELINKNODE;
typedef struct _tagFreeLinkNode		*LPFREELINKNODE;

/* �����ڴ�����ڵ� */
struct _tagFreeLinkNode
{
	unsigned long	magic;				/* ��ǩ */
	unsigned char	*begin_addr;		/* ���п����ַ */
	unsigned long	len_in_byte;		/* ���п鳤�ȣ��ֽڵ�λ��*/
	unsigned long	len_in_page;		/* ���п鳤�ȣ�ҳ��λ��*/

	unsigned long	rev0;				/* �����������Ժ���չ */
	unsigned long	rev1;
	unsigned char	rev[32];			/* ���������뵽64�ֽ� */

	LPFREELINKNODE	prev;				/* ǰһ�����п��ַ */
	LPFREELINKNODE	next;				/* ��һ�����п��ַ */
}; /* 64�ֽ� */




#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif
	

void  CALLAGREEMENT imf_create(unsigned char *pstart_addr, int page_size, HANDLE heap);
void CALLAGREEMENT imf_destroy(void);

unsigned char * CALLAGREEMENT imf_lookup(int page_count);
void CALLAGREEMENT imf_reclaim(unsigned char *paddr, int page_count);


#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif	/* __FREELINK_INC__ */
