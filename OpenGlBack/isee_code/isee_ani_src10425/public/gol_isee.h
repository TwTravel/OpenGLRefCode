/********************************************************************

	gol_isee.h - ISee图像浏览器—图像读写模块全局定义文件
          
    版权所有(C) VCHelp-coPathway-ISee workgroup 2000 all member's

    这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用公共许
	可证条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根
	据你的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用公共许可证。

    你应该已经和程序一起收到一份GNU通用公共许可证的副本（本目录
	GPL.TXT文件）。如果还没有，写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	如果你在使用本软件时有什么问题或建议，用以下地址可以与我们取得联
	系：
		http://isee.126.com
		http://www.vchelp.net
	或：
		iseesoft@china.com

	编写人：YZ, xiaoyueer, orbit

	文件版本：
		Build 01016
		Date  2000-10-16

********************************************************************/



#ifndef __GOL_ISEE_INC
#define __GOL_ISEE_INC


// 接口版本号
#define	ISEE_IFVER		8

#define ISEE_IMAGE_BITCOUNT	32

#define DPK_ANNEXINFOSIZE	512
#define DPK_FILENAMESIZE	512

// 图像信息数据类型定义
//************************************************************

// 图像读写模块操作命令
enum COMMAND
{
	CMD_NULL,			// 保留
	CMD_GETPROCTYPE,	// 获取本模块能处理的图像类型，如：BMP，PCX
	CMD_GETWRITERS,		// 获取本模块的作者列表，多人时用逗号分隔
	CMD_GETWRITERMESS,	// 获取作者们的留言
	CMD_GETBUILDID,		// 获取图像模块内部版本号
	CMD_IS_VALID_FILE,	// 判断指定文件是否能被本模块处理
	CMD_GET_FILE_INFO,	// 获取指定文件的信息
	CMD_LOAD_FROM_FILE,	// 从指定图像文件中读取数据
	CMD_SAVE_TO_FILE,	// 将数据保存到指定文件中
	CMD_IS_SUPPORT,		// 查询某个命令是否被支持
	CMD_RESIZE,			// 重新获取指定尺寸的图像位数据（只适用于矢量图像）
	//CMD_XXXXXX,		// 新命令将在此插入
	CMD_MAX				// 边界值
};


// 图像读写模块执行结果
enum EXERESULT
{
	ER_SUCCESS,			// 执行成功
	ER_FILERWERR,		// 文件在读写过程中发生错误（系统、硬件等原因引起文件读写异常）
	ER_MEMORYERR,		// 内存不足，导致操作失败
	ER_ILLCOMM,			// 非法指令
	ER_COMMINFOERR,		// 与命令有关的信息不足或有缺损
	ER_SYSERR,			// 操作系统不稳定，无法执行命令
	ER_NOTSUPPORT,		// 不支持的命令
	ER_USERBREAK,		// 操作被用户中断
	ER_EMPTY,			// 初始状态值，用于客户方初始化返回变量
	//@@@@@@@@@@Added By YZ 2000.8.21
	ER_BADIMAGE, 		// 受损的图像文件
	//@@@@@@@@@@End 2000.8.21
	//ER_XXXX,			// 新的错误信息在此插入
	ER_MAX				// 边界值
};

// 图像类型
enum IMGTYPE
{
	IMT_NULL,			// 无效图像类型
	IMT_RESSTATIC,		// 光栅、静态图像
	IMT_RESDYN,			// 光栅、动态图像
	IMT_VECTORSTATIC,	// 矢量、静态图像
	IMT_VECTORDYN,		// 矢量、动态图像
	//IMT_XXXXX,		// 在此插入新的图像类型
	IMT_MAX				// 边界值
};

