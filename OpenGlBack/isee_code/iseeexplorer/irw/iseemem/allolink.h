/********************************************************************

	allolink.h

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
	本文件用途：	ISee图像浏览器—内存防护系统已分配内存链管理定义文件
	
	  
	本文件编写人：	
					YZ			yzfree##yeah.net
		
	本文件版本：	20618
	最后修改于：	2002-6-18
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		2002-6		第二版发布
		2002-5		第一版发布
			  
********************************************************************/



#ifndef __ALLOLINK_INC__
#define __ALLOLINK_INC__


/* 已分配内存块节点标签 */
#define IM_ALLOLINK_MAGIC			(0x474d4c41)			/* “ALMG” */


typedef struct _tagAlloLinkNode		ALLOLINKNODE;
typedef struct _tagAlloLinkNode		*LPALLOLINKNODE;

/* 已分配内存链表节点 */
struct _tagAlloLinkNode
{
	unsigned long	magic;				/* 标签 */

	unsigned char	*guard_addr;		/* 防护页起始地址 */
	unsigned long	guard_length;		/* 所占虚拟内存页总数（连同前置防护页）*/

	LPFGUARDSTRUCT	fg_addr;			/* 提交内存基地址（也即前防护结构地址） */
	LPBGUARDSTRUCT	bg_addr;			/* 后防护结构地址 */
	unsigned long	begin_length;		/* 总数据长度（包括前、后防护结构及用户数据）*/

	unsigned char	*user_addr;			/* 用户数据基地址 */
	unsigned long	user_length;		/* 用户数据块长度（字节单位）*/
	
	unsigned char	*filename;			/* 分配此内存块的源码所在文件 */
	unsigned long	linenum;			/* 分配此内存块的源码所在行号 */

	unsigned long	attrib;				/* 单元属性（0－可读写，非0－只读）*/

	unsigned long	rev0;				/* 保留，用于以后扩展 */
	unsigned long	rev1;
	unsigned long	rev2;
	
	LPALLOLINKNODE	prev;				/* 前一个分配块地址 */
	LPALLOLINKNODE	next;				/* 后一个分配块地址 */
}; /* 64字节 */




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
