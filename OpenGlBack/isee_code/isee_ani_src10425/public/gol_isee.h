/********************************************************************

	gol_isee.h - ISeeͼ���������ͼ���дģ��ȫ�ֶ����ļ�
          
    ��Ȩ����(C) VCHelp-coPathway-ISee workgroup 2000 all member's

    ��һ����������������������������������������GNU ͨ�ù�����
	��֤�������޸ĺ����·�����һ���򡣻��������֤�ĵڶ��棬���ߣ���
	�����ѡ�����κθ��µİ汾��

    ������һ�����Ŀ����ϣ�������ã���û���κε���������û���ʺ��ض�
	Ŀ�ص������ĵ���������ϸ����������GNUͨ�ù������֤��

    ��Ӧ���Ѿ��ͳ���һ���յ�һ��GNUͨ�ù������֤�ĸ�������Ŀ¼
	GPL.TXT�ļ����������û�У�д�Ÿ���
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	�������ʹ�ñ����ʱ��ʲô������飬�����µ�ַ����������ȡ����
	ϵ��
		http://isee.126.com
		http://www.vchelp.net
	��
		iseesoft@china.com

	��д�ˣ�YZ, xiaoyueer, orbit

	�ļ��汾��
		Build 01016
		Date  2000-10-16

********************************************************************/



#ifndef __GOL_ISEE_INC
#define __GOL_ISEE_INC


// �ӿڰ汾��
#define	ISEE_IFVER		8

#define ISEE_IMAGE_BITCOUNT	32

#define DPK_ANNEXINFOSIZE	512
#define DPK_FILENAMESIZE	512

// ͼ����Ϣ�������Ͷ���
//************************************************************

// ͼ���дģ���������
enum COMMAND
{
	CMD_NULL,			// ����
	CMD_GETPROCTYPE,	// ��ȡ��ģ���ܴ����ͼ�����ͣ��磺BMP��PCX
	CMD_GETWRITERS,		// ��ȡ��ģ��������б�����ʱ�ö��ŷָ�
	CMD_GETWRITERMESS,	// ��ȡ�����ǵ�����
	CMD_GETBUILDID,		// ��ȡͼ��ģ���ڲ��汾��
	CMD_IS_VALID_FILE,	// �ж�ָ���ļ��Ƿ��ܱ���ģ�鴦��
	CMD_GET_FILE_INFO,	// ��ȡָ���ļ�����Ϣ
	CMD_LOAD_FROM_FILE,	// ��ָ��ͼ���ļ��ж�ȡ����
	CMD_SAVE_TO_FILE,	// �����ݱ��浽ָ���ļ���
	CMD_IS_SUPPORT,		// ��ѯĳ�������Ƿ�֧��
	CMD_RESIZE,			// ���»�ȡָ���ߴ��ͼ��λ���ݣ�ֻ������ʸ��ͼ��
	//CMD_XXXXXX,		// ������ڴ˲���
	CMD_MAX				// �߽�ֵ
};


// ͼ���дģ��ִ�н��
enum EXERESULT
{
	ER_SUCCESS,			// ִ�гɹ�
	ER_FILERWERR,		// �ļ��ڶ�д�����з�������ϵͳ��Ӳ����ԭ�������ļ���д�쳣��
	ER_MEMORYERR,		// �ڴ治�㣬���²���ʧ��
	ER_ILLCOMM,			// �Ƿ�ָ��
	ER_COMMINFOERR,		// �������йص���Ϣ�������ȱ��
	ER_SYSERR,			// ����ϵͳ���ȶ����޷�ִ������
	ER_NOTSUPPORT,		// ��֧�ֵ�����
	ER_USERBREAK,		// �������û��ж�
	ER_EMPTY,			// ��ʼ״ֵ̬�����ڿͻ�����ʼ�����ر���
	//@@@@@@@@@@Added By YZ 2000.8.21
	ER_BADIMAGE, 		// �����ͼ���ļ�
	//@@@@@@@@@@End 2000.8.21
	//ER_XXXX,			// �µĴ�����Ϣ�ڴ˲���
	ER_MAX				// �߽�ֵ
};

// ͼ������
enum IMGTYPE
{
	IMT_NULL,			// ��Чͼ������
	IMT_RESSTATIC,		// ��դ����̬ͼ��
	IMT_RESDYN,			// ��դ����̬ͼ��
	IMT_VECTORSTATIC,	// ʸ������̬ͼ��
	IMT_VECTORDYN,		// ʸ������̬ͼ��
	//IMT_XXXXX,		// �ڴ˲����µ�ͼ������
	IMT_MAX				// �߽�ֵ
};

