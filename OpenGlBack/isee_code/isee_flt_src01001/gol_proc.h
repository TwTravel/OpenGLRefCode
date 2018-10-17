/********************************************************************

	gol_proc.h - ISeeͼ���������ͼ����ӿ�ȫ�ֶ����ļ�
          
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

	��д�ˣ��ٷ�

	�ļ��汾��
		Build 00606
		Date  2000-6-6

********************************************************************/



#ifndef __GOL_PROC_INC
#define __GOL_PROC_INC


// �ӿڰ汾��
#define	ISEE_PRVER			6

// ͼ�������ݰ����Ӳ����ߴ�
#define PPK_ANNEXINFOSIZE	512

//ͼ����ģ����ں�������ֵ
#define PROCERR_SUCCESS			1        //�ɹ�
#define PROCERR_FALSE			0        //ʧ�ܣ�һ���������resultֵ
#define PROCERR_FAIL				-1       //ʧ�ܣ�һ���ǵ��öԻ���ʧ�ܣ�ϵͳ����ȣ��Ǵ����WIN API����ʧ�ܷ���ֵ��
#define PROCERR_NULLPARAM		-2       //�������ΪNULLָ��

// ͼ�������ݰ�����
//************************************************************

// ͼ����ģ���������
enum PROCCOMM
{
	PCM_NULL,			   // δ֪����Ч������
	PCM_GETWRITERS,		// ��ȡ��������������һ����ʱ�ö��ŷָ�����
	PCM_GETWRITERMESS,	// ��ȡ�����ǵ�����
	PCM_GETBUILDID,		// ��ȡͼ��ģ���ڲ��汾��
	
	PCM_PROP,            // ͼ�����Դ���
	PCM_RESIZE,          // �ض���ߴ�	
	PCM_ROTATE,			   // ��ת
	PCM_SPLIT,           // �ָ�
	PCM_SPLICE,          // ƴ��
	PCM_GREYSCALE,       // �Ҷ�ת��
	PCM_REFINECOLOR,     // ��ʣ��߲ʣ�256ɫת��
	//PCM_XXXXX,         // ͼ�����Դ��������ڴ˲���
	
	PCM_MASS,            // ͼ����������
	PCM_FILTER,          // ��ɫ�����˲���
	PCM_CONTRAST,        // �������Աȶȴ���
	PCM_DETECTEDGE,      // ��Ե̽��
	//PCM_XXXXX,         // ͼ���������������ڴ˲���
	
	PCM_STEFF,           // ͼ��̬Ч������
	PCM_EMBOSS,          // ����Ч��
	PCM_VARIATIONS,      // ɫ������
	PCM_BLUR,            // ģ��
	PCM_BUTTON,          // ��ť������
	PCM_TWIRL,           // Ť��
	PCM_INTERLACE,       // ��˿
	PCM_MOSAIC,          // ������
	//PCM_XXXXX,         // ͼ��̬Ч�����������ڴ˲���
	
	PCM_ANIEFF,          // ͼ��̬Ч������
	PCM_FADEIN,          // ��ӳ
	PCM_ALPHASCROLL,     // ALPHA����
	PCM_BURNUP,          // ��������
	//PCM_XXXXX,         // ͼ��̬Ч�����������ڴ˲���
	PCM_MAX				   // �߽�ֵ
};


// ͼ����ģ��ִ�н��
enum PROCRESULT
{
	PR_SUCCESS,			// �����ɹ�
	PR_FAIL,			// ����ʧ��
	PR_NULL,			// δ����
	PR_MEMORYERR,		// �ڴ治�㣬���²���ʧ��
	PR_ILLCOMM,			// �Ƿ�ָ��ͻ���������������벻��ȷ��
	PR_COMMINFOERR,		// �������йص���Ϣ�������ȱ��
	PR_SYSERR,			// ����ϵͳ��ʱ���ȶ����޷�ִ������
	//PR_XXXX,			// �µĴ�����Ϣ�ڴ˲���
	PR_NOTEXE,			// ����û�б�ִ�У���ʾͼ����ģ�鲻֧�ָ�����
	PR_MAX				// �߽�ֵ
};


