program ISeeTest;

uses
  Forms,
  MainFrm in 'MainFrm.pas' {MainForm},
  ISee in '..\ISee.PAS',
  iseeio in '..\iseeio.pas',
  iseeirw in '..\iseeirw.pas',
  ce in '..\ce.pas',
  ISeeCtrls in '..\ISeeCtrls.PAS';

{$R *.RES}

begin
  Application.Initialize;
  Application.Title := 'ISeeÍ¼Ïñ¶ÁÐ´²å¼þ²âÊÔ³ÌÐò';
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
