{*******************************************************************************

        MainFrm.pas

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
        ���ļ���;��    ISee ͼ���д��� Delphi �ӿڲ��Գ��������嵥Ԫ
        ���ļ�����      MainFrm.pas
        ���ļ���д�ˣ�  ���¹���                yygw##163.com

        ���ļ��汾��    20620
        ����޸��ڣ�    2002-06-20

        ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
        ��ַ�ռ������
        ----------------------------------------------------------------
        ���ļ�������ʷ��
                2002-06-20      ������Ԫ

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
  lblCount.Caption := '���������' + IntToStr(ISeeMgr.Count) + '��';
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
  Supports: array[Boolean] of string = ('��', '��');
  ImgNums: array[TISeeSaveNum] of string = ('��Ҫ��', 'ֻ����һ��ͼ��',
    '�������ͼ��');
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
      Add('����ļ�����' + Item.DLLName);
      Add('������ƣ�' + Item.Name);
      Add(Format('����汾�ţ�%d.%d', [Item.Version div 10, Item.Version mod 10]));
      Add('');
      Add('ͨ����չ����' + Item.Ext);
      for i := 0 to Item.Exts.Count - 1 do
        s := s + Item.Exts[i] + ';';
      Add(Format('������չ����%s(%d��)', [s, Item.Exts.Count]));
      Add('');
      Add('��ͼ��֧�֣�' + Supports[isRead in Item.Style]);
      Add('дͼ��֧�֣�' + Supports[isWrite in Item.Style]);
      Add('����ͼ������' + ImgNums[Item.SaveDesc.ImgNum]);
      if Item.AuthorInfo.Count > 0 then
      begin
        Add('');
        Add(Format('������ߣ�(%d��)', [Item.AuthorInfo.Count]));
        Add('------------------------------------------');
        for i := 0 to Item.AuthorInfo.Count - 1 do
        begin
          Add('  ���ߣ�' + GetEmail(Item.AuthorInfo[i].Name));
          Add('  �ʼ���' + GetEmail(Item.AuthorInfo[i].Email));
          Add('  ���ԣ�' + GetEmail(Item.AuthorInfo[i].Comment));
          Add('------------------------------------------');
        end;
      end;
      if Item.SaveDesc.Count > 0 then
      begin
        Add('');
        Add(Format('�������������(%d��)', [Item.SaveDesc.Count]));
        Add('------------------------------------------');
        for i := 0 to Item.SaveDesc.Count - 1 do
        begin
          Add('  ����������' + Item.SaveDesc[i].DescStr);
          for j := 0 to Item.SaveDesc[i].Count - 1 do
            Add(Format('    ֵ��%d; ˵����%s', [Item.SaveDesc[i].Values[j],
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
      lblSize.Caption := Format('ͼ��ߴ磺%d��%d��%dbit', [Width, Height,
        BitCount]);
      lblLoadTime.Caption := Format('װ��ʱ�䣺%4.2fs', [Tick / 1000]);
      lblSubImage.Caption := Format('��ͼ������%d', [SubImage.Count]);
      lblPlayOrder.Caption := Format('�������У�%d', [PlayOrder.Count]);
    end
  else
  begin
    lblSize.Caption := 'ͼ��ߴ磺��';
    lblLoadTime.Caption := 'װ��ʱ�䣺��';
    lblSubImage.Caption := '��ͼ��������';
    lblPlayOrder.Caption := '�������У���';
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
    if SelectDirectory('ѡ��Ŀ���ļ���', SavePath, s) then
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
    if SelectDirectory('ѡ��Ŀ���ļ���', SavePath, s) then
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
        lblSize1.Caption := Format('ͼ��ߴ磺%d��%d��%dbit', [Width, Height,
          BitCount]);
        lblLoadTime1.Caption := Format('װ��ʱ�䣺%4.2fs', [Tick / 1000]);
        lblSubImage1.Caption := Format('��ͼ������%d', [SubImage.Count]);
        lblPlayOrder1.Caption := Format('�������У�%d', [PlayOrder.Count]);
      end
    else
    begin
      lblSize1.Caption := 'ͼ��ߴ磺��';
      lblLoadTime1.Caption := 'װ��ʱ�䣺��';
      lblSubImage1.Caption := '��ͼ��������';
      lblPlayOrder1.Caption := '�������У���';
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
  lbSubImage.Items.AddObject('��ͼ��', imgSub.Picture);
  for i := 0 to imgSub.Picture.SubImage.Count - 1 do
    lbSubImage.Items.AddObject(Format('��ͼ��%d', [i]),
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
        S := Format('%s'#13#10'��:%d ��:%d'#13#10'��ʱ:%dms',
          [lbSubImage.Items[Index], 0, 0, Time])
    else
      with imgSub.Picture.SubImage[Index - 1] do
        S := Format('%s'#13#10'��:%d ��:%d'#13#10'��ʱ:%dms',
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