// ͼ���ļ���ʽ
enum IMGFORMAT
{
	IMF_NULL,		// δ֪����Ч��ͼ���ļ�
	IMF_BMP,		// BMP��ʽ��ͼ���ļ�
	IMF_CUR,		// ....
	IMF_ICO,
	IMF_PCX,
	IMF_DCX,
	IMF_GIF,
	IMF_PNG,
	IMF_TGA,
	IMF_WMF,
	IMF_EMF,
	IMF_TIFF,
	IMF_JPEG,		//
	IMF_PSD,		// PSD��ʽ��ͼ���ļ�
	//IMF_XXX		// ������ʽ�ڴ˴�����
	IMF_MAX			// �߽�ֵ
};

// ���ݰ���ǰ״̬
enum PACKSTATE
{
	PKST_NULL,			// ���ݰ�Ϊ��
	PKST_NOTVER,		// ָ���ļ���δͨ����֤�Ľ׶Σ���ʱֻ���ļ������������ã�
	PKST_PASSVER,		// ָ�����ļ���ͨ����֤������Ч��ʽ���ļ�
	PKST_PASSINFO,		// ��ͨ����֤�������ݰ����и��ļ��������Ϣ
	PKST_INFOANDBITS,	// ��ͨ����֤�����и��ļ��������Ϣ��������ͼ���λ����
	PKST_MODIFY,		// ���ݰ��е�ͼ���Ѿ����޸Ĺ�
	//PKST_XXXXX,		// �����µ�״̬
	PKST_MAX			// �߽�ֵ
};


// ͼ��Ĵ洢��ʽ
enum IMGCOMPRESS
{
	ICS_RGB = 0,			// δ��ѹ����DIB�洢��ʽ(ARGB)
	ICS_RLE8,				// RLE8�洢��ʽ
	ICS_RLE4,				// RLE4�洢��ʽ
	ICS_BITFIELDS,			// ��λ�洢��ʽ
	ICS_PCXRLE,				// PCX RLE�洢��ʽ
	ICS_GIFLZW,				// GIF LZW�洢��ʽ
	ICS_GDIRECORD,			// WMF��Ԫ�ļ��洢��ʽ
	//@@@@@@@@@@Added By xiaoyueer 2000.8.20
	ICS_RLE32,				// RLE32�洢��ʽ
	ICS_RLE24,				// RLE24�洢��ʽ
	ICS_RLE16,				// RLE16�洢��ʽ
	//@@@@@@@@@End 2000.8.20
	//@@@@@@@@@@Added By orbit 2000.8.31
	ICS_JPEGGRAYSCALE,		// �Ҷ�ͼ��,JPEGֻ֧��256���Ҷ�
	ICS_JPEGRGB,			//red/green/blue 
	ICS_JPEGYCbCr,			//Y/Cb/Cr (also known as YUV)
	ICS_JPEGCMYK,			//C/M/Y/K
	ICS_JPEGYCCK,			//Y/Cb/Cr/K
	//@@@@@@@@@End 2000.8.31
	//@@@@@@@@@@LibTiff: Sam Leffler 2001.1.6
	ICS_TIFF_NONE,			// TIFF non-compress ABGR mode
	ICS_TIFF_CCITTRLE,		// CCITT modified Huffman RLE
	ICS_TIFF_CCITTAX3,		// CCITT Group 3 fax encoding
	ICS_TIFF_CCITTFAX4,		// CCITT Group 4 fax encoding
	ICS_TIFF_LZW,			// Lempel-Ziv  & Welch
	ICS_TIFF_OJPEG,			// 6.0 JPEG
	ICS_TIFF_JPEG,			// JPEG DCT compression
	ICS_TIFF_NEXT,			// NeXT 2-bit RLE
	ICS_TIFF_CCITTRLEW,		// 1 w/ word alignment
	ICS_TIFF_PACKBITS,		// Macintosh RLE
	ICS_TIFF_THUNDERSCAN,	// ThunderScan RLE
	//@@@@@@@@@@Added By <dkelly@etsinc.com>
	ICS_TIFF_IT8CTPAD,		// IT8 CT w/padding
	ICS_TIFF_IT8LW,			// IT8 Linework RLE
	ICS_TIFF_IT8MP,			// IT8 Monochrome picture
	ICS_TIFF_IT8BL,			// IT8 Binary line art
	ICS_TIFF_PIXARFILM,		// Pixar companded 10bit LZW
	ICS_TIFF_PIXARLOG,		// Pixar companded 11bit ZIP
	ICS_TIFF_DEFLATE,		// Deflate compression
	ICS_TIFF_ADOBE_DEFLATE,	// Deflate compression, as recognized by Adobe
	//@@@@@@@@@@Added By <dev@oceana.com>
	ICS_TIFF_DCS,			// Kodak DCS encoding
	ICS_TIFF_JBIG,			// ISO JBIG
	ICS_TIFF_SGILOG,		// SGI Log Luminance RLE
	ICS_TIFF_SGILOG24,		// SGI Log 24-bit packed
	//@@@@@@@@@End 2001.1.6
	//ICS_XXXX,				// �����µĴ洢��ʽ����
	ICS_UNKONW = 1024,		// δ֪��ѹ����ʽ
	ICS_MAX = 2048			// �߽�
};



							
// ���ݽṹ����
//************************************************************

