// LoadTGA.h: interface for the CLoadTGA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADTGA_H__DF6DBB46_E79E_47C9_BC4F_3F4A3C686064__INCLUDED_)
#define AFX_LOADTGA_H__DF6DBB46_E79E_47C9_BC4F_3F4A3C686064__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define RGB16(r,g,b)   ( ((r>>3) << 10) + ((g>>3) << 5) + (b >> 3) )
#define RGB24(r,g,b)   ( ((r) << 16) + ((g) << 8) + (b) )
#define GET16R(v)   (v >> 10)
#define GET16G(v)   ((v >> 5) & 0x1f)
#define GET16B(v)   (v & 0x1f)
//  TGA文件的文件头结构
typedef struct
{
   unsigned char  d_iif_size;            // 通常为0
   unsigned char  d_cmap_type;           // 忽略
   unsigned char  d_image_type;          // 应为2
   unsigned char  pad[5];

   unsigned short d_x_origin;
   unsigned short d_y_origin;
   unsigned short d_width;
   unsigned short d_height;

   unsigned char  d_pixel_size;          // 像素的颜色位16、24或32
   unsigned char  d_image_descriptor;    // 位3-0: alpha通道的大小
                                         // 位4: 保留(为0)
                                         // 位5: 为0
                                         // 位6-7: 为0
} tTGAHeader_s;

class CLoadTGA  
{
public:
	CLoadTGA();
	virtual ~CLoadTGA();

	unsigned char *LoadTGAFile( char* strFilename,tTGAHeader_s *header);

};

#endif // !defined(AFX_LOADTGA_H__DF6DBB46_E79E_47C9_BC4F_3F4A3C686064__INCLUDED_)
