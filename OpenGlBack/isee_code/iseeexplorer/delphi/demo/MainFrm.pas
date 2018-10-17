{*******************************************************************************

        MainFrm.pas

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
        本文件用途：    ISee 图像读写插件 Delphi 接口测试程序主窗体单元
        本文件名：      MainFrm.pas
        本文件编写人：  与月共舞                yygw##163.com

        本文件版本：    20620
        最后修改于：    2002-06-20

        注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
        地址收集软件。
        ----------------------------------------------------------------
        本文件修正历史：
                2002-06-20      创建单元

*******************************************************************************}

unit MainFrm;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ComCtrls, StdCtrls, Mask, ToolEdit, Placemnt, Buttons, ExtCtrls, FileCtrl,
  ISee, Spin, Menus, ISeeCtrls, ShellAPI, Math;

type
  TMainForm = class(TForm)
    PageControl: TPageControl;
    tsPlugins: TTabSheet;
    tsImage: TTabSheet;
    GroupBox1: TGroupBox;
    GroupBox2: TGroupBox;
    edtDir: TDirectoryEdit;
    btnAddPath: TButton;
    lbPath: TListBox;
    btnDelPath: TButton;
    btnClearPath: TButton;
    GroupBox3: TGroupBox;
    lbPlugins: TListBox;
    memInfo: TMemo;
    rbDLLName: TRadioButton;
    rbPluginName: TRadioButton;
    FormStorage: TFormStorage;
    rbFullName: TRadioButton;
    lblCount: TLabel;
    btnUpdate: TBitBtn;
    Panel1: TPanel;
    cbbDrive: TDriveComboBox;
    lbDir: TDirectoryListBox;
    lbFile: TFileListBox;
    cbIn: TCheckBox;
    cbOut: TCheckBox;
    cbRate: TCheckBox;
    cbAutoScroll: TCheckBox;
    seAuto: TSpinEdit;
    Label1: TLabel;
    tmrAutoScroll: TTimer;
    lblSize: TLabel;
    lblSubImage: TLabel;
    lblPlayOrder: TLabel;
    pmFileList: TPopupMenu;
    pmiSaveAs: TMenuItem;
    lblLoadTime: TLabel;
    Image: TISeeImage;
    ISeeOpenDialog: TISeeOpenDialog;
    ISeeSaveDialog: TISeeSaveDialog;
    ISeeMgr: TISeeMgr;
    pmiCopy: TMenuItem;
    pmiOpen: TMenuItem;
    N1: TMenuItem;
    pmiMove: TMenuItem;
    cbActive: TCheckBox;
    tsSubImage: TTabSheet;
    Panel2: TPanel;
    imgSub: TISeeImage;
    btnOpen: TButton;
    btnSave: TButton;
    lbSubImage: TListBox;
    lblSize1: TLabel;
    lblLoadTime1: TLabel;
    lblSubImage1: TLabel;
    lblPlayOrder1: TLabel;
    cbActive1: TCheckBox;
    cbIn1: TCheckBox;
    cbOut1: TCheckBox;
    cbRate1: TCheckBox;
    procedure btnAddPathClick(Sender: TObject);
    procedure btnDelPathClick(Sender: TObject);
    procedure btnClearPathClick(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure lbPluginsClick(Sender: TObject);
    procedure rbDLLNameClick(Sender: TObject);
    procedure btnUpdateClick(Sender: TObject);
    procedure lbFileChange(Sender: TObject);
    procedure FormStorageStoredValues0Restore(Sender: TStoredValue;
      var Value: Variant);
    procedure FormStorageStoredValues0Save(Sender: TStoredValue;
      var Value: Variant);
    procedure cbAutoScrollClick(Sender: TObject);
    procedure seAutoChange(Sender: TObject);
    procedure tmrAutoScrollTimer(Sender: TObject);
    procedure cbInClick(Sender: TObject);
    procedure pmiSaveAsClick(Sender: TObject);
    procedure FormStorageStoredValues1Restore(Sender: TStoredValue;
      var Value: Variant);
    procedure FormStorageStoredValues1Save(Sender: TStoredValue;
      var Value: Variant);
    procedure pmiCopyClick(Sender: TObject);
    procedure pmiOpenClick(Sender: TObject);
    procedure pmiMoveClick(Sender: TObject);
    procedure lbSubImageDrawItem(Control: TWinControl; Index: Integer;
      Rect: TRect; State: TOwnerDrawState);
    procedure btnOpenClick(Sender: TObject);
    procedure btnSaveClick(Sender: TObject);
    procedure lbSubImageClick(Sender: TObject);
    procedure cbIn1Click(Sender: TObject);
    procedure pmFileListPopup(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    { Private declarations }
    SavePath: WideString;
    procedure UpdatePath;
    procedure UpdateSub;
  public
    { Public declarations }
    procedure CreateParams(var Params: TCreateParams); override;
  end;

var
  MainForm: TMainForm;

implementation

{$R *.DFM}

procedure TMainForm.FormCreate(Sender: TObject);
begin
  PageControl.ActivePageIndex := 0;
end;

procedure TMainForm.FormShow(Sender: TObject);
begin
  UpdatePath;
  seAutoChange(nil);
  lbFileChange(nil);
  cbAutoScrollClick(nil);
  cbInClick(nil);
  cbIn1Click(nil);
end;

procedure TMainForm.CreateParams(var Params: TCreateParams);
begin
  inherited;
  Params.Style := Params.Style - WS_SIZEBOX;
end;

procedure TMainForm.UpdatePath;
begin
  ISeeMgr.PluginPath := lbPath.Items;
  ISeeMgr.Refresh;
  rbDLLNameClick(nil);
  lbPluginsClick(nil);
  lblCount.Caption := '插件总数：' + IntToStr(ISeeMgr.Count) + '个';
end;

procedure TMainForm.rbDLLNameClick(Sender: TObject);
var
  i: Integer;
begin
  lbPlugins.Clear;
  for i := 0 to ISeeMgr.Count - 1 do
    if rbDLLName.Checked then
      lbPlugins.Items.Add(ExtractFileName(ISeeMgr[i].DLLName))
    else if rbFullName.Checked then
      lbPlugins.Items.Add(ISeeMgr[i].DLLName)
    else
      lbPlugins.Items.Add(ISeeMgr[i].Name);
end;

procedure TMainForm.btnAddPathClick(Sender: TObject);
begin
  if edtDir.Text <> '' then
    lbPath.Items.Add(edtDir.Text);
  UpdatePath;
end;

procedure TMainForm.btnDelPathClick(Sender: TObject);
begin
  if lbPath.ItemIndex >= 0 then
    lbPath.Items.Delete(lbPath.ItemIndex);
  UpdatePath;
end;

procedure TMainForm.btnClearPathClick(Sender: TObject);
begin
  lbPath.Clear;
  UpdatePath;
end;

procedure TMainForm.btnUpdateClick(Sender: TObject);
begin
  UpdatePath;
end;

procedure TMainForm.lbPluginsClick(Sender: TObject);
const
  Supports: array[Boolean] of string = ('无', '有');
  ImgNums: array[TISeeSaveNum] of string = ('无要求', '只允许一张图像',
    '允许多张图像');
var
  Item: TISeePluginItem;
  i, j: Integer;
  s: string;
  function GetEmail(Em: string): string;
  var
    i: Integer;
  begin
    Result := Em;
    i := Pos('##', Result);
    if i > 0 then
    begin
      Delete(Result, i, 2);
      Insert('@', Result, i);
    end;
  end;
begin
  memInfo.Clear;
  if lbPlugins.ItemIndex >= 0 then
  begin
    Item := ISeeMgr[lbPlugins.ItemIndex];
    with memInfo.Lines do
    begin
      Add('插件文件名：' + Item.DLLName);
      Add('插件名称：' + Item.Name);
      Add(Format('插件版本号：%d.%d', [Item.Version div 10, Item.Version mod 10]));
      Add('');
      Add('通用扩展名：' + Item.Ext);
      for i := 0 to Item.Exts.Count - 1 do
        s := s + Item.Exts[i] + ';';
      Add(Format('衍生扩展名：%s(%d项)', [s, Item.Exts.Count]));
      Add('');
      Add('读图像支持：' + Supports[isRead in Item.Style]);
      Add('写图像支持：' + Supports[isWrite in Item.Style]);
      Add('保存图像数：' + ImgNums[Item.SaveDesc.ImgNum]);
      if Item.AuthorInfo.Count > 0 then
      begin
        Add('');
        Add(Format('插件作者：(%d名)', [Item.AuthorInfo.Count]));
        Add('------------------------------------------');
        for i := 0 to Item.AuthorInfo.Count - 1 do
        begin
          Add('  作者：' + GetEmail(Item.AuthorInfo[i].Name));
          Add('  邮件：' + GetEmail(Item.AuthorInfo[i].Email));
          Add('  留言：' + GetEmail(Item.AuthorInfo[i].Comment));
          Add('------------------------------------------');
        end;
      end;
      if Item.SaveDesc.Count > 0 then
      begin
        Add('');
        Add(Format('保存参数描述：(%d项)', [Item.SaveDesc.Count]));
        Add('------------------------------------------');
        for i := 0 to Item.SaveDesc.Count - 1 do
        begin
          Add('  参数描述：' + Item.SaveDesc[i].DescStr);
          for j := 0 to Item.SaveDesc[i].Count - 1 do
            Add(Format('    值：%d; 说明：%s', [Item.SaveDesc[i].Values[j],
              Item.SaveDesc[i].Names[j]]));
          Add('------------------------------------------');
        end;
      end;
    end;
  end;
end;

procedure TMainForm.FormStorageStoredValues0Restore(Sender: TStoredValue;
  var Value: Variant);
begin
  try
    lbFile.FileName := Value;
  except
    ;
  end;
end;

procedure TMainForm.FormStorageStoredValues0Save(Sender: TStoredValue;
  var Value: Variant);
begin
  Value := lbFile.FileName;
end;

procedure TMainForm.FormStorageStoredValues1Restore(Sender: TStoredValue;
  var Value: Variant);
begin
  SavePath := Value;
end;

procedure TMainForm.FormStorageStoredValues1Save(Sender: TStoredValue;
  var Value: Variant);
begin
  Value := SavePath;
end;

procedure TMainForm.lbFileChange(Sender: TObject);
var
  Tick: Cardinal;
begin
  Tick := GetTickCount;
  Image.Picture.LoadFromFile(lbFile.FileName);
  if not Image.Picture.Empty then
    with Image.Picture do
    begin
      Tick := GetTickCount - Tick;
      lblSize.Caption := Format('图像尺寸：%d×%d×%dbit', [Width, Height,
        BitCount]);
      lblLoadTime.Caption := Format('装载时间：%4.2fs', [Tick / 1000]);
      lblSubImage.Caption := Format('子图像数：%d', [SubImage.Count]);
      lblPlayOrder.Caption := Format('播放序列：%d', [PlayOrder.Count]);
    end
  else
  begin
    lblSize.Caption := '图像尺寸：无';
    lblLoadTime.Caption := '装载时间：无';
    lblSubImage.Caption := '子图像数：无';
    lblPlayOrder.Caption := '播放序列：无';
  end;
end;

procedure TMainForm.cbAutoScrollClick(Sender: TObject);
begin
  seAuto.Enabled := cbAutoScroll.Checked;
  tmrAutoScroll.Enabled := cbAutoScroll.Checked;
end;

procedure TMainForm.seAutoChange(Sender: TObject);
begin
  tmrAutoScroll.Interval := seAuto.Value * 1000;
end;

procedure TMainForm.tmrAutoScrollTimer(Sender: TObject);
begin
  if PageControl.ActivePage = tsImage then
  begin
    if lbFile.ItemIndex < lbFile.Items.Count - 1 then
      with Image.Picture do
        repeat
          lbFile.ItemIndex := lbFile.ItemIndex + 1;
          lbFileChange(nil);
        until not Image.Picture.Empty or (lbFile.ItemIndex >=
          lbFile.Items.Count - 1);
  end;
end;

procedure TMainForm.cbInClick(Sender: TObject);
  procedure SetStyle(B: Boolean; Value: TISeeImageStyles);
  begin
    if B then
      Image.Style := Image.Style + Value
    else
      Image.Style := Image.Style - Value;
  end;
begin
  SetStyle(cbOut.Checked, [isAutoZoomOut]);
  SetStyle(cbIn.Checked, [isAutoZoomIn]);
  SetStyle(cbRate.Checked, [isConstrainRate]);
  Image.Active := cbActive.Checked;
end;

procedure TMainForm.pmFileListPopup(Sender: TObject);
begin
  pmiOpen.Enabled := FileExists(lbFile.FileName);
  pmiSaveAs.Enabled := not Image.Picture.Empty;
  pmiCopy.Enabled := FileExists(lbFile.FileName);
  pmiMove.Enabled := FileExists(lbFile.FileName);
end;

procedure TMainForm.pmiOpenClick(Sender: TObject);
begin
  if FileExists(lbFile.FileName) then
    ShellExecute(Handle, nil, PChar(lbFile.FileName), '', '', SW_SHOWNORMAL);
end;

procedure TMainForm.pmiSaveAsClick(Sender: TObject);
begin
  if not Image.Picture.Empty then
  begin
    if ISeeSaveDialog.Execute then
      Image.Picture.SaveToFile(ISeeSaveDialog.FileName);
  end;
end;

procedure TMainForm.pmiCopyClick(Sender: TObject);
var
  s: string;
begin
  if FileExists(lbFile.FileName) then
  begin
    if SelectDirectory('选择目标文件夹', SavePath, s) then
      CopyFile(PChar(lbFile.FileName), PChar(s + '\' + ExtractFileName(lbFile.FileName)),
        False);
  end;
end;

procedure TMainForm.pmiMoveClick(Sender: TObject);
var
  s: string;
  Save: Integer;
begin
  if FileExists(lbFile.FileName) then
  begin
    if SelectDirectory('选择目标文件夹', SavePath, s) then
    begin
      CopyFile(PChar(lbFile.FileName), PChar(s + '\' + ExtractFileName(lbFile.FileName)),
        False);
      Save := lbFile.ItemIndex;
      DeleteFile(lbFile.FileName);
      lbFile.Update;
      lbFile.ItemIndex := Min(Save, lbFile.Items.Count - 1);
      lbFileChange(nil);
    end;
  end;
end;

procedure TMainForm.btnOpenClick(Sender: TObject);
var
  Tick: Cardinal;
begin
  if ISeeOpenDialog.Execute then
  begin
    Tick := GetTickCount;
    imgSub.Picture.LoadFromFile(ISeeOpenDialog.FileName);
    if not imgSub.Picture.Empty then
      with imgSub.Picture do
      begin
        Tick := GetTickCount - Tick;
        lblSize1.Caption := Format('图像尺寸：%d×%d×%dbit', [Width, Height,
          BitCount]);
        lblLoadTime1.Caption := Format('装载时间：%4.2fs', [Tick / 1000]);
        lblSubImage1.Caption := Format('子图像数：%d', [SubImage.Count]);
        lblPlayOrder1.Caption := Format('播放序列：%d', [PlayOrder.Count]);
      end
    else
    begin
      lblSize1.Caption := '图像尺寸：无';
      lblLoadTime1.Caption := '装载时间：无';
      lblSubImage1.Caption := '子图像数：无';
      lblPlayOrder1.Caption := '播放序列：无';
    end;
    UpdateSub;
  end;
end;

procedure TMainForm.btnSaveClick(Sender: TObject);
begin
  if not imgSub.Picture.Empty and ISeeSaveDialog.Execute then
    imgSub.Picture.SaveToFile(ISeeSaveDialog.FileName);
end;

procedure TMainForm.UpdateSub;
var
  i: Integer;
begin
  lbSubImage.Clear;
  lbSubImage.Items.AddObject('主图像', imgSub.Picture);
  for i := 0 to imgSub.Picture.SubImage.Count - 1 do
    lbSubImage.Items.AddObject(Format('子图像%d', [i]),
      imgSub.Picture.SubImage[i].Image);
end;

procedure TMainForm.lbSubImageDrawItem(Control: TWinControl;
  Index: Integer; Rect: TRect; State: TOwnerDrawState);
const
  csBorder = 4;
var
  R: TRect;
  S: string;
  W, H: Integer;
  function GetRectOut(ARect: TRect; Border: Integer): TRect;
  begin
    Result := Classes.Rect(R.Left + Border, R.Top + Border,
      R.Right - Border, R.Bottom - Border);
  end;
begin
  if lbSubImage.Items.Objects[Index] is TBitmap then
  begin
    with lbSubImage.Canvas do
    begin
      if odSelected in State then
        Brush.Color := clNavy
      else
        Brush.Color := clWhite;
      Brush.Style := bsSolid;
      FillRect(Rect);
      Brush.Color := clBlue;
      FrameRect(Rect);
      if odSelected in State then
        Brush.Color := clNavy
      else
        Brush.Color := clWhite;
    end;
    R := Rect;
    R.Right := (R.Left + R.Right) div 2 - 15;
    R := GetRectOut(R, csBorder);
    W := R.Right - R.Left;
    H := R.Bottom - R.Top;
    with TBitmap(lbSubImage.Items.Objects[Index]) do
      if (Width > W) or (Height > H) then
      begin
        if Width / W > Height / H then
          H := Height * W div Width
        else
          W := Width * H div Height;
      end
      else begin
        H := Height;
        W := Width;
      end;
    R.Left := (R.Left + R.Right - W) div 2;
    R.Right := R.Left + W;
    R.Top := (R.Top + R.Bottom - H) div 2;
    R.Bottom := R.Top + H;
    lbSubImage.Canvas.StretchDraw(R, TBitmap(lbSubImage.Items.Objects[Index]));

    R := Rect;
    R.Left := (R.Left + R.Right) div 2 - 14;
    if Index = 0 then
      with imgSub.Picture do
        S := Format('%s'#13#10'左:%d 上:%d'#13#10'延时:%dms',
          [lbSubImage.Items[Index], 0, 0, Time])
    else
      with imgSub.Picture.SubImage[Index - 1] do
        S := Format('%s'#13#10'左:%d 上:%d'#13#10'延时:%dms',
          [lbSubImage.Items[Index], Left, Top, Time]);
    R := GetRectOut(R, csBorder);
    DrawText(lbSubImage.Canvas.Handle, PChar(S), Length(S), R,
      DT_VCENTER);
  end;
end;

procedure TMainForm.lbSubImageClick(Sender: TObject);
begin
  if not imgSub.Active and (lbSubImage.ItemIndex >= 0) then
    imgSub.ImageIndex := lbSubImage.ItemIndex;
end;

procedure TMainForm.cbIn1Click(Sender: TObject);
  procedure SetStyle(B: Boolean; Value: TISeeImageStyles);
  begin
    if B then
      imgSub.Style := imgSub.Style + Value
    else
      imgSub.Style := imgSub.Style - Value;
  end;
begin
  SetStyle(cbOut1.Checked, [isAutoZoomOut]);
  SetStyle(cbIn1.Checked, [isAutoZoomIn]);
  SetStyle(cbRate1.Checked, [isConstrainRate]);
  imgSub.Active := cbActive1.Checked;
  lbSubImageClick(nil);
end;

end.