// 图像文件格式
enum IMGFORMAT
{
	IMF_NULL,		// 未知或无效的图像文件
	IMF_BMP,		// BMP格式的图像文件
	IMF_CUR,		// ....
	IMF_ICO,
	IMF_PCX,
	IMF_DCX,
	IMF_GIF,
	IMF_PNG,
	IMF_TGA,
	IMF_WMF,
	IMF_EMF,
	IMF_TIFF,
	IMF_JPEG,		//
	IMF_PSD,		// PSD格式的图像文件
	//IMF_XXX		// 新增格式在此处插入
	IMF_MAX			// 边界值
};

// 数据包当前状态
enum PACKSTATE
{
	PKST_NULL,			// 数据包为空
	PKST_NOTVER,		// 指定文件还未通过验证的阶段（此时只有文件名变量被设置）
	PKST_PASSVER,		// 指定的文件已通过验证，是有效格式的文件
	PKST_PASSINFO,		// 已通过验证，且数据包中有该文件的相关信息
	PKST_INFOANDBITS,	// 已通过验证，且有该文件的相关信息，并含有图像的位数据
	PKST_MODIFY,		// 数据包中的图像已经被修改过
	//PKST_XXXXX,		// 插入新的状态
	PKST_MAX			// 边界值
};


// 图像的存储方式
enum IMGCOMPRESS
{
	ICS_RGB = 0,			// 未经压缩的DIB存储方式(ARGB)
	ICS_RLE8,				// RLE8存储方式
	ICS_RLE4,				// RLE4存储方式
	ICS_BITFIELDS,			// 分位存储方式
	ICS_PCXRLE,				// PCX RLE存储方式
	ICS_GIFLZW,				// GIF LZW存储方式
	ICS_GDIRECORD,			// WMF的元文件存储方式
	//@@@@@@@@@@Added By xiaoyueer 2000.8.20
	ICS_RLE32,				// RLE32存储方式
	ICS_RLE24,				// RLE24存储方式
	ICS_RLE16,				// RLE16存储方式
	//@@@@@@@@@End 2000.8.20
	//@@@@@@@@@@Added By orbit 2000.8.31
	ICS_JPEGGRAYSCALE,		// 灰度图象,JPEG只支持256级灰度
	ICS_JPEGRGB,			//red/green/blue 
	ICS_JPEGYCbCr,			//Y/Cb/Cr (also known as YUV)
	ICS_JPEGCMYK,			//C/M/Y/K
	ICS_JPEGYCCK,			//Y/Cb/Cr/K
	//@@@@@@@@@End 2000.8.31
	//@@@@@@@@@@LibTiff: Sam Leffler 2001.1.6
	ICS_TIFF_NONE,			// TIFF non-compress ABGR mode
	ICS_TIFF_CCITTRLE,		// CCITT modified Huffman RLE
	ICS_TIFF_CCITTAX3,		// CCITT Group 3 fax encoding
	ICS_TIFF_CCITTFAX4,		// CCITT Group 4 fax encoding
	ICS_TIFF_LZW,			// Lempel-Ziv  & Welch
	ICS_TIFF_OJPEG,			// 6.0 JPEG
	ICS_TIFF_JPEG,			// JPEG DCT compression
	ICS_TIFF_NEXT,			// NeXT 2-bit RLE
	ICS_TIFF_CCITTRLEW,		// 1 w/ word alignment
	ICS_TIFF_PACKBITS,		// Macintosh RLE
	ICS_TIFF_THUNDERSCAN,	// ThunderScan RLE
	//@@@@@@@@@@Added By <dkelly@etsinc.com>
	ICS_TIFF_IT8CTPAD,		// IT8 CT w/padding
	ICS_TIFF_IT8LW,			// IT8 Linework RLE
	ICS_TIFF_IT8MP,			// IT8 Monochrome picture
	ICS_TIFF_IT8BL,			// IT8 Binary line art
	ICS_TIFF_PIXARFILM,		// Pixar companded 10bit LZW
	ICS_TIFF_PIXARLOG,		// Pixar companded 11bit ZIP
	ICS_TIFF_DEFLATE,		// Deflate compression
	ICS_TIFF_ADOBE_DEFLATE,	// Deflate compression, as recognized by Adobe
	//@@@@@@@@@@Added By <dev@oceana.com>
	ICS_TIFF_DCS,			// Kodak DCS encoding
	ICS_TIFF_JBIG,			// ISO JBIG
	ICS_TIFF_SGILOG,		// SGI Log Luminance RLE
	ICS_TIFF_SGILOG24,		// SGI Log 24-bit packed
	//@@@@@@@@@End 2001.1.6
	//ICS_XXXX,				// 插入新的存储方式类型
	ICS_UNKONW = 1024,		// 未知的压缩方式
	ICS_MAX = 2048			// 边界
};



							
// 数据结构定义
//************************************************************

