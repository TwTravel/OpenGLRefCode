/********************************************************************

	gol_proc.h - ISee图像浏览器—图像处理接口全局定义文件
          
    版权所有(C) VCHelp-coPathway-ISee workgroup 2000 all member's

    这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用许可证
	条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根据你
	的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用许可证。

    你应该已经和程序一起收到一份GNU通用许可证(GPL)的副本。如果还没有，
	写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	如果你在使用本软件时有什么问题或建议，用以下地址可以与我们取得联
	系：
		http://isee.126.com
		http://www.vchelp.net
	或：
		iseesoft@china.com

	编写人：临风

	文件版本：
		Build 00606
		Date  2000-6-6

********************************************************************/



#ifndef __GOL_PROC_INC
#define __GOL_PROC_INC


// 接口版本号
#define	ISEE_PRVER			6

// 图像处理数据包附加参数尺寸
#define PPK_ANNEXINFOSIZE	512

//图像处理模块入口函数返回值
#define PROCERR_SUCCESS			1        //成功
#define PROCERR_FALSE			0        //失败，一般错误，须检查result值
#define PROCERR_FAIL				-1       //失败，一般是调用对话框失败，系统错误等，是大多数WIN API函数失败返回值。
#define PROCERR_NULLPARAM		-2       //传入参数为NULL指针

// 图像处理数据包定义
//************************************************************

// 图像处理模块操作命令
enum PROCCOMM
{
	PCM_NULL,			   // 未知或无效的命令
	PCM_GETWRITERS,		// 获取作者名单，多于一个人时用逗号分隔姓名
	PCM_GETWRITERMESS,	// 获取作者们的留言
	PCM_GETBUILDID,		// 获取图像模块内部版本号
	
	PCM_PROP,            // 图像属性处理
	PCM_RESIZE,          // 重定义尺寸	
	PCM_ROTATE,			   // 旋转
	PCM_SPLIT,           // 分割
	PCM_SPLICE,          // 拼接
	PCM_GREYSCALE,       // 灰度转换
	PCM_REFINECOLOR,     // 真彩，高彩，256色转换
	//PCM_XXXXX,         // 图像属性处理命令在此插入
	
	PCM_MASS,            // 图像质量处理
	PCM_FILTER,          // 杂色处理（滤波）
	PCM_CONTRAST,        // 明暗，对比度处理
	PCM_DETECTEDGE,      // 边缘探测
	//PCM_XXXXX,         // 图像质量处理命令在此插入
	
	PCM_STEFF,           // 图像静态效果处理
	PCM_EMBOSS,          // 浮雕效果
	PCM_VARIATIONS,      // 色调处理
	PCM_BLUR,            // 模糊
	PCM_BUTTON,          // 按钮化处理
	PCM_TWIRL,           // 扭曲
	PCM_INTERLACE,       // 抽丝
	PCM_MOSAIC,          // 马赛克
	//PCM_XXXXX,         // 图像静态效果处理命令在此插入
	
	PCM_ANIEFF,          // 图像动态效果处理
	PCM_FADEIN,          // 叠映
	PCM_ALPHASCROLL,     // ALPHA卷轴
	PCM_BURNUP,          // 火焰吞噬
	//PCM_XXXXX,         // 图像动态效果处理命令在此插入
	PCM_MAX				   // 边界值
};


// 图像处理模块执行结果
enum PROCRESULT
{
	PR_SUCCESS,			// 操作成功
	PR_FAIL,			// 操作失败
	PR_NULL,			// 未操作
	PR_MEMORYERR,		// 内存不足，导致操作失败
	PR_ILLCOMM,			// 非法指令（客户程序给出的命令码不正确）
	PR_COMMINFOERR,		// 与命令有关的信息不足或有缺损
	PR_SYSERR,			// 操作系统此时不稳定，无法执行命令
	//PR_XXXX,			// 新的错误信息在此插入
	PR_NOTEXE,			// 命令没有被执行，表示图像处理模块不支持该命令
	PR_MAX				// 边界值
};


// 图像处理数据包状态
enum IPDPCKSTATE
{
	IPDPS_NULL,			// 当前数据包为空
	IPDPS_CONFIG,     // 调用动态处理设置部分
	IPDPS_RUN,        // 调用动态处理运行部分
	//IPDPS_XXXXX,		// 插入新的状态
	IPDPS_MAX			// 边界值
};




// 数据结构定义
//************************************************************

// 图像操作数据包
struct _tagIMAGEPROCSTR;
typedef _tagIMAGEPROCSTR  IMAGEPROCSTR;
typedef _tagIMAGEPROCSTR  *LPIMAGEPROCSTR;

// 图像位数据相关信息
struct _tagIMAGEPROCDATAINFO;
typedef _tagIMAGEPROCDATAINFO  IMAGEPROCDATAINFO;
typedef _tagIMAGEPROCDATAINFO  *LPIMAGEPROCDATAINFO;




// 图像位数据相关信息
struct _tagIMAGEPROCDATAINFO
{
	int width;			// 图像的宽度（以像素为单位）
	int height;			// 图像的高度（以像素为单位）
	int bitperpix; 	// 图像位数据中每个像素所占位数,最小16，因为无调色板传入
	int byteperline;	// 图像位数据中每个扫描行所占的字节数
	BOOL bAlpha;		// 图像位数据是否带ALPHA通道，如果为TRUE，则bitperpix必须=32，传入图象数据强制转换为COLORREF*类型，色值最高位为ALPHA值，即COLORREF col=0xaarrggbb
};


// 图像操作数据包
struct _tagIMAGEPROCSTR
{
	PROCCOMM	comm;				// 操作命令
	PROCRESULT	result;				// 执行的结果
	union {
		char	cAnnData;			// 联合，用于描述不同类型的数据，
		int		iAnnData;			// 是操作命令与执行结果的附加信息
		long	lAnnData;
		float	fAnnData;
		double	dfAnnData;
		unsigned short 	wAnnData;
		unsigned long 	dwAnnData;
		void			*pAnnData;
		unsigned int	siAnnData[PPK_ANNEXINFOSIZE/sizeof(int)];
		unsigned char	scAnnData[PPK_ANNEXINFOSIZE];
	} annexdata;

	HWND		hParentWnd;			// 父窗口句柄
	HINSTANCE	hInst;				// 图像处理模块实例句柄

	IMAGEPROCDATAINFO sImageInfo;	// 源图像位数据信息
	unsigned long  *(*psLineAddr);	// 源标准图像位数据中从0行到n-1行的每一行的首地址
	unsigned char		*_psbdata;	// 源标准图像位数据缓冲区首地址

	IMAGEPROCDATAINFO dImageInfo;	// 目标图像位数据信息
	unsigned long  *(*pdLineAddr);	// 目的标准图像位数据中从0行到n-1行的每一行的首地址
	unsigned char		*_pdbdata;	// 目的标准图像位数据缓冲区首地址
									// （注：目标图像内存块将由图像处理模块分配，并填写dImageInfo变量）
	
	IPDPCKSTATE	state;				// 数据包当前的状态
	int			modify;				// 修改标志
};


#endif	//__GOL_PROC_INC