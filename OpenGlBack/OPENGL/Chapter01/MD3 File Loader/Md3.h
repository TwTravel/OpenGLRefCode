#ifndef _MD3_H
#define _MD3_H

// 下面的结构体保存了从文件的头部信息
struct tMd3Header
{ 
	char	fileID[4];					// 文件ID，对于MD3格式的文件，必须是IDP3
	int		version;					// MD3格式文件的版本号，必须是15
	char	strFile[68];				// 文件的名称
	int		numFrames;					// 动画帧的数目
	int		numTags;					// 标签的数目
	int		numMeshes;					// 网格中子对象的数目
	int		numMaxSkins;				// 网格中皮肤的数目
	int		headerSize;					// 文件头部大小
	int		tagStart;					// 标签信息的开始位置
	int		tagEnd;						// 标签信息的结束位置
	int		fileSize;					// 文件大小
};

// 下面的结构体保存了MD3模型中的网格数据
struct tMd3MeshInfo
{
	char	meshID[4];					// 网格ID
	char	strName[68];				// 网格名称
	int		numMeshFrames;				// 帧的数目
	int		numSkins;					// 皮肤的数目
	int     numVertices;				// 顶点数目
	int		numTriangles;				// 三角形面的数目
	int		triStart;					// 三角形的开始位置
	int		headerSize;					// 网格的头部数据的大小
	int     uvStart;					// UV纹理坐标的开始位置
	int		vertexStart;				// 顶点数据的开始位置
	int		meshSize;					// 整个网格数据的大小
};

//  MD3文件格式的标签结构
struct tMd3Tag
{
	char		strName[64];			// 标签的名称
	CVector3	vPosition;				// 平移变换
	float		rotation[3][3];			// 旋转矩阵
};

//  下面的结构体保存了骨骼信息 
struct tMd3Bone
{
	float	mins[3];					// 最小值(x, y, z)
	float	maxs[3];					// 最大值(x, y, z)
	float	position[3];				// 骨骼位置
	float	scale;						// 骨骼缩放比例
	char	creator[16];				
};


//  下面的结构体保存了面的法向量和顶点索引
struct tMd3Triangle
{
   signed short	 vertex[3];				// 面的顶点
   unsigned char normal[2];				// 法向量
};


//  下面的结构体保存了顶点和纹理坐标队列的索引号
struct tMd3Face
{
   int vertexIndices[3];				
};


// 下面的结构体保存UV纹理坐标
struct tMd3TexCoord
{
   float textureCoord[2];
};


// 下面的结构体保存皮肤名称(也就是纹理名称)
struct tMd3Skin 
{
	char strName[68];
};


// 下面的类的功能是读入MD3文件中的所有信息
class CLoadMD3
{

public:

	// 初始化数据成员
	CLoadMD3();								

	// 读入MD3模型
	bool ImportMD3(t3DModel *pModel, char *strFileName);

	// 读入模型的皮肤文件
	bool LoadSkin(t3DModel *pModel, LPSTR strSkin);

	// 读入使用的武器文件
	bool LoadShader(t3DModel *pModel, LPSTR strShader);

private:

	//  从MD3文件中读出数据，并将其保存在成员变量中
	void ReadMD3Data(t3DModel *pModel);

	//  将成员变量中的数据转换到模型结构体中
	void ConvertDataStructures(t3DModel *pModel, tMd3MeshInfo meshHeader);

	// 释放内存资源，关闭文件
	void CleanUp();
	
	// 以下是成员变量		
	// 文件指针
	FILE *m_FilePointer;

	tMd3Header				m_Header;			// 文件头部数据

	tMd3Skin				*m_pSkins;			// 皮肤数据
	tMd3TexCoord			*m_pTexCoords;		// 纹理坐标
	tMd3Face				*m_pTriangles;		// 三角形面数据
	tMd3Triangle			*m_pVertices;		// 顶点数据
	tMd3Bone				*m_pBones;			// 骨骼数据
};

//  以下是模型数据类，用于绘制MD3模型
class CModelMD3
{

public:

	// 构造函数和析构函数
	CModelMD3();
	~CModelMD3();
	
	// 装入指定路径和名称的MD3模型
	bool LoadModel(LPSTR strPath, LPSTR strModel);

	// 装入武器
	bool LoadWeapon(LPSTR strPath, LPSTR strModel);

	void LinkModel(t3DModel *pModel, t3DModel *pLink, LPSTR strTagName);

	// 绘制模型
	void DrawModel();

	// 释放模型占用的内存资源
	void DestroyModel(t3DModel *pModel);
	
private:

	// 装入模型的纹理
	void LoadModelTextures(t3DModel *pModel, LPSTR strPath);

	void DrawLink(t3DModel *pModel);

	// 绘制模型
	void RenderModel(t3DModel *pModel);

	// 成员变量

	// 模型中的纹理
	UINT m_Textures[MAX_TEXTURES];	
	vector<string> strTextures;

	// 人物的不同部位的模型
	t3DModel m_Head;
	t3DModel m_Upper;
	t3DModel m_Lower;

	// 武器模型
	t3DModel m_Weapon;
};


#endif

