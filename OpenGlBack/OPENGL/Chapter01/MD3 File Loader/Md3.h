#ifndef _MD3_H
#define _MD3_H

// ����Ľṹ�屣���˴��ļ���ͷ����Ϣ
struct tMd3Header
{ 
	char	fileID[4];					// �ļ�ID������MD3��ʽ���ļ���������IDP3
	int		version;					// MD3��ʽ�ļ��İ汾�ţ�������15
	char	strFile[68];				// �ļ�������
	int		numFrames;					// ����֡����Ŀ
	int		numTags;					// ��ǩ����Ŀ
	int		numMeshes;					// �������Ӷ������Ŀ
	int		numMaxSkins;				// ������Ƥ������Ŀ
	int		headerSize;					// �ļ�ͷ����С
	int		tagStart;					// ��ǩ��Ϣ�Ŀ�ʼλ��
	int		tagEnd;						// ��ǩ��Ϣ�Ľ���λ��
	int		fileSize;					// �ļ���С
};

// ����Ľṹ�屣����MD3ģ���е���������
struct tMd3MeshInfo
{
	char	meshID[4];					// ����ID
	char	strName[68];				// ��������
	int		numMeshFrames;				// ֡����Ŀ
	int		numSkins;					// Ƥ������Ŀ
	int     numVertices;				// ������Ŀ
	int		numTriangles;				// �����������Ŀ
	int		triStart;					// �����εĿ�ʼλ��
	int		headerSize;					// �����ͷ�����ݵĴ�С
	int     uvStart;					// UV��������Ŀ�ʼλ��
	int		vertexStart;				// �������ݵĿ�ʼλ��
	int		meshSize;					// �����������ݵĴ�С
};

//  MD3�ļ���ʽ�ı�ǩ�ṹ
struct tMd3Tag
{
	char		strName[64];			// ��ǩ������
	CVector3	vPosition;				// ƽ�Ʊ任
	float		rotation[3][3];			// ��ת����
};

//  ����Ľṹ�屣���˹�����Ϣ 
struct tMd3Bone
{
	float	mins[3];					// ��Сֵ(x, y, z)
	float	maxs[3];					// ���ֵ(x, y, z)
	float	position[3];				// ����λ��
	float	scale;						// �������ű���
	char	creator[16];				
};


//  ����Ľṹ�屣������ķ������Ͷ�������
struct tMd3Triangle
{
   signed short	 vertex[3];				// ��Ķ���
   unsigned char normal[2];				// ������
};


//  ����Ľṹ�屣���˶��������������е�������
struct tMd3Face
{
   int vertexIndices[3];				
};


// ����Ľṹ�屣��UV��������
struct tMd3TexCoord
{
   float textureCoord[2];
};


// ����Ľṹ�屣��Ƥ������(Ҳ������������)
struct tMd3Skin 
{
	char strName[68];
};


// �������Ĺ����Ƕ���MD3�ļ��е�������Ϣ
class CLoadMD3
{

public:

	// ��ʼ�����ݳ�Ա
	CLoadMD3();								

	// ����MD3ģ��
	bool ImportMD3(t3DModel *pModel, char *strFileName);

	// ����ģ�͵�Ƥ���ļ�
	bool LoadSkin(t3DModel *pModel, LPSTR strSkin);

	// ����ʹ�õ������ļ�
	bool LoadShader(t3DModel *pModel, LPSTR strShader);

private:

	//  ��MD3�ļ��ж������ݣ������䱣���ڳ�Ա������
	void ReadMD3Data(t3DModel *pModel);

	//  ����Ա�����е�����ת����ģ�ͽṹ����
	void ConvertDataStructures(t3DModel *pModel, tMd3MeshInfo meshHeader);

	// �ͷ��ڴ���Դ���ر��ļ�
	void CleanUp();
	
	// �����ǳ�Ա����		
	// �ļ�ָ��
	FILE *m_FilePointer;

	tMd3Header				m_Header;			// �ļ�ͷ������

	tMd3Skin				*m_pSkins;			// Ƥ������
	tMd3TexCoord			*m_pTexCoords;		// ��������
	tMd3Face				*m_pTriangles;		// ������������
	tMd3Triangle			*m_pVertices;		// ��������
	tMd3Bone				*m_pBones;			// ��������
};

//  ������ģ�������࣬���ڻ���MD3ģ��
class CModelMD3
{

public:

	// ���캯������������
	CModelMD3();
	~CModelMD3();
	
	// װ��ָ��·�������Ƶ�MD3ģ��
	bool LoadModel(LPSTR strPath, LPSTR strModel);

	// װ������
	bool LoadWeapon(LPSTR strPath, LPSTR strModel);

	void LinkModel(t3DModel *pModel, t3DModel *pLink, LPSTR strTagName);

	// ����ģ��
	void DrawModel();

	// �ͷ�ģ��ռ�õ��ڴ���Դ
	void DestroyModel(t3DModel *pModel);
	
private:

	// װ��ģ�͵�����
	void LoadModelTextures(t3DModel *pModel, LPSTR strPath);

	void DrawLink(t3DModel *pModel);

	// ����ģ��
	void RenderModel(t3DModel *pModel);

	// ��Ա����

	// ģ���е�����
	UINT m_Textures[MAX_TEXTURES];	
	vector<string> strTextures;

	// ����Ĳ�ͬ��λ��ģ��
	t3DModel m_Head;
	t3DModel m_Upper;
	t3DModel m_Lower;

	// ����ģ��
	t3DModel m_Weapon;
};


#endif

