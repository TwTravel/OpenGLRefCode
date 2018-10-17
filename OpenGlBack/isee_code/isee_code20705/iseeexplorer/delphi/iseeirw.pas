{*******************************************************************************

        iseeirw.pas

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
                        http://iseeexplorer.cosoft.org.cn

        或发信到：

                        yzfree##sina.com
        ----------------------------------------------------------------
        本文件用途：    图像读写模块全局定义文件的 Delphi 申明单元
        本文件名：      iseeirw.pas
        原文件名：      iseeirw.h
        本文件编写人：  与月共舞                yygw##163.com
        原文件编写人：  YZ                      yzfree##sina.com
                                        xiaoyueer               xiaoyueer##263.net
                                        orbit                   Inte2000##263.net
                                        Janhail                 janhail##sina.com

        本文件版本：    20606
        最后修改于：    2002-06-06
        原文件版本：    20422
        最后修改于：    2002-4-29

        注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
        地址收集软件。
        ----------------------------------------------------------------
        本文件修正历史：
                2002-06-06            移植原文件为本单元

        原文件修正历史：

                2002-4                增加一些各插件都能使用宏定义
                2002-3                增加播放顺序表。修正相关的INFOSTR分配及释放函数
                2002-3                增加数种新图象格式定义，增加数据结构移植定义
                2002-1                修正静态图像文件不能包含多幅图像的错误注释
                2001-6                修改新版接口
                2000-7                添加图像压缩类型
                2000-6                第一个测试版发布

*******************************************************************************}

unit iseeirw;
{* |<PRE>
================================================================================
* 软件名称：ISeeExplorer
* 单元名称：图像读写模块全局定义文件的 Delphi 申明单元
* 单元版本：V1.0
* 单元作者：与月共舞 yygw@163.com; http://yygw.126.com
* 原 单 元：iseeirw.h
* 原 作 者：YZ            yzfree##sina.com
*           xiaoyueer     xiaoyueer##263.net
*           orbit         Inte2000##263.net
*           Janhail       janhail##sina.com
* 备    注：该单元一般不需要直接使用，请使用封装的 ISee.pas 单元来访问
* 开发平台：PWin98SE + Delphi5
* 兼容测试：PWin9X/2000/XP + Delphi 5/6
* 本 地 化：该单元中的字符串均符合本地化处理方式
* 更新记录：2002.06.20 V1.0
*                创建单元
================================================================================
|</PRE>}

interface

{$I ISee.inc}

uses
  Windows, iseeio;

//==============================================================================
// 与插件相关的类型、常量定义
//==============================================================================

{ 开发者信息结构 }

const
  AI_NAME_SIZE      = 32;
  AI_EMAIL_SIZE     = 64;
  AI_MESSAGE_SIZE   = 160;

type
  PAuthorInfo = ^TAuthorInfo;
  _tag_author_info = packed record
    // 开发者名字
    ai_name: array[0..AI_NAME_SIZE - 1] of Char;
    // 开发者email地址
    ai_email: array[0..AI_EMAIL_SIZE - 1] of Char;
    // 开发者留言。（最大字数限制在159个英文字符，或79个汉字）
    ai_message: array[0..AI_MESSAGE_SIZE - 1] of Char;
  end;
  TAuthorInfo = _tag_author_info;
  AUTHOR_INFO = _tag_author_info;
  LPAUTHOR_INFO = ^_tag_author_info;

{ 插件能处理的图像文件类型信息结构（即扩展名信息） }

const
  IDI_CURRENCY_NAME_SIZE = 16;
  IDI_SYNONYM_NAME_COUNT = 4;

type
  PIrwpDescInfo = ^TIrwpDescInfo;
  _tag_irwp_desc_info = packed record
    // 通用扩展名，必填
    idi_currency_name: array[0..IDI_CURRENCY_NAME_SIZE - 1] of Char;
    // 保留
    idi_rev: Cardinal;
    // 衍生扩展名有效项个数
    idi_synonym_count: Cardinal;
    // 衍生扩展名
    idi_synonym: array[0..IDI_SYNONYM_NAME_COUNT - 1] of array
    [0..IDI_CURRENCY_NAME_SIZE - 1] of Char;
  end;
  TIrwpDescInfo = _tag_irwp_desc_info;
  IRWP_DESC_INFO = _tag_irwp_desc_info;
  LPIRWP_DESC_INFO = ^_tag_irwp_desc_info;

