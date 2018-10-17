{*******************************************************************************

        ISeeCtrls.pas

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
        本文件用途：    图像读写插件组件实现单元
        本文件名：      ISeeCtrls.pas
        本文件编写人：  与月共舞                yygw##163.com

        本文件版本：    20629
        最后修改于：    2002-06-29

        注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
        地址收集软件。
        ----------------------------------------------------------------
        本文件修正历史：
                2002-06-29      新增两个控件：TISeeMgr和TISeeImage 
                                完成TISeeSaveDialog动态创建保持参数控件功能

                2002-06-21      创建单元

*******************************************************************************}

unit ISeeCtrls;
{* |<PRE>
================================================================================
* 软件名称：ISeeExplorer
* 单元名称：图像读写插件组件实现单元
* 单元版本：V1.1
* 单元作者：与月共舞 yygw@163.com; http://yygw.126.com
* 备    注：
* 开发平台：PWin98SE + Delphi5 + ModelMaker6
* 兼容测试：PWin9X/2000/XP + Delphi 5/6
* 本 地 化：该单元中的字符串均符合本地化处理方式
* 更新记录：2002.06.29 V1.1
*                新增两个控件：TISeeMgr和TISeeImage
*                完成TISeeSaveDialog动态创建保持参数控件功能
*           2002.06.21 V1.0
*                创建单元
================================================================================
|</PRE>}

interface

{$I ISee.inc}

uses Messages, Windows, SysUtils, Classes, Controls, StdCtrls, Graphics,
  ExtCtrls, Buttons, Dialogs, Dlgs, ExtDlgs, ISee;

