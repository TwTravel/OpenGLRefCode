/********************************************************************

	freelink.h

	----------------------------------------------------------------
    软件许可证 － GPL
	版权所有 (C) 2002 VCHelp coPathway ISee workgroup.
	----------------------------------------------------------------
	这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用公共许
	可证条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根
	据你的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用公共许可证。

    你应该已经和程序一起收到一份GNU通用公共许可证的副本（本目录
	GPL.txt文件）。如果还没有，写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA
	----------------------------------------------------------------
	如果你在使用本软件时有什么问题或建议，请用以下地址与我们取得联系：

			http://isee.126.com
			http://cosoft.org.cn/projects/iseeexplorer

	或发信到：

			isee##vip.163.com
	----------------------------------------------------------------
	本文件用途：	ISee图像浏览器—内存防护系统空闲内存链管理定义文件
	
	  
	本文件编写人：	
					YZ			yzfree##yeah.net
		
	本文件版本：	20626
	最后修改于：	2002-6-26
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		  2002-6		第二版发布
		  2002-5		第一版发布

********************************************************************/


#ifndef __FREELINK_INC__
#define __FREELINK_INC__


/* 空闲内存块节点标签 */
#define IM_FREELINK_MAGIC			(0x474d4c46)			/* “FLMG” */


typedef struct _tagFreeLinkNode		FREELINKNODE;
typedef struct _tagFreeLinkNode		*LPFREELINKNODE;

/* 空闲内存链表节点 */
struct _tagFreeLinkNode
{
	unsigned long	magic;				/* 标签 */
	unsigned char	*begin_addr;		/* 空闲块基地址 */
	unsigned long	len_in_byte;		/* 空闲块长度（字节单位）*/
	unsigned long	len_in_page;		/* 空闲块长度（页单位）*/

	unsigned long	rev0;				/* 保留，用于以后扩展 */
	unsigned long	rev1;
	unsigned char	rev[32];			/* 保留，对齐到64字节 */

	LPFREELINKNODE	prev;				/* 前一个空闲块地址 */
	LPFREELINKNODE	next;				/* 后一个空闲块地址 */
}; /* 64字节 */




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
