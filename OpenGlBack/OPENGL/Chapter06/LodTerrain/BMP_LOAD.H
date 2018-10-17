#ifndef BMP_LOAD_H
#define BMP_LOAD_H

typedef unsigned char BYTE;
typedef short int SHORT;
typedef long int LONG;
typedef unsigned short int WORD;
typedef unsigned long int DWORD;

typedef struct TEXTURE
{
	int w, h;
	unsigned char *data;
} TEXTURE;

typedef struct _Win3xBitmapHeader
{
	WORD	ImageFileType;
	DWORD FileSize;
	WORD	Reserved1;
	WORD	Reserved2;
	DWORD	ImageDataOffset;

} WIN3XHEAD;

typedef struct _Win3xBitmapInfoHeader
{
	DWORD	HeaderSize;
	LONG ImageWidth;
	LONG ImageHeight;
	WORD	NumberOfImagePlanes;
	WORD	BitsPerPixel;
	DWORD	CompressionMethod;
	DWORD SizeOfBitmap;
	LONG HorzResolution;
	LONG VertResolution;
	DWORD NumColorsUsed;
	DWORD NumSignificantColors;

} WIN3XINFOHEAD;

typedef struct _Win3xPixelData
{
	BYTE r;
	BYTE g;
	BYTE b;
} PAL;

class LAND_TEX 
{
public:
	LAND_TEX();
	~LAND_TEX();
	void NEW_BITMAP(unsigned int *, int *);

	TEXTURE *tex[4][4];
	unsigned int texture_name[4][4];
};

#define INDEX_TEXTURE_NAME(x, z) ((x) + ((z)*8))

TEXTURE *load_bitmap(char *); 
void destroy_bmp(TEXTURE *);

#endif BMP_LOAD_H
