{*******************************************************************************

        isee.inc

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
        本文件用途：    在 Delphi/BCB/Kylix 中产生编译指令和编译器版本信息
        本文件名：      isee.inc
        本文件编写人：  yygw                    yygw##163.com

        本文件版本：    20607
        最后修改于：    2002-06-07

        注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
        地址收集软件。
        ----------------------------------------------------------------
        修正历史：
                2002-06-07            创建该文件

*******************************************************************************}

{ 产生编译指令和编译器版本信息 }

{ 包含jedi.inc文件 }
{$I jedi.inc}

//------------------------------------------------------------------------------
// 请根据您的需要更改调试设置
//------------------------------------------------------------------------------
{$DEFINE _Debug}


{**************************** 下面的编译指令请勿更改 **************************}
  {$A+ Force alignment on word/dword boundaries}
  {$S+ stack checking}

  {$B- Short evaluation of boolean values}
  {$H+ Long string support}
  {$V- No var string checking}
  {$X+ Extended syntax}
  {$P+ Open string parameters}
  {$J+ Writeable typed constants}
  {$R- No Range checking}
{$IFDEF DELPHI6}
  {$WARN SYMBOL_PLATFORM ON} 
  {$WARN UNIT_PLATFORM ON}   // 开启D6下的平台兼容性警告
{$ENDIF}

{$IFDEF BCB}
  {$OBJEXPORTALL ON}
{$ENDIF}

// 为调试而附加的设置
{$IFDEF _DEBUG}
  {$HINTS ON; WARNINGS ON}
  {$D+,L+,W+,Y+,YD,Q+,T+}
{$ELSE}
  {$HINTS OFF; WARNINGS OFF}
  {$D-,L-,Y-,Q-,W-,T-}
  {$IMAGEBASE $00400000}
{$ENDIF}