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

	���ߣ�daoshi
   e-mail:ringphone@sina.com

   ����ʵ�֣�

	�ļ��汾��
		Build 01315
		Date  2001-3-15

********************************************************************/
/******************MFCԭ����*************
#if !defined(AFX_PERLINNOISE2_H__4ED982A2_1A15_11D4_ADFE_9CFE56687A0C__INCLUDED_)
#define AFX_PERLINNOISE2_H__4ED982A2_1A15_11D4_ADFE_9CFE56687A0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
****************************************/
class PerlinNoise2
{
public:
	PerlinNoise2();
	virtual ~PerlinNoise2();
	float Noise2f(int x,int y);
	float SmoothedNoise2f(int x,int y);
	float CosineInterpolate1f(float a, float b, float x);
	float Interpolate1f(float  a, float b, float x);
	float InterpolatedNoise2f(float x, float y);
	float PerlinNoise2f(float x, float y);
};
/******************MFCԭ����*************
#endif // !defined(AFX_PERLINNOISE2_H__4ED982A2_1A15_11D4_ADFE_9CFE56687A0C__INCLUDED_)
****************************************/

