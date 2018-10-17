{*******************************************************************************

        ISee.pas

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
        本文件用途：    图像读写插件 Delphi 封装单元
        本文件名：      ISee.pas
        本文件编写人：  与月共舞                yygw##163.com

        本文件版本：    20629
        最后修改于：    2002-06-29

        注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
        地址收集软件。
        ----------------------------------------------------------------
        本文件修正历史：
                2002-06-29      完成了图像保存功能
                                修正上一版部分错误
                                
                2002-06-20      创建单元

*******************************************************************************}

unit ISee;
{* |<PRE>
================================================================================
* 软件名称：ISeeExplorer
* 单元名称：图像读写插件Delphi实现单元
* 单元版本：V1.1
* 单元作者：与月共舞 yygw@163.com; http://yygw.126.com
* 备    注：
* 开发平台：PWin98SE + Delphi5 + ModelMaker6
* 兼容测试：PWin9X/2000/XP + Delphi 5/6
* 本 地 化：该单元中的字符串均符合本地化处理方式
* 更新记录：2002.06.29 V1.1
*                完成了图像保存功能
*                修正上一版部分错误
*           2002.06.20 V1.0
*                创建单元
================================================================================
|</PRE>}

interface

{$I ISee.inc}

uses
  Windows, Messages, SysUtils, Classes, Graphics, Forms, iseeio, iseeirw, ce;

type
            
//==============================================================================
// ISee 插件作者信息
//==============================================================================
         
{ TISeeAuthorInfoList }

  PISeeAuthorInfo = ^TISeeAuthorInfo;
  {* 插件作者信息指针}
  TISeeAuthorInfo = record
  {* 插件作者信息记录}
    Name: string; 
    {* 插件作者}
    Email: string;
    {* 插件作者电子邮件}
    Comment: string;    
    {* 插件注释}
  end;

  TISeeAuthorInfoList = class (TObject)
  {* ISee图像读写插件作者信息类 }
  private
    FList: TList;
    function Get(Index: Integer): TISeeAuthorInfo;
    function GetCount: Integer;
  protected
    procedure Add(AName, AEmail, AComment: string);
    procedure Clear;
    procedure Delete(Index: Integer);
  public
    constructor Create;
    {* 类构造器 }
    destructor Destroy; override;
    {* 类析构器 }
    property Count: Integer read GetCount;
    {* 项目数 }
    property Items[Index: Integer]: TISeeAuthorInfo read Get; default;
    {* 作者信息数组属性 }
  end;
  
//==================================================================
// ISee 插件保存参数描述
//==============================================================================

{ TISeeSaveDescItem }

  TISeeSaveDescItem = class (TObject)
  {* 保存参数描述项类 }
  private
    FDescStr: string;
    FItems: TStrings;
    FValue: Integer;
    function GetCount: Integer;
    function GetNames(Index: Integer): string;
    procedure SetNames(Index: Integer; Value: string);
    function GetValues(Index: Integer): Integer;
    procedure SetValues(Index: Integer; const Value: Integer);
  protected
    procedure Add(const AName: string; AValue: Integer);
  public
    constructor Create;
    {* 类构造器 }
    destructor Destroy; override;
    {* 类析构器 }
    property Count: Integer read GetCount;
    {* 项目数 }
    property DescStr: string read FDescStr;
    {* 参数描述字符串 }
    property Names[Index: Integer]: string read GetNames write SetNames;
    {* 值列表的描述数组属性 }
    property Value: Integer read FValue write FValue;
    {* 该参数项的设置值，在保存图像时传递给插件 }
    property Values[Index: Integer]: Integer read GetValues write SetValues;
    {* 值列表的值数组属性 }
  end;
  
{ TISeeSaveDescList }

  TISeeSaveNum = (inNone, inSingle, inMulti);
  {* 插件保存功能支持的图像数
   |<PRE>
     inNone        - 无要求
     inSingle      - 只允许一张图像
     inMulti       - 允许多张图像
   |</PRE>}

  TISeeSaveDescList = class (TObject)
  {* ISee图像读写插件保存参数描述类 }
  private
    FBitCount: Cardinal;
    FImgNum: TISeeSaveNum;
    FList: TList;
    function Get(Index: Integer): TISeeSaveDescItem;
    function GetCount: Integer;
  protected
    function Add: TISeeSaveDescItem;
    procedure Clear;
    procedure Delete(Index: Integer);
  public
    constructor Create;
    {* 类构造器 }
    destructor Destroy; override;
    {* 类析构器 }
    property BitCount: Cardinal read FBitCount;
    {* 保存图像时支持的位深参数 }
    property Count: Integer read GetCount;
    {* 项目数 }
    property ImgNum: TISeeSaveNum read FImgNum;
    {* 插件保存功能支持的图像数 }
    property Items[Index: Integer]: TISeeSaveDescItem read Get; default;
    {* 保存参数项数组 }
  end;
  
//==============================================================================
// ISee 图像类
//==============================================================================

{ TISeeSubImageList }

  PISeeSubImage = ^TISeeSubImage;

  TISeeSubImage = record
  {* ISee子图像记录类型}
    Left, Top: Integer;
    {* 子图像块的显示位置（左上角）}
    DoWith: Integer;
    {* 处置方法（只针对动态图像）}
    UserInput: Integer;
    {* 用户输入（只针对动态图像）}
    Time: Integer;
    {* 显示的延迟时间（只针对动态图像）}
    Image: TBitmap;
    {* 子图像位图}
  end;

  TISeeSubImageList = class (TObject)
  {* 子图像列表类 }
  private
    FList: TList;
    function Get(Index: Integer): TISeeSubImage;
    function GetCount: Integer;
  public
    constructor Create;
    {* 类构造器 }
    destructor Destroy; override;
    {* 类析构器 }
    function Add: PISeeSubImage;
    {* 增加一条记录，返回记录指针 }
    procedure Clear;
    {* 清空所有记录 }
    procedure Delete(Index: Integer);
    {* 删除一条记录，参数为索引号 }
    property Count: Integer read GetCount;
    {* 项目数 }
    property Items[Index: Integer]: TISeeSubImage read Get; default;
    {* 保存参数描述信息数组属性 }
  end;
  
