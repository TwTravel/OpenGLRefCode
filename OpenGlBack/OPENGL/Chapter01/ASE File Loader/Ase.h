#ifndef _ASE_H
#define _ASE_H

#include "Main.h"

//  ������һЩ��Ҫ����Ase�ļ��ж���ĸ��ֱ�ǩ����Ҫ��������һЩ����ͳ���������
#define OBJECT				"*GEOMOBJECT"			// һ���¶���Ķ����ǩ
#define NUM_VERTEX			"*MESH_NUMVERTEX"		// �����ǩ������
#define NUM_FACES			"*MESH_NUMFACES"		// ���ǩ����Ŀ
#define NUM_TVERTEX			"*MESH_NUMTVERTEX"		// �������������
#define VERTEX				"*MESH_VERTEX"			// �����ǩ�б�
#define FACE				"*MESH_FACE"			// ���ǩ�б�
#define NORMALS				"*MESH_NORMALS"			// ��������ǩ�б�
#define FACE_NORMAL			"*MESH_FACENORMAL"		// ��ǰ���淨��������
#define NVERTEX				"*MESH_VERTEXNORMAL"	// ���㷨�����б�
#define TVERTEX				"*MESH_TVERT"			// ��������������ǩ
#define TFACE				"*MESH_TFACE"			// ����������ǩ
#define TEXTURE				"*BITMAP"				// ��������ӳ����ļ���
#define UTILE				"*UVW_U_TILING"			// U�ظ��ʱ�ǩ
#define VTILE				"*UVW_V_TILING"			// V�ظ��ʱ�ǩ
#define UOFFSET				"*UVW_U_OFFSET"			// Uƫ�Ʊ�ǩ
#define VOFFSET				"*UVW_V_OFFSET"			// Vƫ�Ʊ�ǩ
#define MATERIAL_ID			"*MATERIAL_REF"			// ����ID��ǩ
#define MATERIAL_COUNT      "*MATERIAL_COUNT"		// ����������ǩ
#define MATERIAL			"*MATERIAL"				// ���ʱ�ǩ
#define MATERIAL_NAME		"*MATERIAL_NAME"		// �������Ʊ�ǩ
#define MATERIAL_COLOR		"*MATERIAL_DIFFUSE"		// ������ɫ��ǩ


// �������CLoadASE�Ĺ����Ƕ���Ase�ļ���������Ϣ
class CLoadASE 
{

public:

	// ����һ��Ase�ļ������ݵ�t3DMode�ṹ����
	bool ImportASE(t3DModel *pModel, char *strFileName);

	// ����Ase�ļ�
	void ReadAseFile(t3DModel *pModel);

	// ���Ase�ļ��еĶ������Ŀ
	int GetObjectCount();

	// ���Ase�ļ��в��ʵ���Ŀ
	int GetMaterialCount();

	// ���������Ϣ
	void GetTextureInfo (tMaterialInfo *pTexture, int desiredMaterial);

	// ���ļ�ָ���ƶ�������Ķ���
	void MoveToObject (int desiredObject);

	// ���ļ��ж���һ��������
	float ReadFloat();

	// ����������Ϣ(�桢���������������)
	void ReadObjectInfo(t3DObject *pObject, int desiredObject);

	// �����������
	void GetTextureName (tMaterialInfo  *pTexture);

	// ��ò�������
	void GetMaterialName(tMaterialInfo *pTexture);

	// ��þ���������������
	void ReadObjectData(t3DModel *pModel, t3DObject *pObject, int desiredObject);

	// ��þ�������ݣ���ReadObjectData()�е���
	void GetData(t3DModel *pModel, t3DObject *pObject, char *strDesiredData, int desiredObject);

	// ���ļ��ж���һ������
	void ReadVertex(t3DObject *pObject);

	// ���ļ��ж���һ����������
	void ReadTextureVertex(t3DObject *pObject, tMaterialInfo texture);

	// ���ļ�����һ����Ķ�������
	void ReadFace(t3DObject *pObject);

	// ���ļ��ж���һ�����������������
	void ReadTextureFace(t3DObject *pObject);

	// �������Ķ��㷨����
	void ComputeNormals(t3DModel *pModel);

private:
	
	// �ļ�ָ��
	FILE *m_FilePointer;
};

#endif