// 标准图像格式信息头结构
struct _tagINTERBITMAPINFO;
typedef _tagINTERBITMAPINFO	INTERBITMAPINFO;
typedef _tagINTERBITMAPINFO	*LPINTERBITMAPINFO;

// 图像信息结构（对于多帧的图像，以下结构只用来描述第一帧）
struct _tagIMAGEINFOSTR;
typedef _tagIMAGEINFOSTR  IMAGEINFOSTR;
typedef _tagIMAGEINFOSTR  *LPIMAGEINFOSTR;

// 图像读写数据包结构
struct _tagINFOSTR;
typedef _tagINFOSTR  INFOSTR;
typedef _tagINFOSTR  *LPINFOSTR;

// 多桢图像的子图像块结构（主要用于动画类图像文件）
struct _tagSUBIMGBLOCK;
typedef _tagSUBIMGBLOCK	SUBIMGBLOCK;
typedef _tagSUBIMGBLOCK	*LPSUBIMGBLOCK;

// 开发人员信息
struct _tagDeveloperInfo;
typedef _tagDeveloperInfo DEVELOPERINFO;
typedef _tagDeveloperInfo *LPDEVELOPERINFO;

// IRW插件附加内容结构
struct _tagPlugsIn;
typedef _tagPlugsIn	PLUGSIN;
typedef _tagPlugsIn *LPPLUGSIN;



// 多桢图像的子图像块结构
struct _tagSUBIMGBLOCK
{
	int		number;				// 子图像块的序列号
	int		left, top;			// 子图像块的显示位置（左上角）
	int		width, height;		// 子图像块的图像宽度及高度
	int		dowith;				// 处置方法（针对于动画GIF）
	int		userinput;			// 用户输入（针对于动画GIF）
	COLORREF colorkey;			// 透明色分量值
	int		time;				// 显示的延迟时间

	unsigned long	  *(*pLineAddr);// 标准图像位数据中从0行到n-1行的每一行的首地址
	unsigned char		*_pbdata;	// 标准图像位数据缓冲区首地址
	
	LPINFOSTR		parents;	// 第一桢图像数据的结构地址（所属数据包结构的地址）
	LPSUBIMGBLOCK	prev;		// 前一桢图像数据的结构地址
	LPSUBIMGBLOCK	next;		// 后一桢图像数据的结构地址（NULL表示这是最后一副图像）
};


// 标准图像格式信息头结构
struct _tagINTERBITMAPINFO
{
	BITMAPINFOHEADER	bmi;	// 标准图像格式的信息结构
	DWORD	rmask;				// 红色掩码
	DWORD	gmask;				// 绿色掩码
	DWORD	bmask;				// 蓝色掩码
	DWORD   alphaMask;			// Alpha掩码
};


