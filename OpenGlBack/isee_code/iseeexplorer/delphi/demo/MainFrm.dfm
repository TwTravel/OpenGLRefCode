object MainForm: TMainForm
  Left = 98
  Top = 58
  Width = 659
  Height = 448
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'ISeeͼ���д���Delphi�ӿڲ��Գ���'
  Color = clBtnFace
  Font.Charset = GB2312_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '����'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object PageControl: TPageControl
    Left = 8
    Top = 8
    Width = 633
    Height = 401
    ActivePage = tsPlugins
    TabOrder = 0
    object tsPlugins: TTabSheet
      Caption = '�����Ϣ(&I)'
      object GroupBox1: TGroupBox
        Left = 8
        Top = 8
        Width = 273
        Height = 113
        Caption = '·������(&Q)'
        TabOrder = 0
        object edtDir: TDirectoryEdit
          Left = 8
          Top = 16
          Width = 185
          Height = 21
          DialogKind = dkWin32
          NumGlyphs = 1
          TabOrder = 0
        end
        object btnAddPath: TButton
          Left = 200
          Top = 14
          Width = 59
          Height = 25
          Caption = '����(&A)'
          TabOrder = 1
          OnClick = btnAddPathClick
        end
        object lbPath: TListBox
          Left = 8
          Top = 48
          Width = 185
          Height = 57
          ItemHeight = 12
          TabOrder = 2
        end
        object btnDelPath: TButton
          Left = 200
          Top = 48
          Width = 59
          Height = 25
          Caption = 'ɾ��(&D)'
          TabOrder = 3
          OnClick = btnDelPathClick
        end
        object btnClearPath: TButton
          Left = 200
          Top = 80
          Width = 59
          Height = 25
          Caption = '���(&C)'
          TabOrder = 4
          OnClick = btnClearPathClick
        end
      end
      object GroupBox2: TGroupBox
        Left = 288
        Top = 8
        Width = 329
        Height = 353
        Caption = '�����Ϣ(&F)'
        TabOrder = 1
        object memInfo: TMemo
          Left = 8
          Top = 16
          Width = 313
          Height = 329
          ReadOnly = True
          ScrollBars = ssBoth
          TabOrder = 0
        end
      end
      object GroupBox3: TGroupBox
        Left = 8
        Top = 128
        Width = 273
        Height = 233
        Caption = '����б�(&L)'
        TabOrder = 2
        object lblCount: TLabel
          Left = 9
          Top = 204
          Width = 60
          Height = 12
          Caption = '���������'
        end
        object lbPlugins: TListBox
          Left = 8
          Top = 40
          Width = 257
          Height = 153
          ItemHeight = 12
          TabOrder = 3
          OnClick = lbPluginsClick
        end
        object rbDLLName: TRadioButton
          Left = 8
          Top = 16
          Width = 73
          Height = 17
          Caption = '�ļ���(&N)'
          Checked = True
          TabOrder = 0
          TabStop = True
          OnClick = rbDLLNameClick
        end
        object rbPluginName: TRadioButton
          Left = 192
          Top = 16
          Width = 73
          Height = 17
          Caption = '�����(&P)'
          TabOrder = 2
          OnClick = rbDLLNameClick
        end
        object rbFullName: TRadioButton
          Left = 88
          Top = 16
          Width = 97
          Height = 17
          Caption = '·���ļ���(&E)'
          TabOrder = 1
          OnClick = rbDLLNameClick
        end
        object btnUpdate: TBitBtn
          Left = 208
          Top = 200
          Width = 51
          Height = 25
          Caption = 'ˢ��(&Z)'
          TabOrder = 4
          OnClick = btnUpdateClick
        end
      end
    end
    object tsImage: TTabSheet
      Caption = 'ͼ���ȡ(&V)'
      ImageIndex = 1
      object Label1: TLabel
        Left = 144
        Top = 350
        Width = 12
        Height = 12
        Caption = '��'
      end
      object lblSize: TLabel
        Left = 184
        Top = 349
        Width = 60
        Height = 12
        Caption = 'ͼ��ߴ磺'
      end
      object lblSubImage: TLabel
        Left = 456
        Top = 349
        Width = 60
        Height = 12
        Caption = '��ͼ������'
      end
      object lblPlayOrder: TLabel
        Left = 536
        Top = 349
        Width = 60
        Height = 12
        Caption = '�������У�'
      end
      object lblLoadTime: TLabel
        Left = 352
        Top = 349
        Width = 60
        Height = 12
        Caption = 'װ��ʱ�䣺'
      end
      object Panel1: TPanel
        Left = 184
        Top = 16
        Width = 425
        Height = 321
        BevelOuter = bvLowered
        Color = clBlack
        FullRepaint = False
        TabOrder = 0
        object Image: TISeeImage
          Left = 1
          Top = 1
          Width = 423
          Height = 319
          Align = alClient
          OnDblClick = pmiOpenClick
          PopupMenu = pmFileList
          Style = [isColorFrame, isCenter]
        end
      end
      object cbbDrive: TDriveComboBox
        Left = 8
        Top = 16
        Width = 161
        Height = 18
        DirList = lbDir
        TabOrder = 1
      end
      object lbDir: TDirectoryListBox
        Left = 8
        Top = 40
        Width = 161
        Height = 97
        FileList = lbFile
        ItemHeight = 16
        TabOrder = 2
      end
      object lbFile: TFileListBox
        Left = 8
        Top = 144
        Width = 161
        Height = 121
        ItemHeight = 12
        PopupMenu = pmFileList
        TabOrder = 3
        OnChange = lbFileChange
        OnDblClick = pmiOpenClick
      end
      object cbIn: TCheckBox
        Left = 8
        Top = 273
        Width = 145
        Height = 17
        Caption = '�Զ��Ŵ�����Ӧ��ʾ(&T)'
        TabOrder = 4
        OnClick = cbInClick
      end
      object cbOut: TCheckBox
        Left = 8
        Top = 292
        Width = 145
        Height = 17
        Caption = '�Զ���С����Ӧ��ʾ(&B)'
        Checked = True
        State = cbChecked
        TabOrder = 5
        OnClick = cbInClick
      end
      object cbRate: TCheckBox
        Left = 8
        Top = 310
        Width = 137
        Height = 17
        Caption = '����ʱ���ֱ���(&K)'
        Checked = True
        State = cbChecked
        TabOrder = 6
        OnClick = cbInClick
      end
      object cbAutoScroll: TCheckBox
        Left = 8
        Top = 348
        Width = 89
        Height = 17
        Caption = '�Զ�����(&O)'
        TabOrder = 7
        OnClick = cbAutoScrollClick
      end
      object seAuto: TSpinEdit
        Left = 96
        Top = 346
        Width = 41
        Height = 21
        MaxLength = 3
        MaxValue = 999
        MinValue = 1
        TabOrder = 8
        Value = 5
        OnChange = seAutoChange
      end
      object cbActive: TCheckBox
        Left = 8
        Top = 329
        Width = 137
        Height = 17
        Caption = '����̬ͼ���л�(&D)'
        Checked = True
        State = cbChecked
        TabOrder = 9
        OnClick = cbInClick
      end
    end
    object tsSubImage: TTabSheet
      Caption = '��ͼ�����(&U)'
      ImageIndex = 2
      object lblSize1: TLabel
        Left = 184
        Top = 349
        Width = 60
        Height = 12
        Caption = 'ͼ��ߴ磺'
      end
      object lblLoadTime1: TLabel
        Left = 352
        Top = 349
        Width = 60
        Height = 12
        Caption = 'װ��ʱ�䣺'
      end
      object lblSubImage1: TLabel
        Left = 456
        Top = 349
        Width = 60
        Height = 12
        Caption = '��ͼ������'
      end
      object lblPlayOrder1: TLabel
        Left = 536
        Top = 349
        Width = 60
        Height = 12
        Caption = '�������У�'
      end
      object Panel2: TPanel
        Left = 184
        Top = 16
        Width = 425
        Height = 321
        BevelOuter = bvLowered
        Color = clBlack
        FullRepaint = False
        TabOrder = 0
        object imgSub: TISeeImage
          Left = 1
          Top = 1
          Width = 423
          Height = 319
          Align = alClient
          OnDblClick = pmiOpenClick
          PopupMenu = pmFileList
          Style = [isAutoZoomOut, isConstrainRate, isColorFrame, isCenter]
        end
      end
      object btnOpen: TButton
        Left = 8
        Top = 16
        Width = 75
        Height = 25
        Caption = '��ͼ��(&O)'
        TabOrder = 1
        OnClick = btnOpenClick
      end
      object btnSave: TButton
        Left = 96
        Top = 16
        Width = 75
        Height = 25
        Caption = '����ͼ��(&S)'
        TabOrder = 2
        OnClick = btnSaveClick
      end
      object lbSubImage: TListBox
        Left = 8
        Top = 48
        Width = 161
        Height = 238
        ItemHeight = 46
        Style = lbOwnerDrawFixed
        TabOrder = 3
        OnClick = lbSubImageClick
        OnDrawItem = lbSubImageDrawItem
      end
      object cbActive1: TCheckBox
        Left = 8
        Top = 348
        Width = 137
        Height = 17
        Caption = '����̬ͼ���л�(&D)'
        Checked = True
        State = cbChecked
        TabOrder = 4
        OnClick = cbIn1Click
      end
      object cbIn1: TCheckBox
        Left = 8
        Top = 292
        Width = 145
        Height = 17
        Caption = '�Զ��Ŵ�����Ӧ��ʾ(&T)'
        TabOrder = 5
        OnClick = cbIn1Click
      end
      object cbOut1: TCheckBox
        Left = 8
        Top = 310
        Width = 145
        Height = 17
        Caption = '�Զ���С����Ӧ��ʾ(&B)'
        Checked = True
        State = cbChecked
        TabOrder = 6
        OnClick = cbIn1Click
      end
      object cbRate1: TCheckBox
        Left = 8
        Top = 329
        Width = 137
        Height = 17
        Caption = '����ʱ���ֱ���(&K)'
        Checked = True
        State = cbChecked
        TabOrder = 7
        OnClick = cbIn1Click
      end
    end
  end
  object FormStorage: TFormStorage
    Options = [fpPosition]
    UseRegistry = True
    StoredProps.Strings = (
      'edtDir.InitialDir'
      'lbPath.Items'
      'rbDLLName.Checked'
      'rbFullName.Checked'
      'rbPluginName.Checked'
      'PageControl.ActivePage'
      'cbAutoScroll.Checked'
      'cbOut.Checked'
      'cbIn.Checked'
      'cbRate.Checked'
      'seAuto.Value'
      'ISeeOpenDialog.InitialDir'
      'ISeeSaveDialog.InitialDir'
      'ISeeOpenDialog.FilterIndex'
      'ISeeSaveDialog.FilterIndex')
    StoredValues = <
      item
        Name = 'Path'
        Value = ''
        OnSave = FormStorageStoredValues0Save
        OnRestore = FormStorageStoredValues0Restore
      end
      item
        Name = 'SavePath'
        Value = ''
        OnSave = FormStorageStoredValues1Save
        OnRestore = FormStorageStoredValues1Restore
      end>
    Left = 536
    Top = 8
  end
  object tmrAutoScroll: TTimer
    OnTimer = tmrAutoScrollTimer
    Left = 568
    Top = 8
  end
  object pmFileList: TPopupMenu
    OnPopup = pmFileListPopup
    Left = 504
    Top = 8
    object pmiOpen: TMenuItem
      Caption = '���ⲿ�����(&O)'
      Default = True
      OnClick = pmiOpenClick
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object pmiSaveAs: TMenuItem
      Caption = '���Ϊ(&S)...'
      OnClick = pmiSaveAsClick
    end
    object pmiCopy: TMenuItem
      Caption = '���Ƶ�(&C)...'
      OnClick = pmiCopyClick
    end
    object pmiMove: TMenuItem
      Caption = '�ƶ���(&M)...'
      OnClick = pmiMoveClick
    end
  end
  object ISeeOpenDialog: TISeeOpenDialog
    Options = [ofHideReadOnly]
    Left = 376
    Top = 8
  end
  object ISeeSaveDialog: TISeeSaveDialog
    Options = [ofHideReadOnly]
    Left = 408
    Top = 8
  end
  object ISeeMgr: TISeeMgr
    PluginExtName = 'DLL'
    Left = 440
    Top = 8
  end
end