{ TISeePlayOrder }

  TISeePlayOrder = class (TObject)
  {* 动态图像播放顺序表类 }
  private
    FList: TList;
    function Get(Index: Integer): Integer;
    function GetCount: Integer;
  protected
    procedure Add(Order: Integer);
    procedure Clear;
    procedure Delete(Index: Integer);
  public
    constructor Create;
    {* 类构造器 }
    destructor Destroy; override;
    {* 类析构器 }
    property Count: Integer read GetCount;
    {* 项目数 }
    property Items[Index: Integer]: Integer read Get; default;
    {* 播放顺序数组 }
  end;
  
{ TISeeBitmap }

  TISeeBitmap = class (TBitmap)
  {* 支持多幅图像的Bitmap类 }
  private
    FBitCount: Integer;
    FImgCompress: TImgCompress;
    FImgFormat: TImgFormat;
    FImgType: TImgType;
    FPlayOrder: TISeePlayOrder;
    FSubImage: TISeeSubImageList;
    FTime: Integer;
    FUpdateCount: Integer;
  protected
    procedure Changed(Sender: TObject); override;
    procedure SetUpdating(Updating: Boolean);
    function Updating: Boolean;
  public
    constructor Create; override;
    {* 类构造器 }
    destructor Destroy; override;
    {* 类析构器 }
    procedure Assign(Source: TPersistent); override;
    {* 对象赋值方法 }
    procedure BeginUpdate;
    {* 开始更新，必须与EndUpdate配对 }
    procedure EndUpdate;
    {* 结束更新，必须与BeginUpdate配对使用 }
    procedure LoadFromFile(const Filename: string); override;
    {* 从文件中装载图像 }
    procedure LoadFromStream(Stream: TStream); override;
    {* 从流中装载图像 }
    procedure SaveToFile(const Filename: string); override;
    {* 保存图像到文件 }
    procedure SaveToStream(Stream: TStream); override;
    {* 保存图像到流 }
    property BitCount: Integer read FBitCount;
    {* 原图像位深 }
    property ImgCompress: TImgCompress read FImgCompress;
    {* 图像文件压缩方式 }
    property ImgFormat: TImgFormat read FImgFormat;
    {* 图像文件格式 }
    property ImgType: TImgType read FImgType;
    {* 图像文件类型 }
    property PlayOrder: TISeePlayOrder read FPlayOrder;
    {* 播放顺序列表 }
    property SubImage: TISeeSubImageList read FSubImage;
    {* 子图像列表 }
    property Time: Integer read FTime;
    {* 显示时间（动态图像） }
  end;
  
//==============================================================================
// ISee 图像读写插件子项类
//==============================================================================

{ TISeePluginItem }
                  
  TISeePluginStyle = (isRead, isWrite);
  {* 读写插件类型
   |<PRE>
     isRead        - 插件支持读图像功能
     isWrite       - 插件支持图像保存功能
   |</PRE>}
  TISeePluginStyles = set of TISeePluginStyle;
  {* 读写插件类型，集合类型}

//==============================================================================
// ISee 图像读写插件管理类
//==============================================================================
 
{ TISeePluginItem }

  TISeePluginItem = class (TCollectionItem)
  {* ISee图像读写插件子项类 }
  private
    FAuthorInfo: TISeeAuthorInfoList;
    FDesc: string;
    FDLLName: string;
    FExt: string;
    FExts: TStrings;
    FFunctionPrefix: string;
    FGetImageInfo: TGetImageInfoProc;
    FHandle: HMODULE;
    FLoadImage: TLoadImageProc;
    FName: string;
    FSaveDesc: TISeeSaveDescList;
    FSaveImage: TSaveImageProc;
    FStyle: TISeePluginStyles;
    FVersion: Cardinal;
  protected
    property FunctionPrefix: string read FFunctionPrefix;
    property GetImageInfo: TGetImageInfoProc read FGetImageInfo;
    property Handle: HMODULE read FHandle;
    property LoadImage: TLoadImageProc read FLoadImage;
    property SaveImage: TSaveImageProc read FSaveImage;
  public
    constructor Create(Collection: TCollection); override;
    {* 类构造器 }
    destructor Destroy; override;
    {* 类析构器 }
    property AuthorInfo: TISeeAuthorInfoList read FAuthorInfo;
    {* 插件作者信息 }
    property Desc: string read FDesc;
    {* 插件支持的格式的描述 }
    property DLLName: string read FDLLName;
    {* 插件文件名，带完整路径 }
    property Ext: string read FExt;
    {* 插件通用扩展名 }
    property Exts: TStrings read FExts;
    {* 插件衍生扩展名列表 }
    property Name: string read FName;
    {* 插件名称 }
    property SaveDesc: TISeeSaveDescList read FSaveDesc;
    {* 保存参数描述信息 }
    property Style: TISeePluginStyles read FStyle;
    {* 图像读写插件类型 }
    property Version: Cardinal read FVersion;
    {* 插件版本号 }
  end;
  
