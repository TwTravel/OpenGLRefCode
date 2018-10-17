// PerlinNoise2.h: interface for the PerlinNoise2 class.
//
//////////////////////////////////////////////////////////////////////
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