{ 保存功能参数设定描述 }

const
  ISPD_ITEM_NUM     = 16;
  ISPD_STRING_SIZE  = 32;

type
  PIrwSaveParaDesc = ^TIrwSaveParaDesc;
  _tag_irw_save_para_desc = packed record
    // 有效描述项个数
    count: Integer;
    // 结构描述串
    desc: array[0..ISPD_STRING_SIZE - 1] of Char;
    // 值列表
    value: array[0..ISPD_ITEM_NUM - 1] of Integer;
    // 对应于值的描述
    value_desc: array[0..ISPD_ITEM_NUM - 1] of array[0..ISPD_STRING_SIZE - 1] of Char;
  end;                                  { 612 byte }
  TIrwSaveParaDesc = _tag_irw_save_para_desc;
  IRW_SAVE_PARA_DESC = _tag_irw_save_para_desc;
  LPIRW_SAVE_PARA_DESC = ^_tag_irw_save_para_desc;

{ 保存功能描述 }

const
  ISD_ITEM_NUM      = 4;

type
  PIrwSaveDesc = ^TIrwSaveDesc;
  _tag_irw_save_desc = packed record
    // 位深支持描述
    bitcount: Cardinal;
    // 对图像个数的要求，0－无要求  1－只能保存一副图像  2－可保存多幅图像
    img_num: Integer;
    // 有效描述项个数
    count: Integer;
    // 描述项
    para: array[0..ISD_ITEM_NUM - 1] of TIrwSaveParaDesc;
  end;                                  { 2456 byte }
  TIrwSaveDesc = _tag_irw_save_desc;
  IRW_SAVE_DESC = _tag_irw_save_desc;
  LPIRW_SAVE_DESC = ^_tag_irw_save_desc;

{ 插件自身信息结构 }

const
  IRWP_NAME_SIZE    = 32;
  IRWP_AUTHOR_SIZE  = 16;
  IRWP_FUNC_PREFIX_SIZE = 16;

  IRWP_READ_SUPP    = $01;
  IRWP_WRITE_SUPP   = $02;
  IRWP_REREAD_SUPP  = $04;

type
  PIrwpInfo = ^TIrwpInfo;
  _tag_irwp_info = packed record
    // 初始化标签，0－表示未初始化，1－表示已初始化
    init_tag: Cardinal;
    // 保留
    rev0: Cardinal;
    // 插件名称
    irwp_name: array[0..IRWP_NAME_SIZE - 1] of Char;
    // 本模块函数前缀，如BMP模块函数前缀为"bmp_"
    irwp_func_prefix: array[0..IRWP_FUNC_PREFIX_SIZE - 1] of Char;
    // 版本号。（十进制值，十位为主版本号，个位为副版本，如12，即表示1.2版）
    irwp_version: Cardinal;
    // 0－调试版插件，1－发布版插件
    irwp_build_set: Cardinal;
    // 功能标识：0位是读支持标志，1位是写支持标志，其它未用
    irwp_function: Cardinal;
    // 保存功能描述
    irwp_save: TIrwSaveDesc;
    // 开发者人数（即开发者信息中有效项的个数）
    irwp_author_count: Cardinal;
    // 开发者信息
    irwp_author: array[0..IRWP_AUTHOR_SIZE - 1] of TAuthorInfo;
    // 插件描述信息（扩展名信息）
    irwp_desc_info: TIrwpDescInfo;
  end;                                  { size (in byte) - 4K + 160 + 2456 byte }
  TIrwpInfo = _tag_irwp_info;
  IRWP_INFO = _tag_irwp_info;
  LPIRWP_INFO = ^_tag_irwp_info;

{ 图像读写模块执行结果 }

type
  EXERESULT = Cardinal;
  TExeResult = EXERESULT;

const
  ER_SUCCESS        = 0;                // 执行成功
  ER_USERBREAK      = 1;                // 操作被用户中断
  ER_BADIMAGE       = 2;                // 指定的文件是一个受损的图像文件
  ER_NONIMAGE       = 3;                // 指定的文件不是一个图像文件
  ER_MEMORYERR      = 4;                // 内存不足，导致操作失败
  ER_FILERWERR      = 5;                // 文件在读写过程中发生错误（系统、硬件等原因引起文件读写异常）
  ER_SYSERR         = 6;                // 操作系统不稳定，无法执行命令
  ER_NOTSUPPORT     = 7;                // 不支持的命令
  ER_NSIMGFOR       = 8;                // 保存功能不支持的图像格式
  //ER_XXXX = 9;                        // 新的错误信息在此插入
  ER_MAX            = 9;                // 边界值

{ 图像类型 }

type
  IMGTYPE = Cardinal;
  TImgType = IMGTYPE;

const
  IMT_NULL          = 0;                // 无效图像类型
  IMT_RESSTATIC     = 1;                // 光栅、静态图像
  IMT_RESDYN        = 2;                // 光栅、动态图像
  IMT_VECTORSTATIC  = 3;                // 矢量、静态图像
  IMT_VECTORDYN     = 4;                // 矢量、动态图像
  //IMT_XXXXX = 5;                      // 在此插入新的图像类型
  IMT_MAX           = 5;                // 边界值

{ 图像格式 }

type
  IMGFORMAT = Cardinal;
  TImgFormat = IMGFORMAT;

const
  IMF_NULL          = 0;                // 未知或无效的图像
  IMF_BMP           = 1;                // BMP格式的图像
  IMF_CUR           = 2;                // ....
  IMF_ICO           = 3;
  IMF_PCX           = 4;
  IMF_DCX           = 5;
  IMF_GIF           = 6;
  IMF_PNG           = 7;
  IMF_TGA           = 8;
  IMF_WMF           = 9;
  IMF_EMF           = 10;
  IMF_TIFF          = 11;
  IMF_JPEG          = 12;               // ...
  IMF_PSD           = 13;               // PSD格式的图像
  IMF_XBM           = 14;
  IMF_XPM           = 15;
  IMF_PCD           = 16;
  IMF_RAW           = 17;
  IMF_RAS           = 18;
  IMF_PPM           = 19;
  IMF_PGM           = 20;
  IMF_PBM           = 21;
  IMF_IFF           = 22;               // ...
  IMF_JP2           = 23;               // JPEG2000格式图像
  IMF_LBM           = 24;
  IMF_BW            = 25;
  IMF_PIX           = 26;
  IMF_RLE           = 27;
  IMF_XWD           = 28;
  IMF_DXF           = 29;
  IMF_WBMP          = 30;
  IMF_ANI           = 31;
  IMF_ART           = 32;
  IMF_FPX           = 33;
  IMF_ICN           = 34;               // ...
  IMF_KDC           = 35;
  IMF_LDF           = 36;
  IMF_LWF           = 37;
  IMF_MAG           = 38;
  IMF_PIC           = 39;
  IMF_PCT           = 40;
  IMF_PDF           = 41;
  IMF_PS            = 42;
  IMF_PSP           = 43;
  IMF_RSB           = 44;
  IMF_SGI           = 45;
  IMF_RLA           = 46;
  IMF_RLB           = 47;
  IMF_RLC           = 48;
  IMF_JBIG          = 49;
  //IMF_XXX = 50;                       // 新增格式在此处插入
  IMF_MAX           = 50;               // 边界值

{ 象素数据的存放形式 }

type
  IMGCOMPRESS = Cardinal;
  TImgCompress = IMGCOMPRESS;