type
     
  TISeeImageStyle = (isAutoZoomIn, isAutoZoomOut, isConstrainRate, isColorFrame,
    isCenter);
  {* TISeeImage图像显示风格
   |<PRE>
     isAutoZoomIn       - 自动放大图像以适应显示
     isAutoZoomOut      - 自动缩小图像以适应显示
     isConstrainRate    - 图像缩放时保持比例
     isColorFrame       - 图像边缘绘制彩色边框
     isCenter           - 图像居中显示（不缩放时）
   |</PRE>}
  TISeeImageStyles = set of TISeeImageStyle;
  {* TISeeImage图像显示风格集合类型}

  TISeeImage = class (TGraphicControl)
  {* 使用ISee图像插件接口的图像控件 }
  private
    FActive: Boolean;
    FCurIndex: Integer;
    FDrawing: Boolean;
    FFrameColor: TColor;
    FImageIndex: Integer;
    FIncrementalDisplay: Boolean;
    FOnProgress: TProgressEvent;
    FOrders: array of Integer;
    FPicture: TISeeBitmap;
    FStyle: TISeeImageStyles;
    FTimer: TTimer;
    FTransparent: Boolean;
    procedure OnTimer(Sender: TObject);
    procedure PictureChanged(Sender: TObject);
    procedure UpdateOrders;
    procedure SetActive(const Value: Boolean);
    procedure SetFrameColor(const Value: TColor);
    procedure SetImageIndex(Value: Integer);
    procedure SetPicture(Value: TISeeBitmap);
    procedure SetStyle(const Value: TISeeImageStyles);
    procedure SetTransparent(Value: Boolean);
  protected
    function CanAutoSize(var NewWidth, NewHeight: Integer): Boolean; override;
    function DestRect: TRect;
    function DoPaletteChange: Boolean;
    function GetPalette: HPALETTE; override;
    procedure Paint; override;
    procedure Progress(Sender: TObject; Stage: TProgressStage; PercentDone: 
      Byte; RedrawNow: Boolean; const R: TRect; const Msg: string); dynamic;
  public
    constructor Create(AOwner: TComponent); override;
    {* 类构造器 }
    destructor Destroy; override;
    {* 类析构器 }
    property ImageIndex: Integer read FImageIndex write SetImageIndex;
    {* 当前活动图像索引号 }
    property Picture: TISeeBitmap read FPicture write SetPicture;
    {* ISee图像 }
  published
    property Active: Boolean read FActive write SetActive default True;
    {* 允许动态图像活动显示 }
    property Align;
    {* 同TImage }
    property Anchors;
    {* 同TImage }
    property AutoSize;
    {* 同TImage }
    property Color default clBlack;
    {* 同TImage }
    property Constraints;
    {* 同TImage }
    property DragCursor;
    {* 同TImage }
    property DragKind;
    {* 同TImage }
    property DragMode;
    {* 同TImage }
    property Enabled;
    {* 同TImage }
    property FrameColor: TColor read FFrameColor write SetFrameColor default 
      clBlue;
    {* 图像边框颜色 }
    property IncrementalDisplay: Boolean read FIncrementalDisplay write 
      FIncrementalDisplay default False;
    {* 同TImage }
    property OnClick;
    {* 同TImage }
    property OnContextPopup;
    {* 同TImage }
    property OnDblClick;
    {* 同TImage }
    property OnDragDrop;
    {* 同TImage }
    property OnDragOver;
    {* 同TImage }
    property OnEndDock;
    {* 同TImage }
    property OnEndDrag;
    {* 同TImage }
    property OnMouseDown;
    {* 同TImage }
    property OnMouseMove;
    {* 同TImage }
    property OnMouseUp;
    {* 同TImage }
    property OnProgress: TProgressEvent read FOnProgress write FOnProgress;
    {* 同TImage }
    property OnStartDock;
    {* 同TImage }
    property OnStartDrag;
    {* 同TImage }
    property ParentShowHint;
    {* 同TImage }
    property PopupMenu;
    {* 同TImage }
    property ShowHint;
    {* 同TImage }
    property Style: TISeeImageStyles read FStyle write SetStyle default [];
    {* 图像显示风格 }
    property Transparent: Boolean read FTransparent write SetTransparent 
      default False;
    {* 图像透明 }
    property Visible;
    {* 同TImage }
  end;
  
  TISeeOpenDialog = class (TOpenDialog)
  {* 与ISee图像读写插件配套使用的打开图像文件对话框组件 }
  private
    FImage: TISeeImage;
    FPaintPanel: TPanel;
    FPictureLabel: TLabel;
    FPicturePanel: TPanel;
  protected
    procedure DoClose; override;
    procedure DoSelectionChange; override;
    procedure DoShow; override;
  public
    constructor Create(AOwner: TComponent); override;
    {* 类构造器 }
    destructor Destroy; override;
    {* 类析构器 }
    function Execute: Boolean; override;
    {* 显示对话框 }
  published
    property Filter stored False;
    {* 扩展名过滤字符串属性，重申明为不保存到DFM中 }
    property Options;
    {* 重载对话框设置，去掉ReSize属性 }
  end;
  
  TISeeSaveDialog = class (TSaveDialog)
  {* 与ISee图像读写插件配套使用的保存图像文件对话框组件 }
  private
    FComboBoxs: array of HWND;
    FCurCount: Integer;
    FCurItem: TISeePluginItem;
    FLabels: array of HWND;
    procedure FreeCtrls;
    procedure UpdateCtrls(SaveDesc: TISeeSaveDescList);
  protected
    function DoCanClose: Boolean; override;
    procedure DoShow; override;
    procedure DoTypeChange; override;
  public
    constructor Create(AOwner: TComponent); override;
    {* 类构造器 }
    destructor Destroy; override;
    {* 类析构器 }
    function Execute: Boolean; override;
    {* 显示对话框事件 }
  published
    property Filter stored False;
    {* 扩展名过滤字符串属性，重申明为不保存到DFM中 }
    property Options;
    {* 重载对话框设置，去掉ReSize属性 }
  end;
  
  TISeeMgr = class (TComponent)
  {* ISee图像读写插件管理组件 }
  private
    FISeePlugins: TISeePlugins;
    function GetAutoRefresh: Boolean;
    procedure SetAutoRefresh(Value: Boolean);
    function GetCount: Integer;
    function GetItems(Index: Integer): TISeePluginItem;
    procedure SetItems(Index: Integer; const Value: TISeePluginItem);
    function GetPluginExtName: string;
    procedure SetPluginExtName(const Value: string);
    function GetPluginPath: TStrings;
    procedure SetPluginPath(Value: TStrings);
  protected
    procedure Loaded; override;
  public
    constructor Create(AOwner: TComponent); override;
    {* 类构造器 }
    function LoadFromFile(const FileName: string; Bmp: TISeeBitmap): 
      TISeeExcuteResult;
    {* 从文件中读取图像 }
    function LoadFromStream(Stream: TStream; Bmp: TISeeBitmap): 
      TISeeExcuteResult;
    {* 从流中读取图像 }
    procedure Refresh;
    {* 刷新插件列表 }
    function SaveToFile(const FileName: string; Bmp: TISeeBitmap): 
      TISeeExcuteResult;
    {* 保存图像到文件 }
    function SaveToStream(Stream: TStream; Bmp: TISeeBitmap): TISeeExcuteResult;
    {* 保存图像到流 }
    property Count: Integer read GetCount;
    {* 插件总数 }
    property Items[Index: Integer]: TISeePluginItem read GetItems write 
      SetItems; default;
    {* 插件列表项 }
  published
    property AutoRefresh: Boolean read GetAutoRefresh write SetAutoRefresh 
      default False;
    {* 更新插件路径时是否自动刷新插件列表 }
    property PluginExtName: string read GetPluginExtName write SetPluginExtName;
    {* 默认插件扩展名 }
    property PluginPath: TStrings read GetPluginPath write SetPluginPath;
    {* 插件路径 }
  end;
  
