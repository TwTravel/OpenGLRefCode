// LoadTGA.cpp: implementation of the CLoadTGA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Billiard.h"
#include "LoadTGA.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadTGA::CLoadTGA()
{

}

CLoadTGA::~CLoadTGA()
{

}

//  下面函数的功能是将TGA文件中的数据读入到一个位图变量中
unsigned char *CLoadTGA::LoadTGAFile( char * strFilename,	tTGAHeader_s *header)
{
	short			BitsPerPixel;
	unsigned char	*buffer;
	int bitsize;		// 位图总的大小
	unsigned char	*newbits;
	unsigned char	*from, *to;
	int		i, j, width;
    FILE* file;
    long dwWidth, dwHeight;

    // 打开文件并读入文件头信息
	file = fopen( strFilename, "rb");
    if( NULL == file )
        return NULL;

    if ( fread( header, sizeof( tTGAHeader_s ), 1, file ) != 1 )
    {
        fclose( file );
        return NULL;
    }

    // 获得图像数据的大小
	dwWidth = (long)header->d_width;
	dwHeight = (long)header->d_height;
	BitsPerPixel = (short)header->d_pixel_size;          // 像素大小

    // 创建一个装入图像数据的位图
	bitsize = dwWidth * dwHeight * (BitsPerPixel/8);
	if ((newbits = (unsigned char *)calloc(bitsize, 1)) == NULL)
	{
        fclose( file );
        return NULL;
	}
 	buffer = (unsigned char *)malloc(dwWidth*dwHeight*(BitsPerPixel / 8));
    if ( fread( buffer, dwWidth*dwHeight*(BitsPerPixel / 8), 1, file ) != 1 )
	{
        fclose( file );
		free(buffer);
		free(newbits);
        return NULL;
	}

	width   = (BitsPerPixel / 8) * dwWidth;

    for (i = 0; i < dwHeight; i ++)
		for (j = 0, from = ((unsigned char *)buffer) + i * width,
	        to = newbits + i * width;
			j < dwWidth;
			j ++, from += (BitsPerPixel / 8), to += (BitsPerPixel / 8))
        {
				if (BitsPerPixel == 24)
				{
					to[0] = from[2];
					to[1] = from[1];
					to[2] = from[0];
				}
				else
				{
					to[0] = from[2];
					to[1] = from[1];
					to[2] = from[0];
					to[3] = from[3];
				}
        };
	free(buffer);
    fclose( file );

    return newbits;
}