const
  ICS_RGB           = 0;                // 未经压缩的RGB存储方式
  ICS_RLE4          = 1;                // RLE4存储方式
  ICS_RLE8          = 2;                // RLE8存储方式
  // @@@@@@@@@@Added By xiaoyueer 2000.8.20
  ICS_RLE16         = 3;                // RLE16存储方式
  ICS_RLE24         = 4;                // RLE24存储方式
  ICS_RLE32         = 5;                // RLE32存储方式
  // @@@@@@@@@End 2000.8.20
  ICS_BITFIELDS     = 6;                // 分位存储方式
  ICS_PCXRLE        = 7;                // PCX RLE存储方式
  ICS_GIFLZW        = 8;                // GIF LZW存储方式
  ICS_GDIRECORD     = 9;                // WMF的元文件存储方式
  // @@@@@@@@@@Added By orbit 2000.8.31
  ICS_JPEGGRAYSCALE = 10;               // 灰度图象,JPEG-256级灰度
  ICS_JPEGRGB       = 11;               // red/green/blue
  ICS_JPEGYCbCr     = 12;               // Y/Cb/Cr (also known as YUV)
  ICS_JPEGCMYK      = 13;               // C/M/Y/K
  ICS_JPEGYCCK      = 14;               // Y/Cb/Cr/K
  // @@@@@@@@@End 2000.8.31
  // @@@@@@@@@@LibTiff: Sam Leffler 2001.1.6
  ICS_TIFF_NONE     = 15;               // TIFF non-compress ABGR mode
  ICS_TIFF_CCITTRLE = 16;               // CCITT modified Huffman RLE
  ICS_TIFF_CCITTAX3 = 17;               // CCITT Group 3 fax encoding
  ICS_TIFF_CCITTFAX4 = 18;              // CCITT Group 4 fax encoding
  ICS_TIFF_LZW      = 19;               // Lempel-Ziv  & Welch
  ICS_TIFF_OJPEG    = 20;               // 6.0 JPEG
  ICS_TIFF_JPEG     = 21;               // JPEG DCT compression
  ICS_TIFF_NEXT     = 22;               // NeXT 2-bit RLE
  ICS_TIFF_CCITTRLEW = 23;              // 1 w/ word alignment
  ICS_TIFF_PACKBITS = 24;               // Macintosh RLE
  ICS_TIFF_THUNDERSCAN = 25;            // ThunderScan RLE
  // @@@@@@@@@@Added By <dkelly@etsinc.com>
  ICS_TIFF_IT8CTPAD = 26;               // IT8 CT w/padding
  ICS_TIFF_IT8LW    = 27;               // IT8 Linework RLE
  ICS_TIFF_IT8MP    = 28;               // IT8 Monochrome picture
  ICS_TIFF_IT8BL    = 29;               // IT8 Binary line art
  ICS_TIFF_PIXARFILM = 30;              // Pixar companded 10bit LZW
  ICS_TIFF_PIXARLOG = 31;               // Pixar companded 11bit ZIP
  ICS_TIFF_DEFLATE  = 32;               // Deflate compression
  ICS_TIFF_ADOBE_DEFLATE = 33;          // Deflate compression, as recognized by Adobe
  // @@@@@@@@@@Added By <dev@oceana.com>
  ICS_TIFF_DCS      = 34;               // Kodak DCS encoding
  ICS_TIFF_JBIG     = 35;               // ISO JBIG
  ICS_TIFF_SGILOG   = 36;               // SGI Log Luminance RLE
  ICS_TIFF_SGILOG24 = 37;               // SGI Log 24-bit packed
  // @@@@@@@@@End 2001.1.6
  // @@@@@@@@@@Added By YZ 2001.8.3
  ICS_PNG_GRAYSCALE = 38;               // PNG灰度图
  ICS_PNG_PALETTE   = 39;               // PNG索引图
  ICS_PNG_RGB       = 40;               // PNG真彩图
  // @@@@@@@@@End 2001.8.3
  ICS_XBM_TEXT      = 41;               // 文本方式
  ICS_XPM_TEXT      = 42;
  // @@@@@@@@@@Added By YZ 2002.2.3
  ICS_PNM_TEXT      = 43;
  ICS_PNM_BIN       = 44;
  // @@@@@@@@@End 2001.2.3
  //ICS_XXXX = 45;                      // 插入新的存储方式类型
  ICS_UNKONW        = 1024;             // 未知的压缩方式
  ICS_MAX           = 2048;             // 边界