procedure Register;

implementation

uses Forms;

const
  ID_USERCOMBBOX = 1001;
  ID_USERLABLE = 1101;

procedure Register;
begin
  RegisterComponents('ISee', [TISeeImage]);
  RegisterComponents('ISee', [TISeeOpenDialog]);
  RegisterComponents('ISee', [TISeeSaveDialog]);
  RegisterComponents('ISee', [TISeeMgr]);
end;

{
********************************** TISeeImage **********************************
}
{* 类构造器 }
constructor TISeeImage.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  ControlStyle := ControlStyle + [csReplicatable, csOpaque];
  FTransparent := False;
  FTimer := TTimer.Create(Self);
  FTimer.Enabled := False;
  FTimer.OnTimer := OnTimer;
  FPicture := TISeeBitmap.Create;
  FPicture.OnChange := PictureChanged;
  FPicture.OnProgress := Progress;
  FFrameColor := clBlue;
  FActive := True;
  Color := clBlack;
  Height := 105;
  Width := 105;
end;

{* 类析构器 }
destructor TISeeImage.Destroy;
begin
  FPicture.Free;
  FTimer.Free;
  FOrders := nil;
  inherited Destroy;
end;

{* 自动检测控件大小 }
function TISeeImage.CanAutoSize(var NewWidth, NewHeight: Integer): Boolean;
begin
  Result := True;
  if not (csDesigning in ComponentState) or (Picture.Width > 0) and
    (Picture.Height > 0) then
  begin
    if Align in [alNone, alLeft, alRight] then
      NewWidth := Picture.Width;
    if Align in [alNone, alTop, alBottom] then
      NewHeight := Picture.Height;
    if isColorFrame in FStyle then
    begin
      Inc(NewWidth, 2);
      Inc(NewHeight, 2);
    end;
  end;
end;

{* 计算图像显示大小 }
function TISeeImage.DestRect: TRect;
var
  Rw, Rh: Double;
  W, H, W1, H1: Integer;
begin
  Result := ClientRect;
  if not FPicture.Empty then
  begin
    if isColorFrame in FStyle then
    begin
      Inc(Result.Left);
      Inc(Result.Top);
      Dec(Result.Right);
      Dec(Result.Bottom);
    end;
    W := Result.Right - Result.Left;
    H := Result.Bottom - Result.Top;
    if (((FPicture.Height > H) or (FPicture.Width > W)) and (isAutoZoomOut in
      FStyle)) or ((FPicture.Height < H) and (FPicture.Width < W) and
      (isAutoZoomIn in FStyle)) then
    begin
      Rh := H / FPicture.Height;
      Rw := W / FPicture.Width;
      if isConstrainRate in FStyle then
        if Rh > Rw then Rh := Rw else Rw := Rh;
    end
    else
    begin
      Rh := 1;
      Rw := 1;
    end;
    H1 := Trunc(FPicture.Height * Rh);
    W1 := Trunc(FPicture.Width * Rw);
    if isCenter in FStyle then
    begin
      Inc(Result.Left, (W - W1) div 2);
      Inc(Result.Top, (H - H1) div 2);
    end;
    Result.Right := Result.Left + W1;
    Result.Bottom := Result.Top + H1;
  end;
end;

