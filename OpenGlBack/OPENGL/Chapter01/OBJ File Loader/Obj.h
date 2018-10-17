#ifndef _OBJ_H
#define _OBJ_H

#include "main.h"

// �����Ƕ���obj�ļ�����
class CLoadObj 
{
public:
	//  ��obj�ļ��е���Ϣ���뵽ģ����
	bool ImportObj(t3DModel *pModel, char *strFileName);

	// ���������ImportObj()�н����øú���
	void ReadObjFile(t3DModel *pModel);

	// ���붥����Ϣ����ReadObjFile()�е��øú���
	void ReadVertexInfo();

	// ��������Ϣ����ReadObjFile()�е��øú���
	void ReadFaceInfo();

	// �������Ϣ�Ķ���֮����øú���
	void FillInObjectInfo(t3DModel *pModel);

	// ���㶥��ķ�����
	void ComputeNormals(t3DModel *pModel);

	// ������obj�ļ���û������/�������ƣ�ֻ���ֶ�����
	// materialID����ģ�Ͳ����б��е�������
	void SetObjectMaterial(t3DModel *pModel, int whichObject, int materialID);

	//  ��Ӳ���
	void AddMaterial(t3DModel *pModel, char *strName, char *strFile, 
					 int r = 255,      int g = 255,   int b = 255);

private:

	// ��Ҫ������ļ�ָ��
	FILE *m_FilePointer;

	// ��������
	vector<CVector3>  m_pVertices;

	// ������
	vector<tFace> m_pFaces;

	// UV��������
	vector<CVector2>  m_pTextureCoords;

	// ��ǰ�����Ƿ������������
	bool m_bObjectHasUV;

	// ��ǰ����Ķ����Ƿ�����
	bool m_bJustReadAFace;
};


#endif