// ͼ�������ݰ�״̬
enum IPDPCKSTATE
{
	IPDPS_NULL,			// ��ǰ���ݰ�Ϊ��
	IPDPS_CONFIG,     // ���ö�̬�������ò���
	IPDPS_RUN,        // ���ö�̬�������в���
	//IPDPS_XXXXX,		// �����µ�״̬
	IPDPS_MAX			// �߽�ֵ
};




// ���ݽṹ����
//************************************************************

// ͼ��������ݰ�
struct _tagIMAGEPROCSTR;
typedef _tagIMAGEPROCSTR  IMAGEPROCSTR;
typedef _tagIMAGEPROCSTR  *LPIMAGEPROCSTR;

// ͼ��λ���������Ϣ
struct _tagIMAGEPROCDATAINFO;
typedef _tagIMAGEPROCDATAINFO  IMAGEPROCDATAINFO;
typedef _tagIMAGEPROCDATAINFO  *LPIMAGEPROCDATAINFO;




// ͼ��λ���������Ϣ
struct _tagIMAGEPROCDATAINFO
{
	int width;			// ͼ��Ŀ�ȣ�������Ϊ��λ��
	int height;			// ͼ��ĸ߶ȣ�������Ϊ��λ��
	int bitperpix; 	// ͼ��λ������ÿ��������ռλ��,��С16����Ϊ�޵�ɫ�崫��
	int byteperline;	// ͼ��λ������ÿ��ɨ������ռ���ֽ���
	BOOL bAlpha;		// ͼ��λ�����Ƿ��ALPHAͨ�������ΪTRUE����bitperpix����=32������ͼ������ǿ��ת��ΪCOLORREF*���ͣ�ɫֵ���λΪALPHAֵ����COLORREF col=0xaarrggbb
};


// ͼ��������ݰ�
struct _tagIMAGEPROCSTR
{
	PROCCOMM	comm;				// ��������
	PROCRESULT	result;				// ִ�еĽ��
	union {
		char	cAnnData;			// ���ϣ�����������ͬ���͵����ݣ�
		int		iAnnData;			// �ǲ���������ִ�н���ĸ�����Ϣ
		long	lAnnData;
		float	fAnnData;
		double	dfAnnData;
		unsigned short 	wAnnData;
		unsigned long 	dwAnnData;
		void			*pAnnData;
		unsigned int	siAnnData[PPK_ANNEXINFOSIZE/sizeof(int)];
		unsigned char	scAnnData[PPK_ANNEXINFOSIZE];
	} annexdata;

	HWND		hParentWnd;			// �����ھ��
	HINSTANCE	hInst;				// ͼ����ģ��ʵ�����

	IMAGEPROCDATAINFO sImageInfo;	// Դͼ��λ������Ϣ
	unsigned long  *(*psLineAddr);	// Դ��׼ͼ��λ�����д�0�е�n-1�е�ÿһ�е��׵�ַ
	unsigned char		*_psbdata;	// Դ��׼ͼ��λ���ݻ������׵�ַ

	IMAGEPROCDATAINFO dImageInfo;	// Ŀ��ͼ��λ������Ϣ
	unsigned long  *(*pdLineAddr);	// Ŀ�ı�׼ͼ��λ�����д�0�е�n-1�е�ÿһ�е��׵�ַ
	unsigned char		*_pdbdata;	// Ŀ�ı�׼ͼ��λ���ݻ������׵�ַ
									// ��ע��Ŀ��ͼ���ڴ�齫��ͼ����ģ����䣬����дdImageInfo������
	
	IPDPCKSTATE	state;				// ���ݰ���ǰ��״̬
	int			modify;				// �޸ı�־
};


#endif	//__GOL_PROC_INC