{* 调色板已变更 }
function TISeeImage.DoPaletteChange: Boolean;
var
  ParentForm: TCustomForm;
begin
  Result := False;
  if Visible and (not (csLoading in ComponentState)) and (FPicture <> nil) and
    (FPicture.PaletteModified) then
  begin
    if (FPicture.Palette = 0) then
      FPicture.PaletteModified := False
    else
    begin
      ParentForm := GetParentForm(Self);
      if Assigned(ParentForm) and ParentForm.Active and Parentform.HandleAllocated then
      begin
        if FDrawing then
          ParentForm.Perform(wm_QueryNewPalette, 0, 0)
        else
          PostMessage(ParentForm.Handle, wm_QueryNewPalette, 0, 0);
        Result := True;
        FPicture.PaletteModified := False;
      end;
    end;
  end;
end;

{* 取调色板 }
function TISeeImage.GetPalette: HPALETTE;
begin
  Result := 0;
  if not FPicture.Empty then
    Result := FPicture.Palette;
end;

{* 定时事件 }
procedure TISeeImage.OnTimer(Sender: TObject);
begin
  if FOrders = nil then
  begin
    FTimer.Enabled := False;
    Exit;
  end;
  Inc(FCurIndex);
  if FCurIndex > High(FOrders) then
    FCurIndex := Low(FOrders);
  ImageIndex := FOrders[FCurIndex];
  FTimer.Enabled := Active;
end;

{* 绘制控件画布 }
procedure TISeeImage.Paint;
var
  Save: Boolean;
  Bmp: TBitmap;
  
  procedure DrawCanvas(ACanvas: TCanvas);
  var
    R: TRect;
  begin
    if csDesigning in ComponentState then
    begin
      ACanvas.Pen.Style := psDash;
      ACanvas.Brush.Style := bsClear;
      ACanvas.Rectangle(0, 0, Width, Height);
    end;
    R := DestRect;
    if (FOrders = nil) or (FImageIndex = 0) then
      ACanvas.StretchDraw(R, FPicture)
    else
      with FPicture.SubImage[FImageIndex - 1] do
        ACanvas.StretchDraw(Bounds(R.Left + Left, R.Top + Top,
          (R.Right - R.Left) * Image.Width div FPicture.Width,
          (R.Bottom - R.Top) * Image.Height div FPicture.Height), Image);
    if isColorFrame in FStyle then
    begin
      ACanvas.Brush.Color := FFrameColor;
      ACanvas.Brush.Style := bsSolid;
      ACanvas.FrameRect(Rect(R.Left - 1, R.Top - 1, R.Right + 1, R.Bottom + 1));
    end;
  end;
  
begin
  Save := FDrawing;
  FDrawing := True;
  try
    if Transparent then
      DrawCanvas(Canvas)
    else
    begin
      Bmp := TBitmap.Create;
      try
        Bmp.Width := Width;
        Bmp.Height := Height;
        Bmp.PixelFormat := pf24bit;
        Bmp.Canvas.Brush.Color := Color;
        Bmp.Canvas.Brush.Style := bsSolid;
        Bmp.Canvas.FillRect(ClientRect);
        DrawCanvas(Bmp.Canvas);
        Canvas.Draw(0, 0, Bmp);
      finally
        Bmp.Free;
      end;
    end;
  finally
    FDrawing := Save;
  end;
end;

{* 图像内容已变更 }
procedure TISeeImage.PictureChanged(Sender: TObject);
begin
  if AutoSize and (Picture.Width > 0) and (Picture.Height > 0) then
    SetBounds(Left, Top, Picture.Width, Picture.Height);
  if not FPicture.Empty then
  begin
    FPicture.Transparent := FTransparent;
    if DoPaletteChange and FDrawing then Update;
  end;
  UpdateOrders;
end;

{* 图像更新过程 }
procedure TISeeImage.Progress(Sender: TObject; Stage: TProgressStage; 
  PercentDone: Byte; RedrawNow: Boolean; const R: TRect; const Msg: string);
begin
  if FIncrementalDisplay and RedrawNow then
  begin
    if DoPaletteChange then Update
    else Paint;
  end;
  if Assigned(FOnProgress) then FOnProgress(Sender, Stage, PercentDone, RedrawNow, R, Msg);
end;

{* 更新动态图像播放序列 }
procedure TISeeImage.UpdateOrders;
var
  i: Integer;