// ��׼ͼ���ʽ��Ϣͷ�ṹ
struct _tagINTERBITMAPINFO;
typedef _tagINTERBITMAPINFO	INTERBITMAPINFO;
typedef _tagINTERBITMAPINFO	*LPINTERBITMAPINFO;

// ͼ����Ϣ�ṹ�����ڶ�֡��ͼ�����½ṹֻ����������һ֡��
struct _tagIMAGEINFOSTR;
typedef _tagIMAGEINFOSTR  IMAGEINFOSTR;
typedef _tagIMAGEINFOSTR  *LPIMAGEINFOSTR;

// ͼ���д���ݰ��ṹ
struct _tagINFOSTR;
typedef _tagINFOSTR  INFOSTR;
typedef _tagINFOSTR  *LPINFOSTR;

// ����ͼ�����ͼ���ṹ����Ҫ���ڶ�����ͼ���ļ���
struct _tagSUBIMGBLOCK;
typedef _tagSUBIMGBLOCK	SUBIMGBLOCK;
typedef _tagSUBIMGBLOCK	*LPSUBIMGBLOCK;

// ������Ա��Ϣ
struct _tagDeveloperInfo;
typedef _tagDeveloperInfo DEVELOPERINFO;
typedef _tagDeveloperInfo *LPDEVELOPERINFO;

// IRW����������ݽṹ
struct _tagPlugsIn;
typedef _tagPlugsIn	PLUGSIN;
typedef _tagPlugsIn *LPPLUGSIN;



// ����ͼ�����ͼ���ṹ
struct _tagSUBIMGBLOCK
{
	int		number;				// ��ͼ�������к�
	int		left, top;			// ��ͼ������ʾλ�ã����Ͻǣ�
	int		width, height;		// ��ͼ����ͼ���ȼ��߶�
	int		dowith;				// ���÷���������ڶ���GIF��
	int		userinput;			// �û����루����ڶ���GIF��
	COLORREF colorkey;			// ͸��ɫ����ֵ
	int		time;				// ��ʾ���ӳ�ʱ��

	unsigned long	  *(*pLineAddr);// ��׼ͼ��λ�����д�0�е�n-1�е�ÿһ�е��׵�ַ
	unsigned char		*_pbdata;	// ��׼ͼ��λ���ݻ������׵�ַ
	
	LPINFOSTR		parents;	// ��һ��ͼ�����ݵĽṹ��ַ���������ݰ��ṹ�ĵ�ַ��
	LPSUBIMGBLOCK	prev;		// ǰһ��ͼ�����ݵĽṹ��ַ
	LPSUBIMGBLOCK	next;		// ��һ��ͼ�����ݵĽṹ��ַ��NULL��ʾ�������һ��ͼ��
};


// ��׼ͼ���ʽ��Ϣͷ�ṹ
struct _tagINTERBITMAPINFO
{
	BITMAPINFOHEADER	bmi;	// ��׼ͼ���ʽ����Ϣ�ṹ
	DWORD	rmask;				// ��ɫ����
	DWORD	gmask;				// ��ɫ����
	DWORD	bmask;				// ��ɫ����
	DWORD   alphaMask;			// Alpha����
};