{ TISeePlugins }

  TISeeExcuteResult = (irSuccess, irUserBreak, irBadImage, irNonImage,
    irMemoryErr, irFileRWErr, irSysErr, irNotSupport, irNSImgFor, irUnknow);
  {* 图像操作执行结果
   |<PRE>
     irSuccess     - 执行成功
     irUserBreak   - 操作被用户中断
     irBadImage    - 指定的文件是一个受损的图像文件
     irNonImage    - 指定的文件不是一个图像文件
     irMemoryErr   - 内存不足，导致操作失败
     irFileRWErr   - 文件在读写过程中发生错误（系统、硬件等原因引起文件读写异常）
     irSysErr      - 操作系统不稳定，无法执行命令
     irNotSupport  - 不支持的命令
     irNSImgFor    - 保存功能不支持的图像格式
     irUnknow      - 其它错误
   |</PRE>}

  TISeePlugins = class (TCollection)
  {* ISee图像读写插件管理类 }
  private
    FAutoRefresh: Boolean;
    FPluginExtName: string;
    FPluginPath: TStrings;
    procedure PluginPathChanged(Sender: TObject);
    function GetItem(Index: Integer): TISeePluginItem;
    procedure SetItem(Index: Integer; const Value: TISeePluginItem);
    procedure SetPluginExtName(Value: string);
    procedure SetPluginPath(Value: TStrings);
  protected
    function AddPlugin(const DLLName: string): Integer;
    procedure UpdatePlugins;
  public
    constructor Create;
    {* 类构造器 }
    destructor Destroy; override;
    {* 类析构器 }
    function BuildFilter(Index: Integer): string;
    {* 创建插件扩展名过滤字符串 }
    function BuildLoadFilter: string;
    {* 返回可装载的所有格式扩展名过滤字符串 }
    function BuildSaveFilter: string;
    {* 返回可保存的所有格式扩展名过滤字符串 }
    function FindPlugin(AExt: string; AStyle: TISeePluginStyle): 
      TISeePluginItem;
    {* 根据扩展名查找插件，返回插件索引号 }
    function IndexOf(AName: string): Integer;
    {* 根据插件名查找插件，返回插件索引号 }
    function LoadFromFile(const FileName: string; Bmp: TISeeBitmap): 
      TISeeExcuteResult;
    {* 从文件中装载图像 }
    function LoadFromStream(Stream: TStream; Bmp: TISeeBitmap): 
      TISeeExcuteResult;
    {* 从流中装载图像 }
    procedure Refresh;
    {* 刷新当前插件列表 }
    function SaveToFile(const FileName: string; Bmp: TISeeBitmap): 
      TISeeExcuteResult;
    {* 保存图像到文件 }
    function SaveToStream(Stream: TStream; Bmp: TISeeBitmap): TISeeExcuteResult;
    {* 保存图像到流 }
    property AutoRefresh: Boolean read FAutoRefresh write FAutoRefresh default 
      True;
    {* 路径变更时自动刷新 }
    property Items[Index: Integer]: TISeePluginItem read GetItem write SetItem; 
      default;
    {* 插件数组属性 }
    property PluginExtName: string read FPluginExtName write SetPluginExtName;
    {* 插件扩展名，默认为 dll }
    property PluginPath: TStrings read FPluginPath write SetPluginPath;
    {* 插件路径列表 }
  end;
  
var
  ISeePlugins: TISeePlugins;
  
implementation

{
***************************** TISeeAuthorInfoList ******************************
}
{* 类构造器 }
constructor TISeeAuthorInfoList.Create;
begin
  inherited Create;
  FList := TList.Create;
end;

{* 类析构器 }
destructor TISeeAuthorInfoList.Destroy;
begin
  Clear;
  FList.Free;
  inherited Destroy;
end;

{* 增加一条记录 }
procedure TISeeAuthorInfoList.Add(AName, AEmail, AComment: string);
var
  NewRec: PISeeAuthorInfo;
begin
  New(NewRec);
  with NewRec^ do
  begin
    Name := AName;
    Email := AEmail;
    Comment := AComment;
  end;
  FList.Add(NewRec);
end;

{* 清空所有记录 }
procedure TISeeAuthorInfoList.Clear;
begin
  while Count > 0 do Delete(0);
end;

{* 删除一条记录 }
procedure TISeeAuthorInfoList.Delete(Index: Integer);
begin
  Dispose(FList.Items[Index]);
  FList.Delete(Index);
end;

{* Items属性的读方法 }
function TISeeAuthorInfoList.Get(Index: Integer): TISeeAuthorInfo;
begin
  Result := PISeeAuthorInfo(FList.Items[Index])^;
end;

{* Count的读方法 }
function TISeeAuthorInfoList.GetCount: Integer;
begin
  Result := FList.Count;
end;

{
****************************** TISeeSaveDescItem *******************************
}
{* 类构造器 }
constructor TISeeSaveDescItem.Create;
begin
  inherited Create;
  FItems := TStringList.Create;
end;

{* 类析构器 }
destructor TISeeSaveDescItem.Destroy;
begin
  FItems.Free;
  inherited Destroy;
end;

{* 增加一项 }
procedure TISeeSaveDescItem.Add(const AName: string; AValue: Integer);
begin
  FItems.AddObject(AName, TObject(AValue));
end;

{* Count属性的读方法 }
function TISeeSaveDescItem.GetCount: Integer;
begin
  Result := FItems.Count;
end;

{* Names属性的读方法 }
function TISeeSaveDescItem.GetNames(Index: Integer): string;
begin
  Result := FItems[Index];
end;

{* Names属性的写方法 }
procedure TISeeSaveDescItem.SetNames(Index: Integer; Value: string);
begin
  FItems[Index] := Value;
end;

{* Values属性的读方法 }
function TISeeSaveDescItem.GetValues(Index: Integer): Integer;
begin
  Result := Integer(FItems.Objects[Index]);
end;

{* Values属性的写方法 }
procedure TISeeSaveDescItem.SetValues(Index: Integer; const Value: Integer);
begin
  FItems.Objects[Index] := Pointer(Value);
end;

{
****************************** TISeeSaveDescList *******************************
}
{* 类构造器 }
constructor TISeeSaveDescList.Create;
begin
  inherited Create;
  FList := TList.Create;
end;

{* 类析构器 }
destructor TISeeSaveDescList.Destroy;
begin
  Clear;
  FList.Free;
  inherited Destroy;
end;

{* 增加一条记录 }
function TISeeSaveDescList.Add: TISeeSaveDescItem;
begin
  Result := TISeeSaveDescItem.Create;
  FList.Add(Pointer(Result));
end;

{* 清空所有记录 }
procedure TISeeSaveDescList.Clear;
begin
  while Count > 0 do Delete(0);
end;

{* 删除一条记录 }
procedure TISeeSaveDescList.Delete(Index: Integer);
begin
  TISeeSaveDescItem(FList[Index]).Free;
  FList.Delete(Index);
end;

{* Items属性的读方法 }
function TISeeSaveDescList.Get(Index: Integer): TISeeSaveDescItem;
begin
  Result := TISeeSaveDescItem(FList.Items[Index]);
end;

{* Count的读方法 }
function TISeeSaveDescList.GetCount: Integer;
begin
  Result := FList.Count;
end;

{
****************************** TISeeSubImageList *******************************
}
{* 类构造器 }
constructor TISeeSubImageList.Create;
begin
  inherited Create;
  FList := TList.Create;
end;

{* 类析构器 }
destructor TISeeSubImageList.Destroy;
begin
  Clear;
  FList.Free;
  inherited Destroy;
end;

{* 增加一条记录，返回记录指针 }
function TISeeSubImageList.Add: PISeeSubImage;
begin
  New(Result);
  Result^.Image := TBitmap.Create;
  FList.Add(Result);
end;