begin
  FTimer.Enabled := False;
  if (FPicture.SubImage.Count = 0) or (FPicture.Time <= 0) then
  begin
    FOrders := nil;
    FCurIndex := 0;
    ImageIndex := 0;
  end
  else
  begin
    if FPicture.PlayOrder.Count > 0 then
    begin
      SetLength(FOrders, FPicture.PlayOrder.Count);
      for i := 0 to FPicture.PlayOrder.Count - 1 do
      begin
        if (FPicture.PlayOrder[i] < 0) or (FPicture.PlayOrder[i] >
          FPicture.SubImage.Count) then // 引用了不非常的图像号
        begin
          FOrders := nil;
          Exit;
        end;
        FOrders[i] := FPicture.PlayOrder[i];
      end;
    end
    else
    begin
      SetLength(FOrders, FPicture.SubImage.Count + 1);
      for i := 0 to FPicture.SubImage.Count do
        FOrders[i] := i;
    end;
    FCurIndex := 0;
    ImageIndex := FOrders[FCurIndex];
    FTimer.Enabled := FActive;
  end;
end;

{* Active属性写方法 }
procedure TISeeImage.SetActive(const Value: Boolean);
begin
  if FActive <> Value then
  begin
    FActive := Value;
    FTimer.Enabled := FActive;
  end;
end;

{* FrameColor属性写方法 }
procedure TISeeImage.SetFrameColor(const Value: TColor);
begin
  if FFrameColor <> Value then
  begin
    FFrameColor := Value;
    PictureChanged(Self);
  end;
end;

{* ImageIndex写方法 }
procedure TISeeImage.SetImageIndex(Value: Integer);
begin
  if (Value < 0) or (Value > FPicture.SubImage.Count) then Exit;
  FImageIndex := Value;
  if FImageIndex = 0 then
    FTimer.Interval := FPicture.Time
  else
    FTimer.Interval := FPicture.SubImage[FImageIndex - 1].Time;
  if not FDrawing then Invalidate;
end;

{* Picture属性写方法 }
procedure TISeeImage.SetPicture(Value: TISeeBitmap);
begin
  FPicture.Assign(Value);
end;

{* Style属性写方法 }
procedure TISeeImage.SetStyle(const Value: TISeeImageStyles);
begin
  if FStyle <> Value then
  begin
    FStyle := Value;
    PictureChanged(Self);
  end;
end;

{* Transparent属性写方法 }
procedure TISeeImage.SetTransparent(Value: Boolean);
begin
  if Value <> FTransparent then
  begin
    FTransparent := Value;
    if not FTransparent then
      ControlStyle := ControlStyle + [csOpaque]
    else
      ControlStyle := ControlStyle - [csOpaque];
    PictureChanged(Self);
  end;
end;

{
******************************* TISeeOpenDialog ********************************
}
{* 类构造器 }
constructor TISeeOpenDialog.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  Options := [ofHideReadOnly];
  FPicturePanel := TPanel.Create(Self);
  with FPicturePanel do
  begin
    Name := 'PicturePanel';
    Caption := '';
    SetBounds(204, 5, 169, 200);
    BevelOuter := bvNone;
    BorderWidth := 6;
    TabOrder := 1;
    FPictureLabel := TLabel.Create(Self);
    with FPictureLabel do
    begin
      Name := 'PictureLabel';
      Caption := '';
      SetBounds(6, 6, 157, 20);
      Align := alTop;
      AutoSize := False;
      Parent := FPicturePanel;
    end;
    FPaintPanel := TPanel.Create(Self);
    with FPaintPanel do
    begin
      Name := 'PaintPanel';
      Caption := '';
      SetBounds(6, 29, 157, 135);
      Align := alClient;
      BevelInner := bvNone;
      BevelOuter := bvLowered;
      TabOrder := 0;
      FImage := TISeeImage.Create(Self);
      Parent := FPicturePanel;
      with FImage do
      begin
        Name := 'Image';
        SetBounds(0, 0, 153, 141);
        Style := [isAutoZoomOut, isConstrainRate, isColorFrame, isCenter];
        Active := True;
        Transparent := False;
        Color := clBtnFace;
        Align := alClient;
        Parent := FPaintPanel;
      end;
    end;
  end;
end;