{ 图像读写数据包结构（核心结构） }

const
  MAX_PALETTE_COUNT = 256;              // 最大调色板项个数

{$IFDEF _DEBUG}
  INFOSTR_DBG_MARK  = $65655349;        // 图像结构标识串"ISee"
{$ENDIF}

type
  PPointerArray = ^TPointerArray;
  TPointerArray = array[0..65535] of Pointer;
  PCardinalArray = ^TCardinalArray;
  TCardinalArray = array[0..65535] of Cardinal;

  PSubImgBlock = ^TSubImgBlock;

  PInfoStr = ^TInfoStr;
  _tag_infostr = packed record
{$IFDEF _DEBUG}
    // 结构的标识，用于调试，等于INFOSTR_DBG_MARK
    sct_mark: Cardinal;
{$ENDIF}

    // 图像文件类型
    imgtype: TImgType;
    // 图像文件格式（后缀名）
    imgformat: TImgFormat;
    // 图像文件的压缩方式
    compression: TImgCompress;

    // 图像宽度
    width: Cardinal;
    // 图像高度
    height: Cardinal;
    // 每个像素所占的位数（位深度）
    bitcount: Cardinal;
    // 图像的上下顺序 （0－正向，1－倒向）
    order: Cardinal;

    // 各颜色分量的掩码值  注：1~8位的图像无掩码数据
    r_mask: Cardinal;
    g_mask: Cardinal;
    b_mask: Cardinal;
    a_mask: Cardinal;

    // 图像位数据中从0行到n - 1 行每一行的首地址
    pp_line_addr: PPointerArray;
    // 图像位数据缓冲区首地址
    p_bit_data: Pointer;
    // 调色板有效项的个数
    pal_count: Cardinal;
    // 调色板数据
    palette: array[0..MAX_PALETTE_COUNT - 1] of Cardinal;

    // 该文件中图像的个数
    imgnumbers: Cardinal;
    // 子图像数据链地址
    psubimg: PSubImgBlock;

    // 数据包当前的数据状态：
    //   0 － 空的数据包，没有任何变量数据是有效的
    //   1 － 存在图像的描述信息
    //   2 － 存在图像数据
    data_state: Word;

    // 访问许可标志
    access: TRTLCriticalSection;

    // 中断标志。0－继续，1－中断操作
    break_mark: Integer;
    // 总的操作进度值
    process_total: Integer;
    // 当前的操作进度
    process_current: Integer;

    // 播放顺序表尺寸（以元素为单位）
    play_number: Cardinal;
    // 播放顺序表
    play_order: PCardinalArray;
    // 首帧停留时间
    time: Cardinal;

    // 主图象透明色（ - 1 表示无透明色）
    colorkey: Integer;
  end;
  TInfoStr = _tag_infostr;
  INFOSTR = _tag_infostr;
  LPINFOSTR = ^_tag_infostr;

{ 多桢图像的子图像块结构（主要用于动画类图像文件）}

  _tag_subimage_block = packed record
    // 子图像块的序列号
    number: Integer;

    // 子图像块的显示位置（左上角）
    left, top: Integer;
    // 子图像块的图像宽度及高度
    width, height: Cardinal;
    // 子图像块位深
    bitcount: Cardinal;
    // 图像的上下顺序 （0－正向，1－倒向）
    order: Cardinal;

    // 处置方法（只针对动态图像）
    dowith: Integer;
    // 用户输入（只针对动态图像）
    userinput: Integer;
    // 显示的延迟时间（只针对动态图像）
    time: Integer;

    // 各颜色分量的掩码值  注：1~8位的图像无掩码数据
    r_mask: Cardinal;
    g_mask: Cardinal;
    b_mask: Cardinal;
    a_mask: Cardinal;

    // 透明色分量值
    colorkey: Integer;

    // 标准图像位数据中从0行到n - 1 行的每一行的首地址
    pp_line_addr: PPointerArray;
    // 标准图像位数据缓冲区首地址
    p_bit_data: Pointer;
    // 调色板有效项的个数
    pal_count: Cardinal;
    // 调色板数据
    palette: array[0..MAX_PALETTE_COUNT - 1] of Cardinal;

    // 第一桢图像数据的结构地址（所属数据包结构的地址）
    parents: PInfoStr;
    // 前一桢子图像数据的结构地址
    prev: PSubImgBlock;
    // 后一桢子图像数据的结构地址（NULL表示这是最后一副图像）
    next: PSubImgBlock;
  end;
  TSubImgBlock = _tag_subimage_block;
  SUBIMGBLOCK = _tag_subimage_block;
  LPSUBIMGBLOCK = ^_tag_subimage_block;