{* 清空所有记录 }
procedure TISeeSubImageList.Clear;
begin
  while Count > 0 do Delete(0);
end;

{* 删除一条记录，参数为索引号 }
procedure TISeeSubImageList.Delete(Index: Integer);
begin
  FreeAndNil(PISeeSubImage(FList.Items[Index])^.Image);
  Dispose(FList.Items[Index]);
  FList.Delete(Index);
end;

{* Items属性读方法 }
function TISeeSubImageList.Get(Index: Integer): TISeeSubImage;
begin
  Result := PISeeSubImage(FList.Items[Index])^;
end;

{* Count的读方法 }
function TISeeSubImageList.GetCount: Integer;
begin
  Result := FList.Count;
end;

{
******************************** TISeePlayOrder ********************************
}
{* 类构造器 }
constructor TISeePlayOrder.Create;
begin
  inherited Create;
  FList := TList.Create;
end;

{* 类析构器 }
destructor TISeePlayOrder.Destroy;
begin
  Clear;
  FList.Free;
  inherited Destroy;
end;

{* 增加一条记录 }
procedure TISeePlayOrder.Add(Order: Integer);
begin
  FList.Add(Pointer(Order));
end;

{* 清空所有记录 }
procedure TISeePlayOrder.Clear;
begin
  while Count > 0 do Delete(0);
end;

{* 删除一条记录 }
procedure TISeePlayOrder.Delete(Index: Integer);
begin
  FList.Delete(Index);
end;

{* Items属性的读方法 }
function TISeePlayOrder.Get(Index: Integer): Integer;
begin
  Result := Integer(FList.Items[Index]);
end;

{* Count的读方法 }
function TISeePlayOrder.GetCount: Integer;
begin
  Result := FList.Count;
end;

{
********************************* TISeeBitmap **********************************
}
{* 类构造器 }
constructor TISeeBitmap.Create;
begin
  inherited Create;
  FPlayOrder := TISeePlayOrder.Create;
  FSubImage := TISeeSubImageList.Create;
end;

{* 类析构器 }
destructor TISeeBitmap.Destroy;
begin
  FSubImage.Free;
  FPlayOrder.Free;
  inherited Destroy;
end;

{* 对象赋值方法 }
procedure TISeeBitmap.Assign(Source: TPersistent);
var
  i: Integer;
begin
  BeginUpdate;
  try
    FSubImage.Clear;
    FPlayOrder.Clear;
    FBitCount := 0;
    FImgCompress := 0;
    FImgFormat := 0;
    FImgType := 0;
    FTime := 0;
    if Source is TISeeBitmap then
    begin
      FBitCount := TISeeBitmap(Source).FBitCount;
      FImgCompress := TISeeBitmap(Source).FImgCompress;
      FImgFormat := TISeeBitmap(Source).FImgFormat;
      FImgType := TISeeBitmap(Source).FImgType;
      FTime := TISeeBitmap(Source).FTime;
      for i := 0 to TISeeBitmap(Source).FPlayOrder.Count - 1 do
        FPlayOrder.Add(TISeeBitmap(Source).FPlayOrder[i]);
      for i := 0 to TISeeBitmap(Source).FSubImage.Count - 1 do
      begin
        with FSubImage.Add^ do
        begin
          Left := TISeeBitmap(Source).FSubImage[i].Left;
          Top := TISeeBitmap(Source).FSubImage[i].Top;
          DoWith := TISeeBitmap(Source).FSubImage[i].DoWith;
          UserInput := TISeeBitmap(Source).FSubImage[i].UserInput;
          Image.Assign(TISeeBitmap(Source).FSubImage[i].Image);
        end;
      end;
    end;
    inherited Assign(Source);
  finally
    EndUpdate;
  end;
end;

{* 开始更新，必须与EndUpdate配对 }
procedure TISeeBitmap.BeginUpdate;
begin
  Inc(FUpdateCount);
  if FUpdateCount = 1 then SetUpdating(True);
end;

{* 内容已变更 }
procedure TISeeBitmap.Changed(Sender: TObject);
begin
  if not Updating then
    inherited Changed(Sender);
end;

{* 结束更新，必须与BeginUpdate配对使用 }
procedure TISeeBitmap.EndUpdate;
begin
  Dec(FUpdateCount);
  if FUpdateCount = 0 then SetUpdating(False);
end;

{* 从文件中装载图像 }
procedure TISeeBitmap.LoadFromFile(const Filename: string);
begin
  BeginUpdate;
  try
    ISeePlugins.LoadFromFile(FileName, Self);
  finally
    EndUpdate;
  end;
end;

{* 从流中装载图像 }
procedure TISeeBitmap.LoadFromStream(Stream: TStream);
begin
  BeginUpdate;
  try
    ISeePlugins.LoadFromStream(Stream, Self);
  finally
    EndUpdate;
  end;
end;

{* 保存图像到文件 }
procedure TISeeBitmap.SaveToFile(const Filename: string);
begin
  ISeePlugins.SaveToFile(FileName, Self);
end;

{* 保存图像到流 }
procedure TISeeBitmap.SaveToStream(Stream: TStream);
begin
  ISeePlugins.SaveToStream(Stream, Self);
end;

{* 更新通知 }
procedure TISeeBitmap.SetUpdating(Updating: Boolean);
begin
  if not Updating then Changed(Self);
end;

{* 正在更新 }
function TISeeBitmap.Updating: Boolean;
begin
  Result := (FUpdateCount <> 0);
end;

{
******************************* TISeePluginItem ********************************
}
{* 类构造器 }
constructor TISeePluginItem.Create(Collection: TCollection);
begin
  inherited Create(Collection);
  FExts := TStringList.Create;
  FAuthorInfo := TISeeAuthorInfoList.Create;
  FSaveDesc := TISeeSaveDescList.Create;
end;

{* 类析构器 }
destructor TISeePluginItem.Destroy;
begin
  FAuthorInfo.Free;
  FSaveDesc.Free;
  FExts.Free;
  FreeLibrary(Handle);
  inherited Destroy;
end;

{
********************************* TISeePlugins *********************************
}
{* 类构造器 }
constructor TISeePlugins.Create;
begin
  inherited Create(TISeePluginItem);
  FPluginExtName := 'DLL';
  FPluginPath := TStringList.Create;
  TStringList(FPluginPath).OnChange := PluginPathChanged;
  FAutoRefresh := False;