{* 类析构器 }
destructor TISeeOpenDialog.Destroy;
begin
  FImage.Free;
  FPaintPanel.Free;
  FPictureLabel.Free;
  FPicturePanel.Free;
  inherited Destroy;
end;

{* 关闭对话框 }
procedure TISeeOpenDialog.DoClose;
begin
  inherited DoClose;
  { Hide any hint windows left behind }
  Application.HideHint;
end;

{* 选择对象变更时更新预览图像 }
procedure TISeeOpenDialog.DoSelectionChange;
begin
  if FileExists(FileName) then
  begin
    FImage.Picture.LoadFromFile(FileName);
    if not FImage.Picture.Empty then
    begin
      with FImage.Picture do
        FPictureLabel.Caption := Format('尺寸：%d×%d×%dbit',
          [Width, Height, BitCount]);
      Exit;
    end;
  end;
  FPictureLabel.Caption := '尺寸：(无)';
  inherited DoSelectionChange;
end;

{* 对话框显示前，设置用户控件和事件 }
procedure TISeeOpenDialog.DoShow;
var
  PreviewRect, StaticRect: TRect;
begin
  { Set preview area to entire dialog }
  GetClientRect(Handle, PreviewRect);
  StaticRect := GetStaticRect;
  { Move preview area to right of static area }
  PreviewRect.Left := StaticRect.Left + (StaticRect.Right - StaticRect.Left);
  Inc(PreviewRect.Top, 4);
  FPicturePanel.Font.Handle := SendMessage(GetParent(Handle), WM_GETFONT, 0, 0);
  FPicturePanel.BoundsRect := PreviewRect;
  FImage.Picture.Assign(nil);
  FPicturePanel.ParentWindow := Handle;
  inherited DoShow;
end;

{* 显示对话框 }
function TISeeOpenDialog.Execute: Boolean;
begin
  Filter := ISeePlugins.BuildLoadFilter;
  if NewStyleControls and not (ofOldStyleDialog in Options) then
    Template := 'DLGTEMPLATE' else
    Template := nil;
  Result := inherited Execute;
  if Result then InitialDir := ExtractFileDir(FileName);
end;

{
******************************* TISeeSaveDialog ********************************
}
{* 类构造器 }
constructor TISeeSaveDialog.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  Options := [ofHideReadOnly];
end;

{* 类析构器 }
destructor TISeeSaveDialog.Destroy;
begin
  FreeCtrls;
  inherited Destroy;
end;

{* 关闭对话框前设置插件保存参数 }
function TISeeSaveDialog.DoCanClose: Boolean;
var
  i: Integer;
  idx: Integer;
begin
  if (FCurItem <> nil) and (FCurItem.SaveDesc.Count > 0) then
  begin
    for i := 0 to FCurItem.SaveDesc.Count - 1 do
    begin
      idx := SendMessage(FComboBoxs[i], CB_GETCURSEL, 0, 0);
      if (idx >= 0) and (idx < FCurItem.SaveDesc[i].Count) then
        FCurItem.SaveDesc[i].Value := FCurItem.SaveDesc[i].Values[idx];
    end;
  end;
  Result := inherited DoCanClose;
end;

{* 显示对话框事件 }
procedure TISeeSaveDialog.DoShow;
begin
  inherited DoShow;
  FCurCount := 0;
  DoTypeChange;
end;

{* 当前选择的扩展名变更事件 }
procedure TISeeSaveDialog.DoTypeChange;
var
  s, Ext: string;
  i: Integer;
  
  function DelStr(var Str: string; c: Char): Boolean;
  var
    idx: Integer;
  begin
    idx := Pos(c, Str);
    Result := idx > 0;
    if Result then
      Delete(Str, 1, idx)
    else
      Exit;
  end;
  
begin
  inherited;
  if (FilterIndex > 0) and NewStyleControls and not (ofOldStyleDialog in Options) then
  begin
    s := Filter;
    for i := 1 to FilterIndex - 1 do
    begin
      if not DelStr(s, '|') then Exit;
      if not DelStr(s, '|') then Exit;
    end;
    if not DelStr(s, '|') then Exit;
    if not DelStr(s, '.') then Exit;
    i := Pos('|', s);
    if i > 0 then s := Copy(s, 1, i - 1);
    i := Pos(';', s);
    if i > 0 then s := Copy(s, 1, i - 1);
    Ext := s;
    DefaultExt := Ext;
    FCurItem := ISeePlugins.FindPlugin(Ext, isWrite); // 查找当前扩展名插件
    if FCurItem <> nil then
      UpdateCtrls(FCurItem.SaveDesc);
  end;
