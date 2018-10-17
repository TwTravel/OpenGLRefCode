
#include "main.h"
#include "Md2.h"

//  ����ĺ����Ĺ����ǳ�ʼ����CLoadMD2�еĸ���Ա����
CLoadMD2::CLoadMD2()
{
	// ��ʼ���ṹ��
	memset(&m_Header, 0, sizeof(tMd2Header));

	// ���ø�ָ��ΪNULL
	m_pSkins=NULL;
	m_pTexCoords=NULL;
	m_pTriangles=NULL;
	m_pFrames=NULL;
}


//  ����ĺ����Ĺ����Ǵ�MD2�ļ�������MD2�ļ��е����ݣ�����ͷ��ڴ���Դ���ر��ļ�
bool CLoadMD2::ImportMD2(t3DModel *pModel, char *strFileName, char *strTexture)
{
	char strMessage[255] = {0};

	// ��ֻ����������ģʽ��MD2�ļ�
	m_FilePointer = fopen(strFileName, "rb");

	// �ж��ļ�ָ���ͷźϷ�
	if(!m_FilePointer) 
	{
		// ����Ƿ�����ʾ������Ϣ
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	// �������ļ���ʽһ������Ҫ��MD2�ļ��ж���һ���ļ�ͷ��Ϣ

	// ����ͷ����Ϣ�������䱣���ڳ�Ա����m_Header��
	fread(&m_Header, 1, sizeof(tMd2Header), m_FilePointer);

	// ����MD2��ʽ���ļ����ļ��İ汾������8
	if(m_Header.version != 8)
	{
		// �������8������ʾ������Ϣ�󷵻�
		sprintf(strMessage, "Invalid file format (Version not 8): %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	//  �����������µ�����
	ReadMD2Data();
	
	// ����ת����pModelģ�ͽṹ����
	ConvertDataStructures(pModel);

	// ��������MD2�ļ�֮���ټ���ģ�͵ķ�����
	ComputeNormals(pModel);

	// ����ļ�����һ��������������ƣ����������������
	if(strTexture)
	{
		// ����һ��������Ϣ�ṹ��
		tMaterialInfo texture;

		// ���ļ����ƿ������������Ʊ�����
		strcpy(texture.strFile, strTexture);

		// ��������MD2�ļ���ֻ��Ψһ������ID������0
		texture.texureId = 0;

		// �����UV�ظ�����1
		texture.uTile = texture.uTile = 1;

		// һ��ģ����ֻ��һ������
		pModel->numOfMaterials = 1;

		// ��Ӳ�����Ϣ��ģ�͵Ĳ����б���
		pModel->pMaterials.push_back(texture);
	}

	// �ͷ��ڴ���Դ
	CleanUp();

	// ����true
	return true;
}

//  ����ĺ��������Ƕ���ģ�͵ĳ��˶���֮֡���������������
void CLoadMD2::ReadMD2Data()
{
	// Ϊ֡��������һ���Ƚϴ�Ĵ洢�ռ�
	unsigned char buffer[MD2_MAX_FRAMESIZE];
	int j = 0;

	// Ϊ���е�ͷ����Ϣ�ṹ�����洢�ռ�
	m_pSkins     = new tMd2Skin [m_Header.numSkins];
	m_pTexCoords = new tMd2TexCoord [m_Header.numTexCoords];
	m_pTriangles = new tMd2Face [m_Header.numTriangles];
	m_pFrames    = new tMd2Frame [m_Header.numFrames];

	// ��λ�ļ�ָ����ģ�͵�Ƥ�����ݴ�
	fseek(m_FilePointer, m_Header.offsetSkins, SEEK_SET);
	
	// ����Ƥ������������ģ�͵�Ƥ������
	fread(m_pSkins, sizeof(tMd2Skin), m_Header.numSkins, m_FilePointer);
	
	// ���ļ�ָ���Ƶ��������괦
	fseek(m_FilePointer, m_Header.offsetTexCoords, SEEK_SET);
	
	// �������е���������
	fread(m_pTexCoords, sizeof(tMd2TexCoord), m_Header.numTexCoords, m_FilePointer);

	// ���ļ�ָ���ƶ���������/�����ݴ�
	fseek(m_FilePointer, m_Header.offsetTriangles, SEEK_SET);
	
	// ����ÿ�������ε�������(�������������)
	fread(m_pTriangles, sizeof(tMd2Face), m_Header.numTriangles, m_FilePointer);
			
	// ���ļ�ָ���ƶ�������֡��
	fseek(m_FilePointer, m_Header.offsetFrames, SEEK_SET);

	// ��֡���ݽṹ��ָ��ָ���ڴ滺����
	tMd2AliasFrame *pFrame = (tMd2AliasFrame *) buffer;

	// ����洢�ռ�
	m_pFrames[0].pVertices = new tMd2Triangle [m_Header.numVertices];

	// ���붯���ĵ�һ֡
	fread(pFrame, 1, m_Header.frameSize, m_FilePointer);

	// �����������ƿ�����֡������
	strcpy(m_pFrames[0].strName, pFrame->name);
			
	tMd2Triangle *pVertices = m_pFrames[0].pVertices;

	//  �������еĶ���
	for (j=0; j < m_Header.numVertices; j++)
	{
		pVertices[j].vertex[0] = pFrame->aliasVertices[j].vertex[0] * pFrame->scale[0] + pFrame->translate[0];
		pVertices[j].vertex[2] = -1 * (pFrame->aliasVertices[j].vertex[1] * pFrame->scale[1] + pFrame->translate[1]);
		pVertices[j].vertex[1] = pFrame->aliasVertices[j].vertex[2] * pFrame->scale[2] + pFrame->translate[2];
	}
}


//  ����ĺ����Ĺ����ǽ�MD2�ṹ��ת����OpenGLģ�ͽṹ��
void CLoadMD2::ConvertDataStructures(t3DModel *pModel)
{
	int j = 0, i = 0;
	
	//  ����ֻ�����˶�����һ֡�����ģ����ֻ��һ������
	pModel->numOfObjects = 1;

	// ����һ������ṹ�屣���һ֡������
	t3DObject currentFrame = {0};

	// �����㡢��������������������µĽṹ��
	currentFrame.numOfVerts   = m_Header.numVertices;
	currentFrame.numTexVertex = m_Header.numTexCoords;
	currentFrame.numOfFaces   = m_Header.numTriangles;

	// ����洢�ռ�
	currentFrame.pVerts    = new CVector3 [currentFrame.numOfVerts];
	currentFrame.pTexVerts = new CVector2 [currentFrame.numTexVertex];
	currentFrame.pFaces    = new tFace [currentFrame.numOfFaces];

	// �������еĶ��㣬�����еĶ��㸳���µĽṹ����
	for (j=0; j < currentFrame.numOfVerts; j++)
	{
		currentFrame.pVerts[j].x = m_pFrames[0].pVertices[j].vertex[0];
		currentFrame.pVerts[j].y = m_pFrames[0].pVertices[j].vertex[1];
		currentFrame.pVerts[j].z = m_pFrames[0].pVertices[j].vertex[2];
	}

	// ɾ��ʹ�õĿռ�
	delete m_pFrames[0].pVertices;

	//  �������е�UV���꣬�����е��������긳���µĽṹ����
	for (j=0; j < currentFrame.numTexVertex; j++)
	{
		currentFrame.pTexVerts[j].x = m_pTexCoords[j].u / float(m_Header.skinWidth);
		currentFrame.pTexVerts[j].y = 1 - m_pTexCoords[j].v / float(m_Header.skinHeight);
	}

	// �������е������ݣ����丳���µĽṹ����
	for(j=0; j < currentFrame.numOfFaces; j++)
	{
		// ���������ݸ����ṹ����
		currentFrame.pFaces[j].vertIndex[0] = m_pTriangles[j].vertexIndices[0];
		currentFrame.pFaces[j].vertIndex[1] = m_pTriangles[j].vertexIndices[1];
		currentFrame.pFaces[j].vertIndex[2] = m_pTriangles[j].vertexIndices[2];

		// ���������긳���ṹ����
		currentFrame.pFaces[j].coordIndex[0] = m_pTriangles[j].textureIndices[0];
		currentFrame.pFaces[j].coordIndex[1] = m_pTriangles[j].textureIndices[1];
		currentFrame.pFaces[j].coordIndex[2] = m_pTriangles[j].textureIndices[2];
	}

	// ����ǰ�Ķ�����ӵ������б���
	pModel->pObject.push_back(currentFrame);
}


//  ����ĺ������������ռ�õ��ڴ�ռ䣬���ر��ļ�
void CLoadMD2::CleanUp()
{
	//  �ر��ļ�
	fclose(m_FilePointer);						

	if(m_pSkins)	 delete [] m_pSkins;		// ɾ��Ƥ������
	if(m_pTexCoords) delete m_pTexCoords;		// ɾ��Ƥ����������
	if(m_pTriangles) delete m_pTriangles;		// ɾ��������������
	if(m_pFrames)	 delete m_pFrames;			// ɾ������֡����
}



// ����ĺ궨�����һ��ʸ���ĳ���
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))

// ����ĺ��������������ʸ��
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2)
{
	CVector3 vVector;							

	vVector.x = vPoint1.x - vPoint2.x;			
	vVector.y = vPoint1.y - vPoint2.y;			
	vVector.z = vPoint1.z - vPoint2.z;			

	return vVector;								
}


