{*******************************************************************************

        iseeio.pas

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
        本文件用途：    ISee I/O 定义文件的 Delphi 申明单元
        本文件名：      iseeio.pas
        原文件名：      iseeio.h
        本文件编写人：  与月共舞                yygw##163.com
        原文件编写人：  YZ                      yzfree##sina.com

        本文件版本：    20606
        最后修改于：    2002-06-06
        原文件版本：    20422
        最后修改于：    2002-4-29

        注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
        地址收集软件。
        ----------------------------------------------------------------
        本文件修正历史：
                2002-06-06      移植原文件为本单元

        原文件修正历史：
                2002-4          修正文件缺省打开模式（定为二进制模式）
                2001-6          第一个测试版发布

*******************************************************************************}

unit iseeio;
{* |<PRE>
================================================================================
* 软件名称：ISeeExplorer
* 单元名称：ISee I/O 定义文件的 Delphi 申明单元
* 单元版本：V1.0
* 单元作者：与月共舞 yygw@163.com; http://yygw.126.com
* 原 单 元：iseeio.h
* 原 作 者：YZ       yzfree##sina.com
* 备    注：该单元一般不需要直接使用，请使用封装的 ISee.pas 单元来访问
*           由于不打算用 Delphi 来开发插件，当前仅完成读取部分的移植
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
  Windows;

//==============================================================================
// 与 isee I/0 流相关的类型、常量定义
//==============================================================================

{ isee I/O 流类型定义 }

type
  _iseeio_confer_type = Cardinal;
  ISEEIO_CONFER_TYPE = _iseeio_confer_type;
  PISEEIO_CONFER_TYPE = ^_iseeio_confer_type;
  TISeeIOConferType = ISEEIO_CONFER_TYPE;
  PISeeIOConferType = ^TISeeIOConferType;

const
  ISEEIO_CONFER_INVALID = 0;            // 无效的协议类型
  ISEEIO_CONFER_LOC_FILE = 1;           // 本地文件类型         @@
  ISEEIO_CONFER_LOC_MEMORY = 2;         // 本地内存类型         @@
  ISEEIO_CONFER_ROT_INTERNET = 3;       // 远程internet         @@
  ISEEIO_CONFER_XXX = 4;                // 新类型
  ISEEIO_CONFER_MAX = 1024;             // 边界值
  ISEEIO_NON_CONFER_LOC_FILE = 1025;    // 非协议－本地文件类型
  // 注：以上枚举值中，只有带@@注释的部分才可以被用于isio_open_param函数。

{ isee I/O 流的地址结构 }

type
  PIsAddr = ^TIsAddr;
  _tag_istream_address = packed record
    // 协议描述字
    mark: array[0..3] of Char;
    // 流名称
    name: PChar;
    // 附加参数1
    param1: Cardinal;
    // 附加参数2
    param2: Cardinal;
  end;
  TIsAddr = _tag_istream_address;
  ISADDR = _tag_istream_address;

{ isee I/O 协议串标识符 }

const
  ISIO_STREAM_TYPE_INVALID = 'X';       // 无效标识
  ISIO_STREAM_TYPE_LOC_FILE = 'F';      // 本地文件
  ISIO_STREAM_TYPE_LOC_MEMORY = 'M';    // 本地内存
  ISIO_STREAM_TYPE_ROT_INTERNET = 'I';  // 网络
  //ISIO_STREAM_TYPE_XXX_XXXX = '?';    // 新类型

{ isee I/O 流句柄结构 }

{$IFDEF _DEBUG}
const
  ISFILE_DBG_ID     = '+S+';            // 流句柄标识字串（调试用）
{$ENDIF}

type
  PISFile = ^TISFile;
  _tag_iseeio_stream = packed record
    stream: Pointer;
    stream_type: TISeeIOConferType;
    org_name: PChar;
{$IFDEF _DEBUG}
    dbg_iseeio_stream_id: array[0..3] of Char;
{$ENDIF}
  end;
  ISFILE = _tag_iseeio_stream;
  TISFile = _tag_iseeio_stream;

// 本地文件流－FILE 结构已定义在 stdio.h 文件中
// typedef file FILE_STREAM;

{ 本地内存流－结构定义 }

const
  MEMORY_STREAM_UP_SIZE = 4096;
  MEMORY_STREAM_UP_STEP = 1;

type
  PISeeMemoryStream = ^TISeeMemoryStream;
  _tag_memory_stream = packed record
    p_start_pos: Pointer;               // 内存块的首地址
    len: Cardinal;                      // 有内容的长度
    cur_pos: Cardinal;                  // 当前的读写位置
    mode: Cardinal;                     // 流的打开模式

    error: Cardinal;                    // 错误标志
    total_size: Cardinal;               // 内存块的总尺寸
  end;
  MEMORY_STREAM = _tag_memory_stream;
  TISeeMemoryStream = MEMORY_STREAM;

{ 网络数据流－结构定义 }

type
  PISeeInternetStream = ^TISeeInternetStream;
  _tag_internet_stream = packed record
    p_address: Pointer;
  end;
  INTERNET_STREAM = _tag_internet_stream;
  TISeeInternetStream = INTERNET_STREAM;

{ 流模式字－位域含义表：
                                          32 bit (unsigned long)
  _________________________________________________________________->low bit
  |0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|
                           | |                         | | | |
                           | bin                       | | | |
                           text                        | | | read
                                                       | | write
                                                       | append
                                                       create
}

const
  ISIO_MODE_READ    = $01;
  ISIO_MODE_WRITE   = $02;
  ISIO_MODE_RDWR    = $03;
  ISIO_MODE_APPEND  = $04;
  ISIO_MODE_CREATE  = $08;

  ISIO_MODE_BIN     = $10000;
  ISIO_MODE_TXT     = $20000;           // 非ANSI标准，ISIO将不予支持

//==============================================================================
// isee I/O API 申明
//==============================================================================

{ 从 iseeio.dll 中导入的 API 申明 }

// isee I/O 辅助函数
function isio_open_param(psct: PIsAddr; Atype: TISeeIOConferType;
  name: PChar; param1, param2: Cardinal): PIsAddr; cdecl;

// 其它的 API 声明主要用于插件开发用，暂时不做转换

implementation

const
  csISeeIO_Dll      = 'iseeio.dll';

function isio_open_param; cdecl; external csISeeIO_Dll;

end.

