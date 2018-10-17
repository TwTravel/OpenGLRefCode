#ifndef _MD2_H
#define _MD2_H

// 定义MD2文件中的各种参数的最大值
#define MD2_MAX_TRIANGLES		4096
#define MD2_MAX_VERTICES		2048
#define MD2_MAX_TEXCOORDS		2048
#define MD2_MAX_FRAMES			512
#define MD2_MAX_SKINS			32
#define MD2_MAX_FRAMESIZE		(MD2_MAX_VERTICES * 4 + 128)

// 以下结构体保存了MD2文件的头部信息
struct tMd2Header
{ 
   int magic;					// 文件识别号
   int version;					// 文件的版本号(必须是8)
   int skinWidth;				// 皮肤的宽度(以像素为单位)
   int skinHeight;				// 皮肤的高度(以像素为单位)
   int frameSize;				// 每帧的大小(以字节为单位)
   int numSkins;				// 与模型关联的皮肤数量
   int numVertices;				// 顶点数量
   int numTexCoords;			// 纹理坐标数量
   int numTriangles;			// 面(多边形)的数量
   int numGlCommands;			// gl命令的数量
   int numFrames;				// 动画帧的数量
   int offsetSkins;				// 皮肤数据在文件中的偏移地址
   int offsetTexCoords;			// 纹理数据在文件中的偏移地址
   int offsetTriangles;			// 面数据在文件中的偏移地址
   int offsetFrames;			// 帧数据在文件中的偏移地址
   int offsetGlCommands;		// gl命令数据在文件中的偏移地址
   int offsetEnd;				// 文件结束位置在文件中的偏移地址
};


// 用于保存从当前帧中读入的顶点数据
struct tMd2AliasTriangle
{
   byte vertex[3];
   byte lightNormalIndex;
};

// 用于保存从当前帧中读入的法向量和顶点
struct tMd2Triangle
{
   float vertex[3];
   float normal[3];
};

// 用于保存顶点和纹理坐标在队列中的索引
struct tMd2Face
{
   short vertexIndices[3];
   short textureIndices[3];
};

// 用于保存UV坐标
struct tMd2TexCoord
{
   short u, v;
};

// 用于保存动画帧的缩放、平移和名称等信息，包括顶点
struct tMd2AliasFrame
{
   float scale[3];
   float translate[3];
   char name[16];
   tMd2AliasTriangle aliasVertices[1];
};

// 用于保存变换后的帧顶点数据
struct tMd2Frame
{
   char strName[16];
   tMd2Triangle *pVertices;
};

// 用于保存皮肤名称
typedef char tMd2Skin[64];


// 下面的类用于读入所有的模型数据
class CLoadMD2
{

public:
	CLoadMD2();					// 初始化数据成员

	// 从MD2文件中输入模型
	bool ImportMD2(t3DModel *pModel, char *strFileName, char *strTexture);

private:
	
	// 从MD2文件中读出数据并保存在成员变量中
	void ReadMD2Data();

	// 转换成员变量到pModel结构体中
	void ConvertDataStructures(t3DModel *pModel);

	// 计算对象的顶点法向量(用于光照)
	void ComputeNormals(t3DModel *pModel);

	// 释放内存资源，关闭文件
	void CleanUp();
	
	// 文件指针
	FILE *m_FilePointer;

	// 成员变量		
	tMd2Header				m_Header;			// 文件头数据
	tMd2Skin				*m_pSkins;			// 皮肤数据
	tMd2TexCoord			*m_pTexCoords;		// 纹理坐标
	tMd2Face				*m_pTriangles;		// 面的索引信息
	tMd2Frame				*m_pFrames;			// 动画帧信息
};


#endif