end;

{* 显示对话框事件 }
function TISeeSaveDialog.Execute: Boolean;
begin
  Filter := ISeePlugins.BuildSaveFilter;
  if FileName <> '' then
    FileName := Copy(FileName, 1, Pos(ExtractFileExt(FileName), FileName) - 1);
  Result := inherited Execute;
  if Result then
  begin
    if (FileName <> '') and (ExtractFileExt(FileName) = '') then
      FileName := FileName + '.' + DefaultExt;
    InitialDir := ExtractFileDir(FileName);
  end;
end;

{* 释放动态控件 }
procedure TISeeSaveDialog.FreeCtrls;
var
  i: Integer;
begin
  if FLabels <> nil then
  begin
    for i := Low(FLabels) to High(FLabels) do
      DestroyWindow(FLabels[i]);
    FLabels := nil;
  end;
  if FComboBoxs <> nil then
  begin
    for i := Low(FComboBoxs) to High(FComboBoxs) do
      DestroyWindow(FComboBoxs[i]);
    FComboBoxs := nil;
  end;
end;

{* 根据当前插件的保存参数描述，动态创建控件并显示 }
procedure TISeeSaveDialog.UpdateCtrls(SaveDesc: TISeeSaveDescList);
var
  i: Integer;
  j: Integer;
  H: HWND;
  Rects: array[0..4] of TRect;
  ExtraSize: Integer;
  Rect: TRect;
  s: string;
  idx: Integer;
  
  procedure GetRects;
  const
    Ctrls: array[0..4] of Cardinal =
      (stc3, stc2, // 对话框中静态标签的ItemID
      edt1, cmb1, lst1); // 编辑框、下拉框、文件列表的ItemID
  var
    i: Integer;
  begin
    for i := Low(Rects) to High(Rects) do
    begin
      GetWindowRect(GetDlgItem(H, Ctrls[i]), Rects[i]);
      ScreenToClient(H, Rects[i].TopLeft); // 取上述控件的位置
      ScreenToClient(H, Rects[i].BottomRight);
    end;
  end;
  
  procedure SetFont(AHandle: HWND);
  var
    Font: HFont;
  begin
    Font := SendMessage(H, WM_GETFONT, 0, 0);
    SendMessage(AHandle, WM_SETFONT, Font, Cardinal(False));
  end;
  
begin
  FreeCtrls;
  H := GetParent(Handle); // 取对话框真实句柄
  GetRects; // 取对话框中已有的子控件位置
  ExtraSize := Rects[1].Bottom - Rects[0].Bottom; // 新控件扩展高度
  GetWindowRect(H, Rect);
  SetWindowPos(H, 0, 0, 0, Rect.Right - Rect.Left, Rect.Bottom - Rect.Top +
    ExtraSize * (SaveDesc.Count - FCurCount), SWP_NOMOVE); // 重设对话框高度
  FCurCount := SaveDesc.Count;
  if SaveDesc.Count > 0 then
  begin
    SetLength(FLabels, SaveDesc.Count); // 分配动态数组
    SetLength(FComboBoxs, SaveDesc.Count);
    for i := Low(FLabels) to High(FLabels) do
    begin
      Rect := Rects[1];
      Inc(Rect.Top, (i + 1) * ExtraSize);
      Inc(Rect.Bottom, (i + 1) * ExtraSize); // 新标签控件位置
      s := SaveDesc[i].DescStr + ':';
      FLabels[i] := CreateWindow('STATIC', '', WS_CHILD or WS_VISIBLE, Rect.Left,
        Rect.Top, Rect.Right - Rect.Left, Rect.Bottom - Rect.Top, H,
        ID_USERLABLE + i, HInstance, nil); // 创建标签
      SetFont(FLabels[i]); // 设置字体
      SetWindowText(FLabels[i], PChar(s)); // 设置文本
      Rect := Rects[3];
      Inc(Rect.Top, (i + 1) * ExtraSize);
      Inc(Rect.Bottom, (i + 1) * ExtraSize); // 新下拉框控件位置
      FComboBoxs[i] := CreateWindow('COMBOBOX', '', WS_CHILD or WS_VISIBLE or
        WS_VSCROLL or CBS_DROPDOWN or WS_TABSTOP or CBS_AUTOHSCROLL or
        CBS_DROPDOWNLIST or CBS_HASSTRINGS, Rect.Left, Rect.Top,
        Rect.Right - Rect.Left, Rect.Bottom - Rect.Top + 106, H, ID_USERCOMBBOX + i,
        HInstance, nil); // 创建下拉框（窗口高度加上8项列表的高度，否则无法显示下拉项）
      SetFont(FComboBoxs[i]); // 设置字体
      for j := 0 to SaveDesc[i].Count - 1 do // 增加下拉框列表
        SendMessage(FComboBoxs[i], CB_ADDSTRING, 0, Cardinal(PChar(SaveDesc[i].Names[j])));
      idx := 0;
      for j := 0 to SaveDesc[i].Count - 1 do
        if SaveDesc[i].Values[i] = SaveDesc[i].Value then
        begin
          idx := j; // 查找默认值对齐的序号
          Break;
        end;
      SendMessage(FComboBoxs[i], CB_SETCURSEL, idx, 0);
    end;
  end;
