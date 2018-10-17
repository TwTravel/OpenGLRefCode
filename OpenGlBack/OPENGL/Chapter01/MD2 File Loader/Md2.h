#ifndef _MD2_H
#define _MD2_H

// ����MD2�ļ��еĸ��ֲ��������ֵ
#define MD2_MAX_TRIANGLES		4096
#define MD2_MAX_VERTICES		2048
#define MD2_MAX_TEXCOORDS		2048
#define MD2_MAX_FRAMES			512
#define MD2_MAX_SKINS			32
#define MD2_MAX_FRAMESIZE		(MD2_MAX_VERTICES * 4 + 128)

// ���½ṹ�屣����MD2�ļ���ͷ����Ϣ
struct tMd2Header
{ 
   int magic;					// �ļ�ʶ���
   int version;					// �ļ��İ汾��(������8)
   int skinWidth;				// Ƥ���Ŀ��(������Ϊ��λ)
   int skinHeight;				// Ƥ���ĸ߶�(������Ϊ��λ)
   int frameSize;				// ÿ֡�Ĵ�С(���ֽ�Ϊ��λ)
   int numSkins;				// ��ģ�͹�����Ƥ������
   int numVertices;				// ��������
   int numTexCoords;			// ������������
   int numTriangles;			// ��(�����)������
   int numGlCommands;			// gl���������
   int numFrames;				// ����֡������
   int offsetSkins;				// Ƥ���������ļ��е�ƫ�Ƶ�ַ
   int offsetTexCoords;			// �����������ļ��е�ƫ�Ƶ�ַ
   int offsetTriangles;			// ���������ļ��е�ƫ�Ƶ�ַ
   int offsetFrames;			// ֡�������ļ��е�ƫ�Ƶ�ַ
   int offsetGlCommands;		// gl�����������ļ��е�ƫ�Ƶ�ַ
   int offsetEnd;				// �ļ�����λ�����ļ��е�ƫ�Ƶ�ַ
};


// ���ڱ���ӵ�ǰ֡�ж���Ķ�������
struct tMd2AliasTriangle
{
   byte vertex[3];
   byte lightNormalIndex;
};

// ���ڱ���ӵ�ǰ֡�ж���ķ������Ͷ���
struct tMd2Triangle
{
   float vertex[3];
   float normal[3];
};

// ���ڱ��涥������������ڶ����е�����
struct tMd2Face
{
   short vertexIndices[3];
   short textureIndices[3];
};

// ���ڱ���UV����
struct tMd2TexCoord
{
   short u, v;
};

// ���ڱ��涯��֡�����š�ƽ�ƺ����Ƶ���Ϣ����������
struct tMd2AliasFrame
{
   float scale[3];
   float translate[3];
   char name[16];
   tMd2AliasTriangle aliasVertices[1];
};

// ���ڱ���任���֡��������
struct tMd2Frame
{
   char strName[16];
   tMd2Triangle *pVertices;
};

// ���ڱ���Ƥ������
typedef char tMd2Skin[64];


// ����������ڶ������е�ģ������
class CLoadMD2
{

public:
	CLoadMD2();					// ��ʼ�����ݳ�Ա

	// ��MD2�ļ�������ģ��
	bool ImportMD2(t3DModel *pModel, char *strFileName, char *strTexture);

private:
	
	// ��MD2�ļ��ж������ݲ������ڳ�Ա������
	void ReadMD2Data();

	// ת����Ա������pModel�ṹ����
	void ConvertDataStructures(t3DModel *pModel);

	// �������Ķ��㷨����(���ڹ���)
	void ComputeNormals(t3DModel *pModel);

	// �ͷ��ڴ���Դ���ر��ļ�
	void CleanUp();
	
	// �ļ�ָ��
	FILE *m_FilePointer;

	// ��Ա����		
	tMd2Header				m_Header;			// �ļ�ͷ����
	tMd2Skin				*m_pSkins;			// Ƥ������
	tMd2TexCoord			*m_pTexCoords;		// ��������
	tMd2Face				*m_pTriangles;		// ���������Ϣ
	tMd2Frame				*m_pFrames;			// ����֡��Ϣ
};


#endif


