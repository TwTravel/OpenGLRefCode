{*******************************************************************************

        iseeio.pas

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
                        http://iseeexplorer.cosoft.org.cn

        ���ŵ���

                        yzfree##sina.com
        ----------------------------------------------------------------
        ���ļ���;��    ISee I/O �����ļ��� Delphi ������Ԫ
        ���ļ�����      iseeio.pas
        ԭ�ļ�����      iseeio.h
        ���ļ���д�ˣ�  ���¹���                yygw##163.com
        ԭ�ļ���д�ˣ�  YZ                      yzfree##sina.com

        ���ļ��汾��    20606
        ����޸��ڣ�    2002-06-06
        ԭ�ļ��汾��    20422
        ����޸��ڣ�    2002-4-29

        ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
        ��ַ�ռ������
        ----------------------------------------------------------------
        ���ļ�������ʷ��
                2002-06-06      ��ֲԭ�ļ�Ϊ����Ԫ

        ԭ�ļ�������ʷ��
                2002-4          �����ļ�ȱʡ��ģʽ����Ϊ������ģʽ��
                2001-6          ��һ�����԰淢��

*******************************************************************************}

unit iseeio;
{* |<PRE>
================================================================================
* ������ƣ�ISeeExplorer
* ��Ԫ���ƣ�ISee I/O �����ļ��� Delphi ������Ԫ
* ��Ԫ�汾��V1.0
* ��Ԫ���ߣ����¹��� yygw@163.com; http://yygw.126.com
* ԭ �� Ԫ��iseeio.h
* ԭ �� �ߣ�YZ       yzfree##sina.com
* ��    ע���õ�Ԫһ�㲻��Ҫֱ��ʹ�ã���ʹ�÷�װ�� ISee.pas ��Ԫ������
*           ���ڲ������� Delphi �������������ǰ����ɶ�ȡ���ֵ���ֲ
* ����ƽ̨��PWin98SE + Delphi5
* ���ݲ��ԣ�PWin9X/2000/XP + Delphi 5/6
* �� �� �����õ�Ԫ�е��ַ��������ϱ��ػ�����ʽ
* ���¼�¼��2002.06.20 V1.0
*                ������Ԫ
================================================================================
|</PRE>}

interface

{$I ISee.inc}

uses
  Windows;

//==============================================================================
// �� isee I/0 ����ص����͡���������
//==============================================================================

{ isee I/O �����Ͷ��� }

type
  _iseeio_confer_type = Cardinal;
  ISEEIO_CONFER_TYPE = _iseeio_confer_type;
  PISEEIO_CONFER_TYPE = ^_iseeio_confer_type;
  TISeeIOConferType = ISEEIO_CONFER_TYPE;
  PISeeIOConferType = ^TISeeIOConferType;

const
  ISEEIO_CONFER_INVALID = 0;            // ��Ч��Э������
  ISEEIO_CONFER_LOC_FILE = 1;           // �����ļ�����         @@
  ISEEIO_CONFER_LOC_MEMORY = 2;         // �����ڴ�����         @@
  ISEEIO_CONFER_ROT_INTERNET = 3;       // Զ��internet         @@
  ISEEIO_CONFER_XXX = 4;                // ������
  ISEEIO_CONFER_MAX = 1024;             // �߽�ֵ
  ISEEIO_NON_CONFER_LOC_FILE = 1025;    // ��Э�飭�����ļ�����
  // ע������ö��ֵ�У�ֻ�д�@@ע�͵Ĳ��ֲſ��Ա�����isio_open_param������

{ isee I/O ���ĵ�ַ�ṹ }

type
  PIsAddr = ^TIsAddr;
  _tag_istream_address = packed record
    // Э��������
    mark: array[0..3] of Char;
    // ������
    name: PChar;
    // ���Ӳ���1
    param1: Cardinal;
    // ���Ӳ���2
    param2: Cardinal;
  end;
  TIsAddr = _tag_istream_address;
  ISADDR = _tag_istream_address;

{ isee I/O Э�鴮��ʶ�� }

const
  ISIO_STREAM_TYPE_INVALID = 'X';       // ��Ч��ʶ
  ISIO_STREAM_TYPE_LOC_FILE = 'F';      // �����ļ�
  ISIO_STREAM_TYPE_LOC_MEMORY = 'M';    // �����ڴ�
  ISIO_STREAM_TYPE_ROT_INTERNET = 'I';  // ����
  //ISIO_STREAM_TYPE_XXX_XXXX = '?';    // ������

{ isee I/O ������ṹ }

{$IFDEF _DEBUG}
const
  ISFILE_DBG_ID     = '+S+';            // �������ʶ�ִ��������ã�
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

// �����ļ�����FILE �ṹ�Ѷ����� stdio.h �ļ���
// typedef file FILE_STREAM;

{ �����ڴ������ṹ���� }

const
  MEMORY_STREAM_UP_SIZE = 4096;
  MEMORY_STREAM_UP_STEP = 1;

type
  PISeeMemoryStream = ^TISeeMemoryStream;
  _tag_memory_stream = packed record
    p_start_pos: Pointer;               // �ڴ����׵�ַ
    len: Cardinal;                      // �����ݵĳ���
    cur_pos: Cardinal;                  // ��ǰ�Ķ�дλ��
    mode: Cardinal;                     // ���Ĵ�ģʽ

    error: Cardinal;                    // �����־
    total_size: Cardinal;               // �ڴ����ܳߴ�
  end;
  MEMORY_STREAM = _tag_memory_stream;
  TISeeMemoryStream = MEMORY_STREAM;

{ �������������ṹ���� }

type
  PISeeInternetStream = ^TISeeInternetStream;
  _tag_internet_stream = packed record
    p_address: Pointer;
  end;
  INTERNET_STREAM = _tag_internet_stream;
  TISeeInternetStream = INTERNET_STREAM;

{ ��ģʽ�֣�λ�����
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
  ISIO_MODE_TXT     = $20000;           // ��ANSI��׼��ISIO������֧��

//==============================================================================
// isee I/O API ����
//==============================================================================

{ �� iseeio.dll �е���� API ���� }

// isee I/O ��������
function isio_open_param(psct: PIsAddr; Atype: TISeeIOConferType;
  name: PChar; param1, param2: Cardinal): PIsAddr; cdecl;

// ������ API ������Ҫ���ڲ�������ã���ʱ����ת��

implementation

const
  csISeeIO_Dll      = 'iseeio.dll';

function isio_open_param; cdecl; external csISeeIO_Dll;

end.