end;

{* 类析构器 }
destructor TISeePlugins.Destroy;
begin
  FPluginPath.Free;
  inherited Destroy;
end;

{* 增加一个插件 }
function TISeePlugins.AddPlugin(const DLLName: string): Integer;
var
  i: Integer;
  j: Integer;
  Handle: HMODULE;
  Info: PIrwpInfo;
  FunAdd: Integer;
  NewItem: TISeePluginItem;
begin
  Result := -1;
  Handle := LoadLibrary(PChar(DLLName)); // 打开插件
  if Handle <> 0 then
  begin
    FunAdd := isirw_test_plugin(Handle); // 测试插件
    if FunAdd <> 0 then
    begin
      Info := isirw_get_info(FunAdd); // 取插件信息
      if Info <> nil then
      begin
        NewItem := TISeePluginItem(Add); // 增加一个插件项
        NewItem.FDLLName := DLLName; // 插件文件名
        NewItem.FHandle := Handle; // 模块地址
        NewItem.FName := Info^.irwp_name; // 插件名
        NewItem.FDesc := Info^.irwp_name + '-格式'; // 插件描述
        { TODO -oyygw -c预留接口 : 插件格式说明当前为插件名+"-格式"，等待新接口定义。 }
        NewItem.FFunctionPrefix := Info^.irwp_func_prefix; // 插件函数前缀
        NewItem.FVersion := Info^.irwp_version; // 插件版本
        NewItem.FStyle := []; // 插件功能
        if (Info^.irwp_function and $01) <> 0 then
          Include(NewItem.FStyle, isRead); // 读支持
        if (Info^.irwp_function and $02) <> 0 then
          Include(NewItem.FStyle, isWrite); // 写支持
        NewItem.FExt := Info^.irwp_desc_info.idi_currency_name; // 通用扩展名
        for i := 0 to Info^.irwp_desc_info.idi_synonym_count - 1 do
          NewItem.FExts.Add(Info^.irwp_desc_info.idi_synonym[i]); // 衍生扩展名
        for i := 0 to Info^.irwp_author_count - 1 do // 作者信息
          NewItem.FAuthorInfo.Add(Info^.irwp_author[i].ai_name,
            Info^.irwp_author[i].ai_email, Info^.irwp_author[i].ai_message);
        NewItem.FSaveDesc.FBitCount := Info^.irwp_save.bitcount; // 保存参数描述
        case Info^.irwp_save.img_num of
          0: NewItem.FSaveDesc.FImgNum := inNone;
          1: NewItem.FSaveDesc.FImgNum := inSingle;
          else NewItem.FSaveDesc.FImgNum := inMulti;
        end;
        for i := 0 to Info^.irwp_save.count - 1 do
          with NewItem.FSaveDesc.Add do
          begin
            FDescStr := Info^.irwp_save.para[i].desc;
            if Info^.irwp_save.para[i].count > 0 then
              FValue := Info^.irwp_save.para[i].value[0];
              { TODO -oyygw -c预留接口 : 插件存储参数默认值当前为第一项的值，等待新接口定义。 }
            for j := 0 to Info^.irwp_save.para[i].count - 1 do
              Add(Info^.irwp_save.para[i].value_desc[j], Info^.irwp_save.para[i].value[j]);
          end;
        // API地址
        NewItem.FGetImageInfo := isirw_get_api_addr(Handle, Info, get_image_info);
        NewItem.FLoadImage := isirw_get_api_addr(Handle, Info, load_image);
        NewItem.FSaveImage := isirw_get_api_addr(Handle, Info, save_image);
        Result := IndexOf(NewItem.Name);
        Exit; // 退出
      end;
    end;
    FreeLibrary(Handle); // 如果出错释放库
  end;
end;

{* 创建插件扩展名过滤字符串 }
function TISeePlugins.BuildFilter(Index: Integer): string;
var
  i: Integer;
  Item: TISeePluginItem;
  AFilter: string;
  ADesc: string;
begin
  if (Index >= 0) and (Index < Count) then
  begin
    Item := Items[Index];
    AFilter := '*.' + Item.Ext;
    for i := 0 to Item.Exts.Count - 1 do
      AFilter := AFilter + ';*.' + Item.Exts[i];
    AFilter := LowerCase(AFilter);
    ADesc := Format('%s (%s)', [Item.Desc, AFilter]);
    Result := Format('%s|%s', [ADesc, AFilter]);
  end;
end;

{* 返回可装载的所有格式扩展名过滤字符串 }
function TISeePlugins.BuildLoadFilter: string;
var
  i: Integer;
  j: Integer;
  AFilter: string;
  ADesc: string;
begin
  if Count <= 0 then Exit;
  for i := 0 to Count - 1 do
  begin
    if isRead in Items[i].Style then
    begin
      if AFilter <> '' then
        AFilter := AFilter + ';';
      AFilter := AFilter + '*.' + Items[i].Ext;
      for j := 0 to Items[i].Exts.Count - 1 do
        AFilter := AFilter + ';*.' + Items[i].Exts[j];
      Result := Result + '|' + BuildFilter(i);
    end;
  end;
  AFilter := LowerCase(AFilter);
  ADesc := Format('所有支持的格式 (%s)', [AFilter]);
  Result := Format('%s|%s%s', [ADesc, AFilter, Result]);
end;

{* 返回可保存的所有格式扩展名过滤字符串 }
function TISeePlugins.BuildSaveFilter: string;
var
  i: Integer;
begin
  if Count <= 0 then Exit;
  for i := 0 to Count - 1 do
    if isWrite in Items[i].Style then
      if Result = '' then
        Result := BuildFilter(i)
      else
        Result := Result + '|' + BuildFilter(i);
end;

{* 根据扩展名查找插件，返回插件索引号 }
function TISeePlugins.FindPlugin(AExt: string; AStyle: TISeePluginStyle): 
  TISeePluginItem;
var
  i: Integer;
  
  function CompareExt(const Ext1, Ext2: string): Boolean;
  var
    s1, s2: string;
  begin
    if (Ext1 = '') or (Ext2 = '') then
    begin
      Result := False;
      Exit;
    end;
    s1 := Ext1;
    s2 := Ext2;
    if s1[1] = '.' then System.Delete(s1, 1, 1);
    if s2[1] = '.' then System.Delete(s2, 1, 1);
    Result := CompareText(s1, s2) = 0;
  end;
  
  function CompareExts(const Ext: string; Exts: TStrings): Boolean;
  var
    i: Integer;
  begin
    Result := False;
    for i := 0 to Exts.Count - 1 do
      if CompareExt(Ext, Exts[i]) then
      begin
        Result := True;
        Exit;
      end;
  end;
  
