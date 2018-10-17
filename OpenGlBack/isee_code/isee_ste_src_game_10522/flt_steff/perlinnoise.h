/********************************************************************

	filter.h - ISee图像浏览器—图像处理模块实现代码头文件

    版权所有(C) VCHelp-coPathway-ISee workgroup 2000 all member's

    这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用许可证
	条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根据你
	的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用许可证。

    你应该已经和程序一起收到一份GNU通用许可证(GPL)的副本。如果还没有，
	写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	如果你在使用本软件时有什么问题或建议，用以下地址可以与我们取得联
	系：
		http://isee.126.com
		http://www.vchelp.net
	或：
		iseesoft@china.com

	作者：daoshi
   e-mail:ringphone@sina.com

   功能实现：

	文件版本：
		Build 01315
		Date  2001-3-15

********************************************************************/
/******************MFC原代码*************
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
/******************MFC原代码*************
#endif // !defined(AFX_PERLINNOISE2_H__4ED982A2_1A15_11D4_ADFE_9CFE56687A0C__INCLUDED_)
****************************************/