// 图像信息结构（对于多帧的图像，以下结构只用来描述背景图像）
struct _tagIMAGEINFOSTR
{
	IMGTYPE		imgtype;			// 图像类型
	IMGFORMAT	imgformat;			// 图像文件格式
	//@@@@@@@@@@@@@Added By orbit for file's saving function at 2000.8.31
	BYTE		filename[DPK_FILENAMESIZE];//图象的原始文件名（另存为命令时保存原始文件的名字）。
	int			qualtify;			// 图象质量（只适用于JPEG文件）
	//@@@@@@@@@@@@End 2000.8.31
	unsigned long	filesize;		// 图像文件的长度（以字节为单位）
	unsigned long	width;			// 图像宽度
	unsigned long	height;			// 图像高度
	unsigned long	bitcount;		// 像素位数（对应于颜色数）
	IMGCOMPRESS		compression;	// 图像的存储方式
	unsigned long	linesize;		// 每一图像行所占的字节数（DWORD对齐，只针对BMP文件）
	unsigned long	imgnumbers;		// 该文件中图像的个数（普通图像文件此值恒定为1，而在动画GIF中可能会大于1）
	unsigned int	imgchang;		// 该文件是否可被编辑（0-表示可以被编辑，1-表示不能被编辑。比如动画GIF就不能被编辑）
	unsigned short	year;			// 图像文件最后修改的年
	unsigned short	monday;			// 图像文件最后修改的月和日（月在高字节，日在低字节）
	unsigned long	time;			// 图像文件最后修改的时间（字序：最高—0, 2—时，1—分，0—秒）
};



// 图像读写数据包结构
struct _tagINFOSTR
{
	COMMAND		comm;				// 操作命令
	EXERESULT	result;				// 执行的结果（是成功了还是失败了）
	union {
		char	cAnnData;			// 联合，用于描述不同类型的数据，
		int		iAnnData;			// 是操作命令与执行结果的附加信息，
		long	lAnnData;			// 参见每个命令中对此值的含意说明
		float	fAnnData;
		double	dfAnnData;
		COMMAND	cmAnnData;			// 检察命令支持情况时用到
		unsigned short 	wAnnData;
		unsigned long 	dwAnnData;
		void			*pAnnData;
		unsigned int	siAnnData[DPK_ANNEXINFOSIZE/sizeof(int)];
		unsigned char	scAnnData[DPK_ANNEXINFOSIZE];
	} annexdata;

	IMAGEINFOSTR imginfo;			// 图像文件信息
	char filename[DPK_FILENAMESIZE];// 图像文件的路径及文件名		
	void *pImgInfo;					// 原始图像信息

	INTERBITMAPINFO		sDIBInfo;	// 标准图像格式信息结构
	unsigned long	  *(*pLineAddr);// 标准图像位数据中从0行到n-1行的每一行的首地址
	unsigned char		*_pbdata;	// 标准图像位数据缓冲区首地址

	PACKSTATE	state;				// 数据包当前的状态
	int			modify;				// 修改标记
	LPSUBIMGBLOCK psubimg;			// 子图像数据块地址（静态图像此值应设为NULL）
	int (WINAPI *fpProgress)(int,int);
									// 由调用者提供的进度条显示函数指针（可以是NULL）
									// 入口参数1是总的进度数，参数2是当前的进度数。
									// 返回参数：如果返回0，则图像读写模块继续操作，
									// 如果返回非0值，则读写模块终止操作，返回数据包。
};


// 开发者信息结构
struct _tagDeveloperInfo
{
	BYTE	modulename[16];		// 代码名称
	BYTE	author[16];			// 作者名字（多人时用逗号分隔）
	BYTE	EMail[32];			// 作者电邮地址（多人时用逗号分隔）
	BYTE	messages[144];		// 作者的留言信息
};



// 插件附加内容结构定义
struct _tagPlugsIn
{
	BYTE	IRWID[4];			// IRW插件标识符，恒定为字符"IRWP"
	WORD	attrib;
	WORD	version;
	DWORD	function;			// 0位是读支持标志，
								// 1位是写支持标志，
								// 2位是"指定尺寸"的读支持标志，(ReSize)
								// 其它未用

	DWORD	proctypenum;
	BYTE	proctypestr[32];	// 4*8 ，八组，每组4字节

	BYTE	author[16];
	BYTE	EMail[32];

	BYTE	messages[144];

	BYTE	modulename[16];		// 模块的名字
};	// 共256个字节

#endif	//__GOL_ISEE_INC