#ifndef _ASE_H
#define _ASE_H

#include "Main.h"

//  下面是一些重要的在Ase文件中定义的各种标签，主要用来定义一些对象和场景的数据
#define OBJECT				"*GEOMOBJECT"			// 一个新对象的对象标签
#define NUM_VERTEX			"*MESH_NUMVERTEX"		// 顶点标签的数量
#define NUM_FACES			"*MESH_NUMFACES"		// 面标签的数目
#define NUM_TVERTEX			"*MESH_NUMTVERTEX"		// 纹理坐标的数量
#define VERTEX				"*MESH_VERTEX"			// 顶点标签列表
#define FACE				"*MESH_FACE"			// 面标签列表
#define NORMALS				"*MESH_NORMALS"			// 法向量标签列表
#define FACE_NORMAL			"*MESH_FACENORMAL"		// 当前的面法向量索引
#define NVERTEX				"*MESH_VERTEXNORMAL"	// 顶点法向量列表
#define TVERTEX				"*MESH_TVERT"			// 纹理坐标索引标签
#define TFACE				"*MESH_TFACE"			// 顶点索引标签
#define TEXTURE				"*BITMAP"				// 对象纹理映射的文件名
#define UTILE				"*UVW_U_TILING"			// U重复率标签
#define VTILE				"*UVW_V_TILING"			// V重复率标签
#define UOFFSET				"*UVW_U_OFFSET"			// U偏移标签
#define VOFFSET				"*UVW_V_OFFSET"			// V偏移标签
#define MATERIAL_ID			"*MATERIAL_REF"			// 材质ID标签
#define MATERIAL_COUNT      "*MATERIAL_COUNT"		// 材质数量标签
#define MATERIAL			"*MATERIAL"				// 材质标签
#define MATERIAL_NAME		"*MATERIAL_NAME"		// 材质名称标签
#define MATERIAL_COLOR		"*MATERIAL_DIFFUSE"		// 材质颜色标签


// 下面的类CLoadASE的功能是读出Ase文件中所有信息
class CLoadASE 
{

public:

	// 读入一个Ase文件的数据到t3DMode结构体中
	bool ImportASE(t3DModel *pModel, char *strFileName);

	// 读入Ase文件
	void ReadAseFile(t3DModel *pModel);

	// 获得Ase文件中的对象的数目
	int GetObjectCount();

	// 获得Ase文件中材质的数目
	int GetMaterialCount();

	// 获得纹理信息
	void GetTextureInfo (tMaterialInfo *pTexture, int desiredMaterial);

	// 将文件指针移动到具体的对象
	void MoveToObject (int desiredObject);

	// 从文件中读入一个浮点数
	float ReadFloat();

	// 读入对象的信息(面、顶点和纹理坐标数)
	void ReadObjectInfo(t3DObject *pObject, int desiredObject);

	// 获得纹理名称
	void GetTextureName (tMaterialInfo  *pTexture);

	// 获得材质名称
	void GetMaterialName(tMaterialInfo *pTexture);

	// 获得具体对象的所有数据
	void ReadObjectData(t3DModel *pModel, t3DObject *pObject, int desiredObject);

	// 获得具体的数据，在ReadObjectData()中调用
	void GetData(t3DModel *pModel, t3DObject *pObject, char *strDesiredData, int desiredObject);

	// 从文件中读出一个顶点
	void ReadVertex(t3DObject *pObject);

	// 从文件中读出一个纹理坐标
	void ReadTextureVertex(t3DObject *pObject, tMaterialInfo texture);

	// 从文件读出一个面的顶点索引
	void ReadFace(t3DObject *pObject);

	// 从文件中读出一个面的纹理坐标索引
	void ReadTextureFace(t3DObject *pObject);

	// 计算对象的顶点法向量
	void ComputeNormals(t3DModel *pModel);

private:
	
	// 文件指针
	FILE *m_FilePointer;
};

#endif