begin
  for i := 0 to Count - 1 do // 先查找主扩展名
    if (AStyle in Items[i].Style) and CompareExt(AExt, Items[i].Ext) then
    begin
      Result := Items[i];
      Exit;
    end;
  for i := 0 to Count - 1 do // 再查找衍生扩展名
    if (AStyle in Items[i].Style) and  CompareExts(AExt, Items[i].Exts) then
    begin
      Result := Items[i];
      Exit;
    end;
  Result := nil;
end;

{* 根据插件名查找插件，返回插件索引号 }
function TISeePlugins.IndexOf(AName: string): Integer;
var
  i: Integer;
begin
  for i := 0 to Count - 1 do
    if Items[i].Name = AName then
    begin
      Result := i;
      Exit;
    end;
  Result := -1;
end;

{* 从文件中装载图像 }
function TISeePlugins.LoadFromFile(const FileName: string; Bmp: TISeeBitmap): 
  TISeeExcuteResult;
var
  i: Integer;
  Info: PInfoStr;
  InfoTmp: PInfoStr;
  Item: TISeePluginItem;
  IsAddr: TIsAddr;
  PSub: PSubImgBlock;
  
  procedure CopyToBitmap(AWidth, AHeight, ABitCount, AOrder: Cardinal;
    rm, gm, bm, am: Cardinal; PData: Pointer; PalCount: Cardinal;
    PPal: Pointer; AColorKey: Integer; ABmp: TBitmap);
  type
    PBmpInfo = ^TBmpInfo;
    TBmpInfo = packed record
      bmiHeader: TBitmapInfoHeader;
      bmiColors: array[0..255] of TRGBQuad;
    end;
  var
    Info: TBmpInfo;
  begin
    case ABitCount of
      //1: ABmp.PixelFormat := pf1bit;
      { TODO -oyygw -c未知现象 : 图像格式设置为pf1bit时，显示颜色不正常，故用pf4bit代替 }
      1..4: ABmp.PixelFormat := pf4bit;
      8: ABmp.PixelFormat := pf8bit;
      16: ABmp.PixelFormat := pf16bit;
      24: ABmp.PixelFormat := pf24bit;
      32: ABmp.PixelFormat := pf32bit;
    end;
    FillChar(Info, SizeOf(Info), 0);
    Info.bmiHeader.biSize := SizeOf(TBitmapInfoHeader);
    Info.bmiHeader.biWidth := AWidth;
    if AOrder = 0 then
      Info.bmiHeader.biHeight := -AHeight
    else
      Info.bmiHeader.biHeight := AHeight;
    Info.bmiHeader.biPlanes := 1;
    Info.bmiHeader.biBitCount := ABitCount;
    Info.bmiHeader.biCompression := BI_RGB;
    Info.bmiHeader.biSizeImage := 0;
    Info.bmiHeader.biClrUsed := 0;
    if ABitCount <= 8 then
      Move(PPal^, Info.bmiColors, PalCount * SizeOf(TRGBQuad))
    else
    begin
      Info.bmiColors[0] := tagRGBQUAD(rm);
      Info.bmiColors[1] := tagRGBQUAD(gm);
      Info.bmiColors[2] := tagRGBQUAD(bm);
    end;
    ABmp.Width := AWidth;
    ABmp.Height := AHeight;
    ABmp.Transparent := AColorKey <> -1;
    if ABmp.Transparent then
      ABmp.TransparentColor := AColorKey;
    SetDIBitsToDevice(ABmp.Canvas.Handle, 0, 0, AWidth, AHeight, 0, 0, 0,
      AHeight, PData, PBitmapInfo(@Info)^, DIB_RGB_COLORS);
  end;
  
begin
  Result := irUnknow;
  if (Bmp = nil) or (FileName = '') then Exit;
  
  Item := FindPlugin(ExtractFileExt(FileName), isRead);
  if Item <> nil then
  begin
    Info := isirw_alloc_INFOSTR;
    try
      isio_open_param(@IsAddr, ISEEIO_CONFER_LOC_FILE, PChar(FileName), 0, 0);
      Result := TISeeExcuteResult(Item.LoadImage(@IsAddr, Info));
      if not (Result in [Low(Result)..High(Result)]) then
        Result := irUnknow;
      if Result = irSuccess then
      begin
        case Info^.bitcount of    // ISee 图像流格式转 Windows 格式
          1: _1_to_cross(Info);
          2..3:
            begin
              InfoTmp := _23to4(Info);
              isirw_free_INFOSTR(Info);
              Info := InfoTmp;
              _4_to_cross(Info);
            end;
          4: _4_to_cross(Info);
          5..7:
            begin
              InfoTmp := _567to8(Info);
              isirw_free_INFOSTR(Info);
              Info := InfoTmp;
            end;
          8: ;
          9..16:
            begin
              InfoTmp := _9_10_11_12_13_14_15to16(Info);
              isirw_free_INFOSTR(Info);
              Info := InfoTmp;
            end;
          17..23:
            begin
              InfoTmp := _17_18_19_20_21_22_23to24(Info);
              isirw_free_INFOSTR(Info);
              Info := InfoTmp;
            end;
          24:
            begin
              if (Info^.r_mask <> $FF0000) or (Info^.g_mask <> $FF00) or
                (Info^.b_mask <> $FF) then
              begin
                InfoTmp := _17_18_19_20_21_22_23to24(Info);
                isirw_free_INFOSTR(Info);
                Info := InfoTmp;
              end;
            end;
          25..31:
            begin
              InfoTmp := _25_26_27_28_29_30_31to32(Info);
              isirw_free_INFOSTR(Info);
              Info := InfoTmp;
            end;
          32:
            begin
              if (Info^.r_mask <> $FF0000) or (Info^.g_mask <> $FF00) or
                (Info^.b_mask <> $FF) then
              begin
                InfoTmp := _25_26_27_28_29_30_31to32(Info);
                isirw_free_INFOSTR(Info);
                Info := InfoTmp;
              end;
            end;
        end;
        if Info = nil then
        begin
          Result := irUnknow;
          Exit;
        end;
        if Info^.order = 0 then
          _v_conv(Info); // 转为倒向图
        Bmp.FImgType := Info^.imgtype;
        Bmp.FImgCompress := Info^.compression;
        Bmp.FImgFormat := Info^.imgformat;
        Bmp.FBitCount := Info^.bitcount;
        Bmp.FTime := Info^.time;
        Bmp.PlayOrder.Clear;
        for i := 0 to Info^.play_number - 1 do
          Bmp.PlayOrder.Add(Info^.play_order^[i]);
        with Info^ do
          CopyToBitmap(width, height, bitcount, order, r_mask, g_mask, b_mask,
            a_mask, p_bit_data, pal_count, @palette, colorkey, Bmp);
        Bmp.SubImage.Clear;
        if Info^.imgnumbers > 1 then
        begin
          i := Info^.imgnumbers - 1;
          PSub := Info^.psubimg;
          while (i > 0) and (PSub <> nil) do
          begin
            with Bmp.SubImage.Add^ do
            begin
              Left := PSub^.left;
              Top := PSub^.top;
              DoWith := PSub^.dowith;
              UserInput := PSub^.userinput;
              Time := PSub^.time;
              with PSub^ do
                CopyToBitmap(width, height, bitcount, order, r_mask, g_mask, b_mask,
                  a_mask, p_bit_data, pal_count, @palette, colorkey, Image);
              PSub := PSub^.next;
              Dec(i);
            end;
          end;
        end;
      end;
    finally
      isirw_free_INFOSTR(Info);
    end;
  end;
  if Result <> irSuccess then // 读取失败清空位图
    Bmp.Assign(nil);
