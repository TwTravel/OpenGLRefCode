{*******************************************************************************

        ce.pas

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
        本文件用途：    ISee 图像转换引擎的 Delphi 申明单元
        本文件名：      ce.pas
        原文件名：      ce.h
        本文件编写人：  与月共舞                yygw##163.com
        原文件编写人：  YZ                      yzfree##sina.com

        本文件版本：    20620
        最后修改于：    2002-06-20
        原文件版本：    20416
        最后修改于：    2002-4-16

        注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
        地址收集软件。
        ----------------------------------------------------------------
        本文件修正历史：
                2002-06-20      移植原文件为本单元

        原文件修正历史：
                2002-4          修正格式转换函数中颜色分量重叠的BUG，发现者是
                                        小组成员 tearshark。
                2001-12         小组成员 menghui 发现格式转换函数中缺少对多页
                                        图象的支持。（此BUG尚未修正）
                2001-7          第一个测试版发布

*******************************************************************************}

unit ce;
{* |<PRE>
================================================================================
* 软件名称：ISeeExplorer
* 单元名称：ISee 图像转换引擎的 Delphi 申明单元
* 单元版本：V1.0
* 单元作者：与月共舞 yygw@163.com; http://yygw.126.com
* 原 单 元：ce.h
* 原 作 者：YZ       yzfree##sina.com
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
  iseeirw;

//==============================================================================
// isee 图像转换引擎 API 申明
//==============================================================================

{ 从 ce.dll 中导入的 API 申明 }

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