{ 保存图像时用于设定参数的结构 }

type
  PSaveStr = ^TSaveStr;
  _tag_savestr = packed record
    // 保存参数对应值（见IRW_SAVE_DESC结构）
    para_value: array[0..ISD_ITEM_NUM - 1] of Integer;
  end;
  TSaveStr = _tag_savestr;
  SAVESTR = _tag_savestr;
  LPSAVESTR = ^_tag_savestr;

{ IRWP API 类型定义 }

type
  IRWP_API_NAME = Cardinal;
  TIrwpApiName = IRWP_API_NAME;

const
  get_image_info    = 1;
  load_image        = 2;
  save_image        = 3;

const
  ISEEIRW_MAX_BITCOUNT = 32;            // ISee图象读写插件支持的最大位深值

//==============================================================================
// 插件 API 申明
//==============================================================================

{ 插件 API 原型定义 }

type
  // 获得ISee图像读写插件内部信息函数的接口指针类型（内部使用）
  TIsIrwPluginProc = function(): PIrwpInfo; cdecl;
  // get_image_info接口指针类型
  TGetImageInfoProc = function(psct: PIsAddr; pinfo: PInfoStr): TExeResult; cdecl;
  // load_image接口指针类型
  TLoadImageProc = function(psct: PIsAddr; pinfo: PInfoStr): TExeResult; cdecl;
  // save_image接口指针类型
  TSaveImageProc = function(psct: PIsAddr; pinfo: PInfoStr; lpse: PSaveStr): TExeResult; cdecl;

{ 从 iseeirw.dll 中导入的 API 申明 }

// 检测DLL是否有效的插件
function isirw_test_plugin(hmod: HMODULE): Cardinal; cdecl;
// 返回插件信息结构
function isirw_get_info(pfun_add: Cardinal): PIrwpInfo; cdecl;
// 返回插件中指定的API地址
function isirw_get_api_addr(hmod: HMODULE; pInfo: PIrwpInfo; name: TIrwpApiName):
  TFarProc; cdecl;

// 分配一个新的数据包
function isirw_alloc_INFOSTR: PInfoStr; cdecl;
// 释放一个数据包
function isirw_free_INFOSTR(pinfo: PInfoStr): PInfoStr; cdecl;
// 分配一个子图像块
function isirw_alloc_SUBIMGBLOCK: PSubImgBlock; cdecl;
// 释放一个子图像块
procedure isirw_free_SUBIMGBLOCK(p_node: PSubImgBlock); cdecl;

implementation

const
  csISeeIrw_DLL     = 'iseeirw.dll';

function isirw_test_plugin; cdecl; external csISeeIrw_DLL;
function isirw_get_info; cdecl; external csISeeIrw_DLL;
function isirw_get_api_addr; cdecl; external csISeeIrw_DLL;
function isirw_alloc_INFOSTR; cdecl; external csISeeIrw_DLL;
function isirw_free_INFOSTR; cdecl; external csISeeIrw_DLL;
function isirw_alloc_SUBIMGBLOCK; cdecl; external csISeeIrw_DLL;
procedure isirw_free_SUBIMGBLOCK; cdecl; external csISeeIrw_DLL;

end.