end;

{* 从流中装载图像 }
function TISeePlugins.LoadFromStream(Stream: TStream; Bmp: TISeeBitmap): 
  TISeeExcuteResult;
begin
  raise Exception.Create('该功能还未实现');
  { TODO -o与月共舞 -c图像访问 : 从流中读取图像，由于ISeeIO接口的原因，暂未实现 }
end;

{* 插件路径变更通知 }
procedure TISeePlugins.PluginPathChanged(Sender: TObject);
begin
  if AutoRefresh then UpdatePlugins;
end;

{* 刷新当前插件列表 }
procedure TISeePlugins.Refresh;
begin
  UpdatePlugins;
end;

{* 保存图像到文件 }
function TISeePlugins.SaveToFile(const FileName: string; Bmp: TISeeBitmap): 
  TISeeExcuteResult;
var
  Item: TISeePluginItem;
  IsAddr: TIsAddr;
  PSub: PSubImgBlock;
  PSubLast: PSubImgBlock;
  Info: PInfoStr;
  i: Integer;
  SaveStr: TSaveStr;
  
  procedure GetFromBmp(var AWidth, AHeight, ABitCount, AOrder: Cardinal;
    var rm, gm, bm, am: Cardinal; var PData: Pointer; var PalCount: Cardinal;
    PPal: Pointer; var PPLine: PPointerArray; var AColorKey: Integer;
    ABmp: TBitmap);
  type
    PBmpInfo = ^TBmpInfo;
    TBmpInfo = packed record
      bmiHeader: TBitmapInfoHeader;
      bmiColors: array[0..255] of TRGBQuad;
    end;
  var
    dib: DIBSECTION;
    Size: Integer;
    i: Integer;
    Info: TBmpInfo;
  begin
    ABmp.HandleType := bmDIB;
    GetObject(ABmp.Handle, SizeOf(dib), @dib);
    AWidth := dib.dsBmih.biWidth;
    AHeight := Abs(dib.dsBmih.biHeight);
    ABitCount := dib.dsBmih.biBitCount;
    if dib.dsBmih.biHeight > 0 then
      AOrder := 1
    else
      AOrder := 0;
    case ABitCount of
      1: PalCount := 2;
      2: PalCount := 4;
      4: PalCount := 16;
      8: PalCount := 256;
      else PalCount := 0;
    end;
    if PalCount > 0 then
      GetDIBColorTable(ABmp.Canvas.Handle, 0, PalCount, PPal^)
    else
    begin
      rm := dib.dsBitfields[0];
      gm := dib.dsBitfields[1];
      bm := dib.dsBitfields[2];
      if ABitCount = 32 then
        am := $FF000000
      else
        am := 0;
      if rm = 0 then rm := $FF0000;
      if gm = 0 then gm := $FF00;
      if bm = 0 then gm := $FF;
    end;
    if ABmp.Transparent then
      AColorKey := Integer(ABmp.TransparentColor)
    else
      AColorKey := -1;
    Size := (ABitCount * AWidth + 31) div 32 * 4 * AHeight;
    GetMem(PData, Size);
    FillChar(Info, SizeOf(Info), 0);
    Move(dib.dsBmih, Info.bmiHeader, SizeOf(dib.dsBmih));
    if PalCount > 0 then
      Move(PPal^, Info.bmiColors, PalCount * SizeOf(Cardinal))
    else
    begin
      Info.bmiColors[0] := TRGBQuad(rm);
      Info.bmiColors[1] := TRGBQuad(gm);
      Info.bmiColors[2] := TRGBQuad(bm);
    end;
    GetDIBits(ABmp.Canvas.Handle, ABmp.Handle, 0, AHeight, PData,
      PBitmapInfo(@Info)^, DIB_RGB_COLORS);
    GetMem(PPLine, AHeight * SizeOf(Pointer));
    if AOrder = 0 then
    begin
      Size := Integer(ABitCount * AWidth + 31) div 32 * 4;
      PPLine^[0] := PData;
    end
    else
    begin
      Size := -Integer(ABitCount * AWidth + 31) div 32 * 4;
      PPLine^[0] := Pointer(Integer(PData) - Size * Integer(AHeight - 1));
    end;
    for i := 1 to AHeight - 1 do
      PPLine^[i] := Pointer(Integer(PPLine^[0]) + Size * i);
  end;
  
  procedure FreeNilMem(var P: Pointer);
  begin
    if P <> nil then
    begin
      FreeMem(P);
      P := nil;
    end;
  end;
  
