/********************************************************************

	perlinnoise.cpp - ISee图像浏览器—图像处理模块图像分级细化噪声处理实现代码文件

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

   功能实现：分级细化噪声实现

	文件版本：
		Build 00617
		Date  2000-6-17

********************************************************************/

#include "p_win.h"
#include "gol_proc.h"
#include "commctrl.h"
#include "filter.h"
#include "draw.h"
#include "resource.h"
#include "PerlinNoise.h"
#include <math.h>

PerlinNoise2 gp;
extern HWND hProgressWnd;

int Output_PerlinNoise(int width,int height,float wlcoefx,float wlcoefy,int levels,COLORREF* lpData)
{
	int i;
   int j;
	float f;
	float r;
	float basex = wlcoefx;
	float basey = wlcoefy;
	int a;
   LPBYTE Data = (LPBYTE)lpData;
   LPBYTE lpBak = Data;
   float step = (float)height/100;
   float stepinc = step;
   int stepcnt = step>1?1:1/step;
   SendMessage(hProgressWnd,PBM_SETPOS,0,0);
   SendMessage(hProgressWnd,PBM_SETSTEP,stepcnt,0);

	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			r=0;
			for(f=0;f<levels;f++)
			{
				r += gp.InterpolatedNoise2f(j*pow(2,f)/basex,i*pow(2,f)/basey)/pow(2,f);
			}
			//r=gp.PerlinNoise2f(j/2.0,i/2.0);
			if(r>1)r=1;
			else if(r<-1)r=-1;
			a=(r+1)/2*0xff;
//Added by 临风
         *Data += (BYTE)a;
         Data ++;
         *Data += (BYTE)a;
         Data ++;
         *Data += (BYTE)a;
         Data +=2;

/**** DAOSHI原代码 *******************
			*(Data+(i*width+j)*4)=a;
			*(Data+(i*width+j)*4+1)=a;
			*(Data+(i*width+j)*4+2)=a;
			*(Data+(i*width+j)*4+3)=0xff;
************************************/
		}
      lpBak += (width << 2);
      Data = lpBak;
      if(i > step)
      {
      	step += stepinc * stepcnt;
			SendMessage(hProgressWnd,PBM_STEPIT,0,0);
      }
	}
	return TRUE;
}

int PerlinNoise(float wlcoefx,float wlcoefy,int levels)
{
   int w = lpProcInfo->sImageInfo.width;
   int h = lpProcInfo->sImageInfo.height;

   lpProcInfo->_pdbdata = (unsigned char*)New(w*h*(lpProcInfo->sImageInfo.bitperpix/8));
   if(lpProcInfo->_pdbdata==NULL)
   {
   	lpProcInfo->result=PR_MEMORYERR;
   	return PROCERR_FALSE;
   }

   //填写图像信息
   lpProcInfo->dImageInfo.width=w;
   lpProcInfo->dImageInfo.height=h;

   lpProcInfo->pdLineAddr=(unsigned long**)New(lpProcInfo->dImageInfo.height*sizeof(long));
   if(lpProcInfo->pdLineAddr==NULL)
   {
   	lpProcInfo->result=PR_MEMORYERR;
   	return PROCERR_FALSE;
   }

   lpProcInfo->dImageInfo.bitperpix=lpProcInfo->sImageInfo.bitperpix;
   lpProcInfo->dImageInfo.bAlpha=lpProcInfo->sImageInfo.bAlpha;
   int bpl=w*(lpProcInfo->sImageInfo.bitperpix/8);
   bpl=bpl%4==0?bpl:(bpl/4+1)*4;
   lpProcInfo->dImageInfo.byteperline=bpl;

   for(int i=0;i<lpProcInfo->dImageInfo.height;i++)
   	lpProcInfo->pdLineAddr[i]=(unsigned long*)lpProcInfo->_pdbdata+lpProcInfo->dImageInfo.byteperline*i;

   CopyMemory(lpProcInfo->_pdbdata,lpProcInfo->_psbdata,lpProcInfo->sImageInfo.width*lpProcInfo->sImageInfo.height*(lpProcInfo->sImageInfo.bitperpix/8));
   int r=Output_PerlinNoise(w,h,wlcoefx,wlcoefy,levels,(COLORREF*)lpProcInfo->_pdbdata);
   if(r)
   {
  		lpProcInfo->result=PR_SUCCESS;
		return PROCERR_SUCCESS;
   }
   else
    	return r;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PerlinNoise2::PerlinNoise2()
{

}

PerlinNoise2::~PerlinNoise2()
{

}

float PerlinNoise2::Noise2f(int x,int y)
{
	int n;
	n = x + y * 57;
	n = (n<<13) ^ n;
	return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float PerlinNoise2::SmoothedNoise2f(int x,int y)
{
	float corners,sides,center;
    corners = ( Noise2f(x-1, y-1)+Noise2f(x+1, y-1)+Noise2f(x-1, y+1)+Noise2f(x+1, y+1) ) / 16.0f;
    sides   = ( Noise2f(x-1, y)  +Noise2f(x+1, y)  +Noise2f(x, y-1)  +Noise2f(x, y+1) ) /  8.0f;
    center  =  Noise2f(x, y) / 4.0f;
    return corners + sides + center;
}

float PerlinNoise2::CosineInterpolate1f(float a, float b, float x)
{
	float ft,f;
	ft = x * 3.1415927f;
	f = (1 - (float)cos(ft)) * 0.5f;
	return  a*(1-f) + b*f;
}

float PerlinNoise2::Interpolate1f(float  a, float b, float x)
{
	return CosineInterpolate1f(a,b,x);
}

float PerlinNoise2::InterpolatedNoise2f(float x, float y)
{
	//integer_X    = int(x)
	//fractional_X = x - integer_X
	//integer_Y    = int(y)
	//fractional_Y = y - integer_Y
	double integer_X,integer_Y;
	double fractional_X,fractional_Y;
	float v1,v2,v3,v4;
	float i1,i2;

	fractional_X=modf( x, &integer_X );

	fractional_Y=modf( y, &integer_Y );

	v1 = SmoothedNoise2f((int)integer_X,     (int)integer_Y);
	v2 = SmoothedNoise2f((int)integer_X + 1, (int)integer_Y);
	v3 = SmoothedNoise2f((int)integer_X,     (int)integer_Y + 1);
	v4 = SmoothedNoise2f((int)integer_X + 1, (int)integer_Y + 1);

	i1 = Interpolate1f(v1 , v2 , (float)fractional_X);
	i2 = Interpolate1f(v3 , v4 , (float)fractional_X);

	return Interpolate1f(i1 , i2 , (float)fractional_Y);
}

float PerlinNoise2::PerlinNoise2f(float x, float y)
{
	float total = 0;
	float p = 0.25f;//p = persistence
	float n = 6 - 1;//n = Number_Of_Octaves - 1
	float frequency,amplitude;
	for(int i=0;i<n;i++)
	{
		frequency = (float)pow(2,i);
		amplitude = (float)pow(p,i);
		total+=InterpolatedNoise2f(x*frequency,y*frequency)*amplitude;
	}
	return total;
}


