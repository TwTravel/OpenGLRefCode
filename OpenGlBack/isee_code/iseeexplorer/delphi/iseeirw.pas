{*******************************************************************************

        iseeirw.pas

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
        ���ļ���;��    ͼ���дģ��ȫ�ֶ����ļ��� Delphi ������Ԫ
        ���ļ�����      iseeirw.pas
        ԭ�ļ�����      iseeirw.h
        ���ļ���д�ˣ�  ���¹���                yygw##163.com
        ԭ�ļ���д�ˣ�  YZ                      yzfree##sina.com
                                        xiaoyueer               xiaoyueer##263.net
                                        orbit                   Inte2000##263.net
                                        Janhail                 janhail##sina.com

        ���ļ��汾��    20606
        ����޸��ڣ�    2002-06-06
        ԭ�ļ��汾��    20422
        ����޸��ڣ�    2002-4-29

        ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
        ��ַ�ռ������
        ----------------------------------------------------------------
        ���ļ�������ʷ��
                2002-06-06            ��ֲԭ�ļ�Ϊ����Ԫ

        ԭ�ļ�������ʷ��

                2002-4                ����һЩ���������ʹ�ú궨��
                2002-3                ���Ӳ���˳���������ص�INFOSTR���估�ͷź���
                2002-3                ����������ͼ���ʽ���壬�������ݽṹ��ֲ����
                2002-1                ������̬ͼ���ļ����ܰ������ͼ��Ĵ���ע��
                2001-6                �޸��°�ӿ�
                2000-7                ���ͼ��ѹ������
                2000-6                ��һ�����԰淢��

*******************************************************************************}