begin
  Result := irUnknow;
  if (Bmp = nil) or (Bmp.Height * Bmp.Width = 0) or (FileName = '') then Exit;
  
  Item := FindPlugin(ExtractFileExt(FileName), isWrite);
  if Item <> nil then
  begin
    Info := isirw_alloc_INFOSTR;
    try
      with Info^ do
        GetFromBmp(width, height, bitcount, order, r_mask, g_mask, b_mask,
          a_mask, p_bit_data, pal_count, @palette, pp_line_addr, colorkey, Bmp);
      Info^.data_state := 2; // 有数据
      Info^.imgtype := Bmp.FImgType;
      Info^.compression := Bmp.FImgCompress;
      Info^.imgformat := Bmp.FImgFormat;
      Info^.time := Bmp.FTime;
      if Item.SaveDesc.ImgNum = inMulti then
      begin
        Info^.play_number := Bmp.PlayOrder.Count;
        if Bmp.PlayOrder.Count > 0 then // 处理播放顺序
        begin
          GetMem(Info^.play_order, Bmp.PlayOrder.Count * SizeOf(Integer));
          for i := 0 to Bmp.PlayOrder.Count - 1 do
            Info^.play_order^[i] := Bmp.PlayOrder[i];
        end;
        Info^.imgnumbers := Bmp.SubImage.Count + 1;
        if Bmp.SubImage.Count > 0 then
        begin
          PSubLast := nil;
          for i := 0 to Bmp.SubImage.Count - 1 do
          begin
            PSub := isirw_alloc_SUBIMGBLOCK;
            if i = 0 then
              Info^.psubimg := PSub
            else
              PSubLast^.next := PSub;
            PSub^.number := i + 1;
            PSub^.parents := Info;
            PSub^.prev := PSubLast;
            PSub^.left := Bmp.SubImage[i].Left;
            PSub^.top := Bmp.SubImage[i].Top;
            PSub^.dowith := Bmp.SubImage[i].DoWith;
            PSub^.userinput := Bmp.SubImage[i].UserInput;
            PSub^.time := Bmp.SubImage[i].Time;
            PSubLast := PSub;
            with PSub^ do
              GetFromBmp(width, height, bitcount, order, r_mask, g_mask, b_mask,
                a_mask, p_bit_data, pal_count, @palette, pp_line_addr, colorkey,
                Bmp.SubImage[i].Image);
          end;
        end;
      end
      else
      begin
        Info^.play_number := 0;
        Info^.imgnumbers := 1;
      end;
  
      case Info^.bitcount of    // Windows 图像流格式转 ISee 格式
        1: _1_to_cross(Info);
        4: _4_to_cross(Info);
      end;
  
      isio_open_param(@IsAddr, ISEEIO_CONFER_LOC_FILE, PChar(FileName), 0, 0);
      FillChar(SaveStr, SizeOf(SaveStr), 0);
      for i := 0 to Item.SaveDesc.Count - 1 do
        SaveStr.para_value[i] := Item.SaveDesc[i].Value;
      Result := TISeeExcuteResult(Item.SaveImage(@IsAddr, Info, @SaveStr));
      if not (Result in [Low(Result)..High(Result)]) then
        Result := irUnknow;
    finally
      FreeNilMem(Pointer(Info^.p_bit_data)); // 释放由Delphi分配的内存
      FreeNilMem(Pointer(Info^.play_order));
      FreeNilMem(Pointer(Info^.pp_line_addr));
      PSub := Info^.psubimg;
      while PSub <> nil do
      begin
        FreeNilMem(Pointer(PSub^.p_bit_data));
        FreeNilMem(Pointer(PSub^.pp_line_addr));
        PSub := PSub^.next;
      end;
      isirw_free_INFOSTR(Info);
    end;
  end;
end;

{* 保存图像到流 }
function TISeePlugins.SaveToStream(Stream: TStream; Bmp: TISeeBitmap): 
  TISeeExcuteResult;
begin
  raise Exception.Create('该功能还未实现');
  { TODO -o与月共舞 -c图像访问 : 保存图像到流，由于ISee内存管理接口的原因，暂未实现 }
end;

{* 查找所有插件，更新插件列表 }
procedure TISeePlugins.UpdatePlugins;
var
  i: Integer;
  
  function NotInPlugins(const AName: string): Boolean;
  var
    i: Integer;
  begin
    Result := True;
    for i := 0 to Count - 1 do
      if CompareText(ExtractFileName(Items[i].DLLName), AName) = 0 then
      begin
        Result := False;
        Exit;
      end;
  end;
  
  procedure DoUpdate(const Path: string);
  var
    sr: TSearchRec;
    fr: Integer;
    APath: string;
    AExt: string;
  begin
    APath := Path;
    if APath[Length(Path)] <> '\' then
      APath := APath + '\';
    AExt := '*.' + PluginExtName;
    fr := FindFirst(APath + AExt, faAnyFile, sr);
    while fr = 0 do
    begin
      if (sr.Name <> '.') and (sr.Name <> '..') and (sr.Attr and
        faDirectory = 0) and NotInPlugins(sr.Name) then
        AddPlugin(APath + sr.Name);
      fr := FindNext(sr);
    end;
    FindClose(sr);
  end;
  
begin
  if UpdateCount = 0 then
  begin
    Clear;
    if FPluginPath.Count > 0 then
    begin
      for i := 0 to FPluginPath.Count - 1 do
        DoUpdate(FPluginPath[i]);
    end
    else
      DoUpdate(ExtractFilePath(Application.ExeName));
  end;
end;

{* Item读方法 }
function TISeePlugins.GetItem(Index: Integer): TISeePluginItem;
begin
  Result := TISeePluginItem(inherited Items[Index]);
end;

{* Item写方法 }
procedure TISeePlugins.SetItem(Index: Integer; const Value: TISeePluginItem);
begin
  inherited SetItem(Index, TCollectionItem(Value));
end;

{* PluginExtName的写方法 }
procedure TISeePlugins.SetPluginExtName(Value: string);
begin
  if FPluginExtName <> Value then
  begin
    FPluginExtName := Value;
    if AutoRefresh then UpdatePlugins;
  end;
end;

{* PluginPath写方法 }
procedure TISeePlugins.SetPluginPath(Value: TStrings);
begin
  FPluginPath.Assign(Value);
end;

initialization 
  ISeePlugins := TISeePlugins.Create;  
  
finalization
  ISeePlugins.Free;
end.

