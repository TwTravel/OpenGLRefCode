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
//  TGA�ļ����ļ�ͷ�ṹ
typedef struct
{
   unsigned char  d_iif_size;            // ͨ��Ϊ0
   unsigned char  d_cmap_type;           // ����
   unsigned char  d_image_type;          // ӦΪ2
   unsigned char  pad[5];

   unsigned short d_x_origin;
   unsigned short d_y_origin;
   unsigned short d_width;
   unsigned short d_height;

   unsigned char  d_pixel_size;          // ���ص���ɫλ16��24��32
   unsigned char  d_image_descriptor;    // λ3-0: alphaͨ���Ĵ�С
                                         // λ4: ����(Ϊ0)
                                         // λ5: Ϊ0
                                         // λ6-7: Ϊ0
} tTGAHeader_s;

class CLoadTGA  
{
public:
	CLoadTGA();
	virtual ~CLoadTGA();

	unsigned char *LoadTGAFile( char* strFilename,tTGAHeader_s *header);

};

#endif // !defined(AFX_LOADTGA_H__DF6DBB46_E79E_47C9_BC4F_3F4A3C686064__INCLUDED_)
