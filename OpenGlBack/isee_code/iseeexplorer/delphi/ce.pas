{*******************************************************************************

        ce.pas

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
        ���ļ���;��    ISee ͼ��ת������� Delphi ������Ԫ
        ���ļ�����      ce.pas
        ԭ�ļ�����      ce.h
        ���ļ���д�ˣ�  ���¹���                yygw##163.com
        ԭ�ļ���д�ˣ�  YZ                      yzfree##sina.com

        ���ļ��汾��    20620
        ����޸��ڣ�    2002-06-20
        ԭ�ļ��汾��    20416
        ����޸��ڣ�    2002-4-16

        ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
        ��ַ�ռ������
        ----------------------------------------------------------------
        ���ļ�������ʷ��
                2002-06-20      ��ֲԭ�ļ�Ϊ����Ԫ

        ԭ�ļ�������ʷ��
                2002-4          ������ʽת����������ɫ�����ص���BUG����������
                                        С���Ա tearshark��
                2001-12         С���Ա menghui ���ָ�ʽת��������ȱ�ٶԶ�ҳ
                                        ͼ���֧�֡�����BUG��δ������
                2001-7          ��һ�����԰淢��

*******************************************************************************}

unit ce;
{* |<PRE>
================================================================================
* ������ƣ�ISeeExplorer
* ��Ԫ���ƣ�ISee ͼ��ת������� Delphi ������Ԫ
* ��Ԫ�汾��V1.0
* ��Ԫ���ߣ����¹��� yygw@163.com; http://yygw.126.com
* ԭ �� Ԫ��ce.h
* ԭ �� �ߣ�YZ       yzfree##sina.com
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
  iseeirw;

//==============================================================================
// isee ͼ��ת������ API ����
//==============================================================================

{ �� ce.dll �е���� API ���� }

procedure _1_to_cross(psou: PInfoStr); cdecl;
procedure _4_to_cross(psou: PInfoStr); cdecl;

procedure _v_conv(psou: PInfoStr); cdecl;

function _23to4(psou: PInfoStr): PInfoStr; cdecl;
function _567to8(psou: PInfoStr): PInfoStr; cdecl;
function _9_10_11_12_13_14_15to16(psou: PInfoStr): PInfoStr; cdecl;
function _17_18_19_20_21_22_23to24(psou: PInfoStr): PInfoStr; cdecl;
function _25_26_27_28_29_30_31to32(psou: PInfoStr): PInfoStr; cdecl;

implementation

const
  csCE_DLL     = 'ce.dll';

procedure _1_to_cross; cdecl; external csCE_DLL;
procedure _4_to_cross; cdecl; external csCE_DLL;
procedure _v_conv; cdecl; external csCE_DLL;
function _23to4; cdecl; external csCE_DLL;
function _567to8; cdecl; external csCE_DLL;
function _9_10_11_12_13_14_15to16; cdecl; external csCE_DLL;
function _17_18_19_20_21_22_23to24; cdecl; external csCE_DLL;
function _25_26_27_28_29_30_31to32; cdecl; external csCE_DLL;

end.