unit iseeirw;
{* |<PRE>
================================================================================
* ������ƣ�ISeeExplorer
* ��Ԫ���ƣ�ͼ���дģ��ȫ�ֶ����ļ��� Delphi ������Ԫ
* ��Ԫ�汾��V1.0
* ��Ԫ���ߣ����¹��� yygw@163.com; http://yygw.126.com
* ԭ �� Ԫ��iseeirw.h
* ԭ �� �ߣ�YZ            yzfree##sina.com
*           xiaoyueer     xiaoyueer##263.net
*           orbit         Inte2000##263.net
*           Janhail       janhail##sina.com
* ��    ע���õ�Ԫһ�㲻��Ҫֱ��ʹ�ã���ʹ�÷�װ�� ISee.pas ��Ԫ������
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
  Windows, iseeio;

//==============================================================================
// ������ص����͡���������
//==============================================================================

{ ��������Ϣ�ṹ }

const
  AI_NAME_SIZE      = 32;
  AI_EMAIL_SIZE     = 64;
  AI_MESSAGE_SIZE   = 160;

type
  PAuthorInfo = ^TAuthorInfo;
  _tag_author_info = packed record
    // ����������
    ai_name: array[0..AI_NAME_SIZE - 1] of Char;
    // ������email��ַ
    ai_email: array[0..AI_EMAIL_SIZE - 1] of Char;
    // ���������ԡ����������������159��Ӣ���ַ�����79�����֣�
    ai_message: array[0..AI_MESSAGE_SIZE - 1] of Char;
  end;
  TAuthorInfo = _tag_author_info;
  AUTHOR_INFO = _tag_author_info;
  LPAUTHOR_INFO = ^_tag_author_info;

{ ����ܴ����ͼ���ļ�������Ϣ�ṹ������չ����Ϣ�� }

const
  IDI_CURRENCY_NAME_SIZE = 16;
  IDI_SYNONYM_NAME_COUNT = 4;

type
  PIrwpDescInfo = ^TIrwpDescInfo;
  _tag_irwp_desc_info = packed record
    // ͨ����չ��������
    idi_currency_name: array[0..IDI_CURRENCY_NAME_SIZE - 1] of Char;
    // ����
    idi_rev: Cardinal;
    // ������չ����Ч�����
    idi_synonym_count: Cardinal;
    // ������չ��
    idi_synonym: array[0..IDI_SYNONYM_NAME_COUNT - 1] of array
    [0..IDI_CURRENCY_NAME_SIZE - 1] of Char;
  end;
  TIrwpDescInfo = _tag_irwp_desc_info;
  IRWP_DESC_INFO = _tag_irwp_desc_info;
  LPIRWP_DESC_INFO = ^_tag_irwp_desc_info;

{ ���湦�ܲ����趨���� }

const
  ISPD_ITEM_NUM     = 16;
  ISPD_STRING_SIZE  = 32;

type
  PIrwSaveParaDesc = ^TIrwSaveParaDesc;
  _tag_irw_save_para_desc = packed record
    // ��Ч���������
    count: Integer;
    // �ṹ������
    desc: array[0..ISPD_STRING_SIZE - 1] of Char;
    // ֵ�б�
    value: array[0..ISPD_ITEM_NUM - 1] of Integer;
    // ��Ӧ��ֵ������
    value_desc: array[0..ISPD_ITEM_NUM - 1] of array[0..ISPD_STRING_SIZE - 1] of Char;
  end;                                  { 612 byte }
  TIrwSaveParaDesc = _tag_irw_save_para_desc;
  IRW_SAVE_PARA_DESC = _tag_irw_save_para_desc;
  LPIRW_SAVE_PARA_DESC = ^_tag_irw_save_para_desc;

{ ���湦������ }

const
  ISD_ITEM_NUM      = 4;

type
  PIrwSaveDesc = ^TIrwSaveDesc;
  _tag_irw_save_desc = packed record
    // λ��֧������
    bitcount: Cardinal;
    // ��ͼ�������Ҫ��0����Ҫ��  1��ֻ�ܱ���һ��ͼ��  2���ɱ�����ͼ��
    img_num: Integer;
    // ��Ч���������
    count: Integer;
    // ������
    para: array[0..ISD_ITEM_NUM - 1] of TIrwSaveParaDesc;
  end;                                  { 2456 byte }
  TIrwSaveDesc = _tag_irw_save_desc;
  IRW_SAVE_DESC = _tag_irw_save_desc;
  LPIRW_SAVE_DESC = ^_tag_irw_save_desc;

{ ���������Ϣ�ṹ }

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
    // ��ʼ����ǩ��0����ʾδ��ʼ����1����ʾ�ѳ�ʼ��
    init_tag: Cardinal;
    // ����
    rev0: Cardinal;
    // �������
    irwp_name: array[0..IRWP_NAME_SIZE - 1] of Char;
    // ��ģ�麯��ǰ׺����BMPģ�麯��ǰ׺Ϊ"bmp_"
    irwp_func_prefix: array[0..IRWP_FUNC_PREFIX_SIZE - 1] of Char;
    // �汾�š���ʮ����ֵ��ʮλΪ���汾�ţ���λΪ���汾����12������ʾ1.2�棩
    irwp_version: Cardinal;
    // 0�����԰�����1����������
    irwp_build_set: Cardinal;
    // ���ܱ�ʶ��0λ�Ƕ�֧�ֱ�־��1λ��д֧�ֱ�־������δ��
    irwp_function: Cardinal;
    // ���湦������
    irwp_save: TIrwSaveDesc;
    // ����������������������Ϣ����Ч��ĸ�����
    irwp_author_count: Cardinal;
    // ��������Ϣ
    irwp_author: array[0..IRWP_AUTHOR_SIZE - 1] of TAuthorInfo;
    // ���������Ϣ����չ����Ϣ��
    irwp_desc_info: TIrwpDescInfo;
  end;                                  { size (in byte) - 4K + 160 + 2456 byte }
  TIrwpInfo = _tag_irwp_info;
  IRWP_INFO = _tag_irwp_info;
  LPIRWP_INFO = ^_tag_irwp_info;

{ ͼ���дģ��ִ�н�� }

type
  EXERESULT = Cardinal;
  TExeResult = EXERESULT;

const
  ER_SUCCESS        = 0;                // ִ�гɹ�
  ER_USERBREAK      = 1;                // �������û��ж�
  ER_BADIMAGE       = 2;                // ָ�����ļ���һ�������ͼ���ļ�
  ER_NONIMAGE       = 3;                // ָ�����ļ�����һ��ͼ���ļ�
  ER_MEMORYERR      = 4;                // �ڴ治�㣬���²���ʧ��
  ER_FILERWERR      = 5;                // �ļ��ڶ�д�����з�������ϵͳ��Ӳ����ԭ�������ļ���д�쳣��
  ER_SYSERR         = 6;                // ����ϵͳ���ȶ����޷�ִ������
  ER_NOTSUPPORT     = 7;                // ��֧�ֵ�����
  ER_NSIMGFOR       = 8;                // ���湦�ܲ�֧�ֵ�ͼ���ʽ
  //ER_XXXX = 9;                        // �µĴ�����Ϣ�ڴ˲���
  ER_MAX            = 9;                // �߽�ֵ

{ ͼ������ }

type
  IMGTYPE = Cardinal;
  TImgType = IMGTYPE;

const
  IMT_NULL          = 0;                // ��Чͼ������
  IMT_RESSTATIC     = 1;                // ��դ����̬ͼ��
  IMT_RESDYN        = 2;                // ��դ����̬ͼ��
  IMT_VECTORSTATIC  = 3;                // ʸ������̬ͼ��
  IMT_VECTORDYN     = 4;                // ʸ������̬ͼ��
  //IMT_XXXXX = 5;                      // �ڴ˲����µ�ͼ������
  IMT_MAX           = 5;                // �߽�ֵ

{ ͼ���ʽ }

type
  IMGFORMAT = Cardinal;
  TImgFormat = IMGFORMAT;

const
  IMF_NULL          = 0;                // δ֪����Ч��ͼ��
  IMF_BMP           = 1;                // BMP��ʽ��ͼ��
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
  IMF_PSD           = 13;               // PSD��ʽ��ͼ��
  IMF_XBM           = 14;
  IMF_XPM           = 15;
  IMF_PCD           = 16;
  IMF_RAW           = 17;
  IMF_RAS           = 18;
  IMF_PPM           = 19;
  IMF_PGM           = 20;
  IMF_PBM           = 21;
  IMF_IFF           = 22;               // ...
  IMF_JP2           = 23;               // JPEG2000��ʽͼ��
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
  //IMF_XXX = 50;                       // ������ʽ�ڴ˴�����
  IMF_MAX           = 50;               // �߽�ֵ

{ �������ݵĴ����ʽ }

type
  IMGCOMPRESS = Cardinal;
  TImgCompress = IMGCOMPRESS;

const
  ICS_RGB           = 0;                // δ��ѹ����RGB�洢��ʽ
  ICS_RLE4          = 1;                // RLE4�洢��ʽ
  ICS_RLE8          = 2;                // RLE8�洢��ʽ
  // @@@@@@@@@@Added By xiaoyueer 2000.8.20
  ICS_RLE16         = 3;                // RLE16�洢��ʽ
  ICS_RLE24         = 4;                // RLE24�洢��ʽ
  ICS_RLE32         = 5;                // RLE32�洢��ʽ
  // @@@@@@@@@End 2000.8.20
  ICS_BITFIELDS     = 6;                // ��λ�洢��ʽ
  ICS_PCXRLE        = 7;                // PCX RLE�洢��ʽ
  ICS_GIFLZW        = 8;                // GIF LZW�洢��ʽ
  ICS_GDIRECORD     = 9;                // WMF��Ԫ�ļ��洢��ʽ
  // @@@@@@@@@@Added By orbit 2000.8.31
  ICS_JPEGGRAYSCALE = 10;               // �Ҷ�ͼ��,JPEG-256���Ҷ�
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
  ICS_PNG_GRAYSCALE = 38;               // PNG�Ҷ�ͼ
  ICS_PNG_PALETTE   = 39;               // PNG����ͼ
  ICS_PNG_RGB       = 40;               // PNG���ͼ
  // @@@@@@@@@End 2001.8.3
  ICS_XBM_TEXT      = 41;               // �ı���ʽ
  ICS_XPM_TEXT      = 42;
  // @@@@@@@@@@Added By YZ 2002.2.3
  ICS_PNM_TEXT      = 43;
  ICS_PNM_BIN       = 44;
  // @@@@@@@@@End 2001.2.3
  //ICS_XXXX = 45;                      // �����µĴ洢��ʽ����
  ICS_UNKONW        = 1024;             // δ֪��ѹ����ʽ
  ICS_MAX           = 2048;             // �߽�

{ ͼ���д���ݰ��ṹ�����Ľṹ�� }

const
  MAX_PALETTE_COUNT = 256;              // ����ɫ�������

{$IFDEF _DEBUG}
  INFOSTR_DBG_MARK  = $65655349;        // ͼ��ṹ��ʶ��"ISee"
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
    // �ṹ�ı�ʶ�����ڵ��ԣ�����INFOSTR_DBG_MARK
    sct_mark: Cardinal;
{$ENDIF}

    // ͼ���ļ�����
    imgtype: TImgType;
    // ͼ���ļ���ʽ����׺����
    imgformat: TImgFormat;
    // ͼ���ļ���ѹ����ʽ
    compression: TImgCompress;

    // ͼ����
    width: Cardinal;
    // ͼ��߶�
    height: Cardinal;
    // ÿ��������ռ��λ����λ��ȣ�
    bitcount: Cardinal;
    // ͼ�������˳�� ��0������1������
    order: Cardinal;

    // ����ɫ����������ֵ  ע��1~8λ��ͼ������������
    r_mask: Cardinal;
    g_mask: Cardinal;
    b_mask: Cardinal;
    a_mask: Cardinal;

    // ͼ��λ�����д�0�е�n - 1 ��ÿһ�е��׵�ַ
    pp_line_addr: PPointerArray;
    // ͼ��λ���ݻ������׵�ַ
    p_bit_data: Pointer;
    // ��ɫ����Ч��ĸ���
    pal_count: Cardinal;
    // ��ɫ������
    palette: array[0..MAX_PALETTE_COUNT - 1] of Cardinal;

    // ���ļ���ͼ��ĸ���
    imgnumbers: Cardinal;
    // ��ͼ����������ַ
    psubimg: PSubImgBlock;

    // ���ݰ���ǰ������״̬��
    //   0 �� �յ����ݰ���û���κα�����������Ч��
    //   1 �� ����ͼ���������Ϣ
    //   2 �� ����ͼ������
    data_state: Word;

    // ������ɱ�־
    access: TRTLCriticalSection;

    // �жϱ�־��0��������1���жϲ���
    break_mark: Integer;
    // �ܵĲ�������ֵ
    process_total: Integer;
    // ��ǰ�Ĳ�������
    process_current: Integer;

    // ����˳���ߴ磨��Ԫ��Ϊ��λ��
    play_number: Cardinal;
    // ����˳���
    play_order: PCardinalArray;
    // ��֡ͣ��ʱ��
    time: Cardinal;

    // ��ͼ��͸��ɫ�� - 1 ��ʾ��͸��ɫ��
    colorkey: Integer;
  end;
  TInfoStr = _tag_infostr;
  INFOSTR = _tag_infostr;
  LPINFOSTR = ^_tag_infostr;

{ ����ͼ�����ͼ���ṹ����Ҫ���ڶ�����ͼ���ļ���}

  _tag_subimage_block = packed record
    // ��ͼ�������к�
    number: Integer;

    // ��ͼ������ʾλ�ã����Ͻǣ�
    left, top: Integer;
    // ��ͼ����ͼ���ȼ��߶�
    width, height: Cardinal;
    // ��ͼ���λ��
    bitcount: Cardinal;
    // ͼ�������˳�� ��0������1������
    order: Cardinal;

    // ���÷�����ֻ��Զ�̬ͼ��
    dowith: Integer;
    // �û����루ֻ��Զ�̬ͼ��
    userinput: Integer;
    // ��ʾ���ӳ�ʱ�䣨ֻ��Զ�̬ͼ��
    time: Integer;

    // ����ɫ����������ֵ  ע��1~8λ��ͼ������������
    r_mask: Cardinal;
    g_mask: Cardinal;
    b_mask: Cardinal;
    a_mask: Cardinal;

    // ͸��ɫ����ֵ
    colorkey: Integer;

    // ��׼ͼ��λ�����д�0�е�n - 1 �е�ÿһ�е��׵�ַ
    pp_line_addr: PPointerArray;
    // ��׼ͼ��λ���ݻ������׵�ַ
    p_bit_data: Pointer;
    // ��ɫ����Ч��ĸ���
    pal_count: Cardinal;
    // ��ɫ������
    palette: array[0..MAX_PALETTE_COUNT - 1] of Cardinal;

    // ��һ��ͼ�����ݵĽṹ��ַ���������ݰ��ṹ�ĵ�ַ��
    parents: PInfoStr;
    // ǰһ����ͼ�����ݵĽṹ��ַ
    prev: PSubImgBlock;
    // ��һ����ͼ�����ݵĽṹ��ַ��NULL��ʾ�������һ��ͼ��
    next: PSubImgBlock;
  end;
  TSubImgBlock = _tag_subimage_block;
  SUBIMGBLOCK = _tag_subimage_block;
  LPSUBIMGBLOCK = ^_tag_subimage_block;

{ ����ͼ��ʱ�����趨�����Ľṹ }

type
  PSaveStr = ^TSaveStr;
  _tag_savestr = packed record
    // ���������Ӧֵ����IRW_SAVE_DESC�ṹ��
    para_value: array[0..ISD_ITEM_NUM - 1] of Integer;
  end;
  TSaveStr = _tag_savestr;
  SAVESTR = _tag_savestr;
  LPSAVESTR = ^_tag_savestr;

{ IRWP API ���Ͷ��� }

type
  IRWP_API_NAME = Cardinal;
  TIrwpApiName = IRWP_API_NAME;

const
  get_image_info    = 1;
  load_image        = 2;
  save_image        = 3;

const
  ISEEIRW_MAX_BITCOUNT = 32;            // ISeeͼ���д���֧�ֵ����λ��ֵ

//==============================================================================
// ��� API ����
//==============================================================================

{ ��� API ԭ�Ͷ��� }

type
  // ���ISeeͼ���д����ڲ���Ϣ�����Ľӿ�ָ�����ͣ��ڲ�ʹ�ã�
  TIsIrwPluginProc = function(): PIrwpInfo; cdecl;
  // get_image_info�ӿ�ָ������
  TGetImageInfoProc = function(psct: PIsAddr; pinfo: PInfoStr): TExeResult; cdecl;
  // load_image�ӿ�ָ������
  TLoadImageProc = function(psct: PIsAddr; pinfo: PInfoStr): TExeResult; cdecl;
  // save_image�ӿ�ָ������
  TSaveImageProc = function(psct: PIsAddr; pinfo: PInfoStr; lpse: PSaveStr): TExeResult; cdecl;

{ �� iseeirw.dll �е���� API ���� }

// ���DLL�Ƿ���Ч�Ĳ��
function isirw_test_plugin(hmod: HMODULE): Cardinal; cdecl;
// ���ز����Ϣ�ṹ
function isirw_get_info(pfun_add: Cardinal): PIrwpInfo; cdecl;
// ���ز����ָ����API��ַ
function isirw_get_api_addr(hmod: HMODULE; pInfo: PIrwpInfo; name: TIrwpApiName):
  TFarProc; cdecl;

// ����һ���µ����ݰ�
function isirw_alloc_INFOSTR: PInfoStr; cdecl;
// �ͷ�һ�����ݰ�
function isirw_free_INFOSTR(pinfo: PInfoStr): PInfoStr; cdecl;
// ����һ����ͼ���
function isirw_alloc_SUBIMGBLOCK: PSubImgBlock; cdecl;
// �ͷ�һ����ͼ���
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

