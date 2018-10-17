/********************************************************************

	rgbtab.h

	----------------------------------------------------------------
    软件许可证 － GPL
	版权所有 (C) 2001 VCHelp coPathway ISee workgroup.
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
			http://www.vchelp.net
			http://www.chinafcu.com

	或发信到：

			yzfree##sina.com
	----------------------------------------------------------------
	本文件用途：	ISee图像浏览器—XPM图像读写模块彩色表定义文件

	本文件编写人：	YZ			yzfree##yeah.net

	本文件版本：	20125
	最后修改于：	2002-1-25

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------
	修正历史：

			2002-1		第一次发布


********************************************************************/


#ifndef __XPM_RGBTAB_INC__
#define __XPM_RGBTAB_INC__


#ifdef WIN32	/* Windows */
#	define CALLAGREEMENT	__cdecl
#else			/* Linux */
#	define CALLAGREEMENT
#endif	/*WIN32*/


/* XPM 预定义颜色表项结构 */
typedef struct _tagXPMRGBTAB
{
	unsigned long	rgba;		/* RGB颜色值 */
	int 			iqv;		/* 名称串各字符ASCII值相加结果 */
	char			*name;		/* 名称串 */
} XPMRGBTAB, LPXPMRGBTAB;


/* 颜色表项总个数 */
#define RGBTABSIZE 	752


/* XPM 图像颜色结构 */
typedef struct _tagXPMCOLOR
{
	char			pix[8];		/* 象素描述符（最多为7个） */
	unsigned long	rgb;		/* RGBA数据 */
	int				attrib;		/* 颜色属性－0：普通颜色、1：透明色 */
} XPMCOLOR, *LPXPMCOLOR;



#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

/* 内部辅助函数 */
int CALLAGREEMENT _search_color(char *name, unsigned long *pcol);
int CALLAGREEMENT _color_to_rgb(char *name, int len, unsigned long *rgb);
int CALLAGREEMENT _search_pix(LPXPMCOLOR pcol, int ncol, char *pix, int cpp);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif

#endif  /* __XPM_RGBTAB_INC__ */