// ͼ����Ϣ�ṹ�����ڶ�֡��ͼ�����½ṹֻ������������ͼ��
struct _tagIMAGEINFOSTR
{
	IMGTYPE		imgtype;			// ͼ������
	IMGFORMAT	imgformat;			// ͼ���ļ���ʽ
	//@@@@@@@@@@@@@Added By orbit for file's saving function at 2000.8.31
	BYTE		filename[DPK_FILENAMESIZE];//ͼ���ԭʼ�ļ��������Ϊ����ʱ����ԭʼ�ļ������֣���
	int			qualtify;			// ͼ��������ֻ������JPEG�ļ���
	//@@@@@@@@@@@@End 2000.8.31
	unsigned long	filesize;		// ͼ���ļ��ĳ��ȣ����ֽ�Ϊ��λ��
	unsigned long	width;			// ͼ����
	unsigned long	height;			// ͼ��߶�
	unsigned long	bitcount;		// ����λ������Ӧ����ɫ����
	IMGCOMPRESS		compression;	// ͼ��Ĵ洢��ʽ
	unsigned long	linesize;		// ÿһͼ������ռ���ֽ�����DWORD���룬ֻ���BMP�ļ���
	unsigned long	imgnumbers;		// ���ļ���ͼ��ĸ�������ͨͼ���ļ���ֵ�㶨Ϊ1�����ڶ���GIF�п��ܻ����1��
	unsigned int	imgchang;		// ���ļ��Ƿ�ɱ��༭��0-��ʾ���Ա��༭��1-��ʾ���ܱ��༭�����綯��GIF�Ͳ��ܱ��༭��
	unsigned short	year;			// ͼ���ļ�����޸ĵ���
	unsigned short	monday;			// ͼ���ļ�����޸ĵ��º��գ����ڸ��ֽڣ����ڵ��ֽڣ�
	unsigned long	time;			// ͼ���ļ�����޸ĵ�ʱ�䣨������ߡ�0, 2��ʱ��1���֣�0���룩
};



// ͼ���д���ݰ��ṹ
struct _tagINFOSTR
{
	COMMAND		comm;				// ��������
	EXERESULT	result;				// ִ�еĽ�����ǳɹ��˻���ʧ���ˣ�
	union {
		char	cAnnData;			// ���ϣ�����������ͬ���͵����ݣ�
		int		iAnnData;			// �ǲ���������ִ�н���ĸ�����Ϣ��
		long	lAnnData;			// �μ�ÿ�������жԴ�ֵ�ĺ���˵��
		float	fAnnData;
		double	dfAnnData;
		COMMAND	cmAnnData;			// �������֧�����ʱ�õ�
		unsigned short 	wAnnData;
		unsigned long 	dwAnnData;
		void			*pAnnData;
		unsigned int	siAnnData[DPK_ANNEXINFOSIZE/sizeof(int)];
		unsigned char	scAnnData[DPK_ANNEXINFOSIZE];
	} annexdata;

	IMAGEINFOSTR imginfo;			// ͼ���ļ���Ϣ
	char filename[DPK_FILENAMESIZE];// ͼ���ļ���·�����ļ���		
	void *pImgInfo;					// ԭʼͼ����Ϣ

	INTERBITMAPINFO		sDIBInfo;	// ��׼ͼ���ʽ��Ϣ�ṹ
	unsigned long	  *(*pLineAddr);// ��׼ͼ��λ�����д�0�е�n-1�е�ÿһ�е��׵�ַ
	unsigned char		*_pbdata;	// ��׼ͼ��λ���ݻ������׵�ַ

	PACKSTATE	state;				// ���ݰ���ǰ��״̬
	int			modify;				// �޸ı��
	LPSUBIMGBLOCK psubimg;			// ��ͼ�����ݿ��ַ����̬ͼ���ֵӦ��ΪNULL��
	int (WINAPI *fpProgress)(int,int);
									// �ɵ������ṩ�Ľ�������ʾ����ָ�루������NULL��
									// ��ڲ���1���ܵĽ�����������2�ǵ�ǰ�Ľ�������
									// ���ز������������0����ͼ���дģ�����������
									// ������ط�0ֵ�����дģ����ֹ�������������ݰ���
};


// ��������Ϣ�ṹ
struct _tagDeveloperInfo
{
	BYTE	modulename[16];		// ��������
	BYTE	author[16];			// �������֣�����ʱ�ö��ŷָ���
	BYTE	EMail[32];			// ���ߵ��ʵ�ַ������ʱ�ö��ŷָ���
	BYTE	messages[144];		// ���ߵ�������Ϣ
};



// ����������ݽṹ����
struct _tagPlugsIn
{
	BYTE	IRWID[4];			// IRW�����ʶ�����㶨Ϊ�ַ�"IRWP"
	WORD	attrib;
	WORD	version;
	DWORD	function;			// 0λ�Ƕ�֧�ֱ�־��
								// 1λ��д֧�ֱ�־��
								// 2λ��"ָ���ߴ�"�Ķ�֧�ֱ�־��(ReSize)
								// ����δ��

	DWORD	proctypenum;
	BYTE	proctypestr[32];	// 4*8 �����飬ÿ��4�ֽ�

	BYTE	author[16];
	BYTE	EMail[32];

	BYTE	messages[144];

	BYTE	modulename[16];		// ģ�������
};	// ��256���ֽ�

#endif	//__GOL_ISEE_INC