// ����ĺ�������ʸ�����
CVector3 AddVector(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vResult;							
	
	vResult.x = vVector2.x + vVector1.x;		
	vResult.y = vVector2.y + vVector1.y;		
	vResult.z = vVector2.z + vVector1.z;		

	return vResult;								
}

// ����ĺ�������ʸ��������
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler)
{
	CVector3 vResult;							
	
	vResult.x = vVector1.x / Scaler;			
	vResult.y = vVector1.y / Scaler;			
	vResult.z = vVector1.z / Scaler;			

	return vResult;								
}

// ����ĺ�����������ʸ���Ĳ��
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vCross;								
												
	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
												
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
												
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vCross;								
}


// ����ĺ����淶��ʸ��
CVector3 Normalize(CVector3 vNormal)
{
	double Magnitude;							

	Magnitude = Mag(vNormal);					// ���ʸ���ĳ���

	vNormal.x /= (float)Magnitude;				
	vNormal.y /= (float)Magnitude;				
	vNormal.z /= (float)Magnitude;				

	return vNormal;								
}

//  ����ĺ������ڼ������ķ�����
void CLoadMD2::ComputeNormals(t3DModel *pModel)
{
	CVector3 vVector1, vVector2, vNormal, vPoly[3];

	// ���ģ����û�ж����򷵻�
	if(pModel->numOfObjects <= 0)
		return;

	// ����ģ�������еĶ���
	for(int index = 0; index < pModel->numOfObjects; index++)
	{
		// ��õ�ǰ�Ķ���
		t3DObject *pObject = &(pModel->pObject[index]);

		// ������Ҫ�Ĵ洢�ռ�
		CVector3 *pNormals		= new CVector3 [pObject->numOfFaces];
		CVector3 *pTempNormals	= new CVector3 [pObject->numOfFaces];
		pObject->pNormals		= new CVector3 [pObject->numOfVerts];

		// ���������������
		for(int i=0; i < pObject->numOfFaces; i++)
		{												
			vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
			vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
			vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];

			// ������ķ�����

			vVector1 = Vector(vPoly[0], vPoly[2]);		// ��ö���ε�ʸ��
			vVector2 = Vector(vPoly[2], vPoly[1]);		// ��ö���εĵڶ���ʸ��

			vNormal  = Cross(vVector1, vVector2);		// �������ʸ���Ĳ��
			pTempNormals[i] = vNormal;					// ����ǹ淶��������
			vNormal  = Normalize(vNormal);				// �淶����õĲ��

			pNormals[i] = vNormal;						// ����������ӵ��������б���
		}

		//  �����󶥵㷨����
		CVector3 vSum = {0.0, 0.0, 0.0};
		CVector3 vZero = vSum;
		int shared=0;
		// �������еĶ���
		for (i = 0; i < pObject->numOfVerts; i++)			
		{
			for (int j = 0; j < pObject->numOfFaces; j++)	// �������е���������
			{												// �жϸõ��Ƿ����������湲��
				if (pObject->pFaces[j].vertIndex[0] == i || 
					pObject->pFaces[j].vertIndex[1] == i || 
					pObject->pFaces[j].vertIndex[2] == i)
				{
					vSum = AddVector(vSum, pTempNormals[j]);
					shared++;								
				}
			}      
			
			pObject->pNormals[i] = DivideVectorByScaler(vSum, float(-shared));

			// �淶�����Ķ��㷨��
			pObject->pNormals[i] = Normalize(pObject->pNormals[i]);	

			vSum = vZero;								
			shared = 0;										
		}
	
		// �ͷŴ洢�ռ䣬��ʼ��һ������
		delete [] pTempNormals;
		delete [] pNormals;
	}
}