end;

{
*********************************** TISeeMgr ***********************************
}
{* 类构造器 }
constructor TISeeMgr.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  FISeePlugins := ISeePlugins;
end;

{* 运行时属性已装载 }
procedure TISeeMgr.Loaded;
begin
  inherited Loaded;
  if not (csDesigning in ComponentState) and AutoRefresh and
    (Count = 0) then
    Refresh;
end;

{* 从文件中读取图像 }
function TISeeMgr.LoadFromFile(const FileName: string; Bmp: TISeeBitmap): 
  TISeeExcuteResult;
begin
  Result := FISeePlugins.LoadFromFile(FileName, Bmp);
end;

{* 从流中读取图像 }
function TISeeMgr.LoadFromStream(Stream: TStream; Bmp: TISeeBitmap): 
  TISeeExcuteResult;
begin
  Result := FISeePlugins.LoadFromStream(Stream, Bmp);
end;

{* 刷新插件列表 }
procedure TISeeMgr.Refresh;
begin
  FISeePlugins.Refresh;
end;

{* 保存图像到文件 }
function TISeeMgr.SaveToFile(const FileName: string; Bmp: TISeeBitmap): 
  TISeeExcuteResult;
begin
  Result := FISeePlugins.SaveToFile(FileName, Bmp);
end;

{* 保存图像到流 }
function TISeeMgr.SaveToStream(Stream: TStream; Bmp: TISeeBitmap): 
  TISeeExcuteResult;
begin
  Result := FISeePlugins.SaveToStream(Stream, Bmp);
end;

{* AutoRefresh的读方法 }
function TISeeMgr.GetAutoRefresh: Boolean;
begin
  Result := FISeePlugins.AutoRefresh;
end;

{* AutoRefresh的写方法 }
procedure TISeeMgr.SetAutoRefresh(Value: Boolean);
begin
  FISeePlugins.AutoRefresh := Value;
end;

{* Count的读方法 }
function TISeeMgr.GetCount: Integer;
begin
  Result := FISeePlugins.Count;
end;

{* Items的读方法 }
function TISeeMgr.GetItems(Index: Integer): TISeePluginItem;
begin
  Result := FISeePlugins.Items[Index];
end;

{* Items的写方法 }
procedure TISeeMgr.SetItems(Index: Integer; const Value: TISeePluginItem);
begin
  FISeePlugins.Items[Index] := Value;
end;

{* PluginExtName的读方法 }
function TISeeMgr.GetPluginExtName: string;
begin
  Result := FISeePlugins.PluginExtName;
end;

{* PluginExtName的写方法 }
procedure TISeeMgr.SetPluginExtName(const Value: string);
begin
  FISeePlugins.PluginExtName := Value;
end;

{* PluginPath的读方法 }
function TISeeMgr.GetPluginPath: TStrings;
begin
  Result := FISeePlugins.PluginPath;
end;

{* PluginPath的写方法 }
procedure TISeeMgr.SetPluginPath(Value: TStrings);
begin
  FISeePlugins.PluginPath := Value;
end;

end.

