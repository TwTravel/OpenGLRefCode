/********************************************************************

	filter.h - ISeeͼ���������ͼ����ģ��ʵ�ִ���ͷ�ļ�

    ��Ȩ����(C) VCHelp-coPathway-ISee workgroup 2000 all member's

    ��һ����������������������������������������GNU ͨ�����֤
	�������޸ĺ����·�����һ���򡣻��������֤�ĵڶ��棬���ߣ�������
	��ѡ�����κθ��µİ汾��

    ������һ�����Ŀ����ϣ�������ã���û���κε���������û���ʺ��ض�
	Ŀ�ص������ĵ���������ϸ����������GNUͨ�����֤��

    ��Ӧ���Ѿ��ͳ���һ���յ�һ��GNUͨ�����֤(GPL)�ĸ����������û�У�
	д�Ÿ���
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	�������ʹ�ñ����ʱ��ʲô������飬�����µ�ַ����������ȡ����
	ϵ��
		http://isee.126.com
		http://www.vchelp.net
	��
		iseesoft@china.com

	���ߣ��ٷ�
   e-mail:ringphone@sina.com

   ����ʵ�֣�ͼ����ת�����ܺ�������

	�ļ��汾��
		Build 00617
		Date  2000-6-17

********************************************************************/

extern LPIMAGEPROCSTR lpProcInfo;
extern LPVOID lpPreViewData,lpBakData;
extern HBITMAP hbm;         				//�Ի��򱳾�ͼ��

int WINAPI FAR _export AccessPropFilter(LPIMAGEPROCSTR lpInfo);

int ShowWriterMessage();
void ShowCopyright();

void PaintDlgBk(HWND hWnd);						//���ƶԻ��򱳾�

int DoRotate(LPIMAGEPROCSTR lpInfo);	//��ת
int Output_Rotate();                   //�����ת���ͼ������

int DoGreyScale(LPIMAGEPROCSTR lpInfo);//�Ҷ�ת��

int DoResize(LPIMAGEPROCSTR lpInfo);
int Output_Resize(int width,int height,LPBYTE lpDestData);//�ض���ߴ�

