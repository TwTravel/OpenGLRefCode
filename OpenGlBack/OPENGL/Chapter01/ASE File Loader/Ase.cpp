
#include "Main.h"
#include "Ase.h"


//  ����ĺ��������ǽ�ASEģ�Ͷ��뵽t3DModelģ�ͽṹ����
bool CLoadASE::ImportASE(t3DModel *pModel, char *strFileName)
{
	char strMessage[255] = {0};				// ������ʾ������Ϣ

	// �ж��Ƿ���һ���Ϸ���ģ�ͺ��ļ�����
	if(!pModel || !strFileName) return false;

	// ��ֻ����ʽ���ļ��������ļ�ָ��
	m_FilePointer = fopen(strFileName, "r");

	// �ж��ļ�ָ���Ƿ���ȷ
	if(!m_FilePointer) {
		// ����ļ�ָ�벻��ȷ������ʾ������Ϣ
		sprintf(strMessage, "Unable to find or open the file: %s", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// �����ļ���Ϣ
	ReadAseFile(pModel);

	// ���㶥��ķ����������ڹ���
	ComputeNormals(pModel);

	// �رմ򿪵��ļ�
	fclose(m_FilePointer);

	return true;
}


//  ����ĺ�����������һ��Ase�ļ�
void CLoadASE::ReadAseFile(t3DModel *pModel)
{
	tMaterialInfo newMaterial = {0};		// �²���
	t3DObject     newObject   = {0};		// �¶���
	int i = 0;	

	// ���ase�ļ��ж��������
	pModel->numOfObjects   = GetObjectCount();

	// ���ase�ļ��в��ʵ�����
	pModel->numOfMaterials = GetMaterialCount();

	// �������еĲ���
	for(i = 0; i < pModel->numOfMaterials; i++)
	{
		// �ڲ������������һ���µĲ���
		pModel->pMaterials.push_back(newMaterial);

		// ��õ�ǰ���ʵĲ�����Ϣ
		GetTextureInfo(&(pModel->pMaterials[i]), i + 1);
	}

	// �������еĶ���
	for(i = 0; i < pModel->numOfObjects; i++)
	{	
		// �ڶ������������һ���µĶ���
		pModel->pObject.push_back(newObject);

		// ��ʼ������ID������Ϊ-1
		pModel->pObject[i].materialID = -1;

		// ���ļ�ָ���ƶ���ָ���Ķ���
		MoveToObject(i + 1);

		// ȷ������Ķ������������������������������Ȼ������㹻���ڴ�ռ�
		ReadObjectInfo(&(pModel->pObject[i]), i + 1);
		
		// �������Ķ��㡢���������������
		ReadObjectData(pModel, &(pModel->pObject[i]), i + 1);
	}
}

//  ����ĺ����Ĺ����ǻ��ase�ļ��ж��������
int CLoadASE::GetObjectCount()
{
	char strWord[255] = {0};
	int objectCount = 0;

	// ���ļ�ָ��Ķ�λ���ļ��Ŀ�ʼ
	rewind(m_FilePointer);

	// ���������ļ�
	while (!feof(m_FilePointer))
	{
		// ����ļ��е�ÿ���ַ���
		fscanf(m_FilePointer, "%s", &strWord);

		// �ж��Ƿ��Ƕ���
		if (!strcmp(strWord, OBJECT))
		{
			// ���Ӷ�������� 
			objectCount++;
		}
		else
		{
			// ������һ��
			fgets(strWord, 100, m_FilePointer);
		}
	}

	// ����ase�ļ��еĶ�������
	return objectCount;
}

//  ����ĺ��������ǻ��ase�ļ��еĲ���
int CLoadASE::GetMaterialCount()
{
	char strWord[255] = {0};
	int materialCount = 0;

	// ���ļ�ָ�붨λ���ļ���ʼ��
	rewind(m_FilePointer);

	// ���������ļ�
	while (!feof(m_FilePointer))
	{
		// ����ļ��е�ÿ���ַ���
		fscanf(m_FilePointer, "%s", &strWord);

		// �ж��Ƿ��Ƕ���Ĳ�������
		if (!strcmp(strWord, MATERIAL_COUNT))
		{
			// ���������
			fscanf(m_FilePointer, "%d", &materialCount);

			// ���ز�����
			return materialCount;
		}
		else
		{
			// ������һ��
			fgets(strWord, 100, m_FilePointer);
		}
	}

	// ���û�в��ʣ��򷵻�0
	return 0;
}

//  ���ָ�����ʵĲ�����Ϣ
void CLoadASE::GetTextureInfo (tMaterialInfo *pTexture, int desiredMaterial)
{
	char strWord[255] = {0};
	int materialCount= 0;
	
	// ���ļ�ָ�붨λ���ļ���ʼ��
	rewind(m_FilePointer);

	// ���������ļ�
	while (!feof(m_FilePointer))
	{
		// ����ļ���ÿ���ַ���
		fscanf(m_FilePointer, "%s", &strWord);

		// �ж��Ƿ��ǲ���
		if (!strcmp(strWord, MATERIAL))
		{
			// ���Ӳ��ʼ����� 
			materialCount++;

			// �жϲ��ʼ������Ƿ������Ĳ���ƥ��
			if(materialCount == desiredMaterial)
				break;
		}
		else
		{
			// ������һ��
			fgets(strWord, 100, m_FilePointer);
		}
	}

	// ���ڽ�������Ҫ����Ĳ���

	// �������µ��ļ���ֱ������
	while (!feof(m_FilePointer))
	{
		// ����ļ��е�ÿ���ַ���
		fscanf(m_FilePointer, "%s", &strWord);

		// �ж��Ƿ��ǲ��ʱ�ǩ
		if (!strcmp (strWord, MATERIAL))
		{
			// �µĲ��ʣ��򷵻�
			return;
		}
		// �����MATERIAL_COLOR��ǩ����Ҫ��ò��ʵ���ɫ
		else if (!strcmp(strWord, MATERIAL_COLOR))
		{
			// ��ö�����ʵ�RGB��ɫ
			fscanf(m_FilePointer, " %f %f %f", &(pTexture->fColor[0]), 
											   &(pTexture->fColor[1]), 
											   &(pTexture->fColor[2]));
		}
		// �����TEXTURE��ǩ����Ҫ�����������
		else if (!strcmp(strWord, TEXTURE))
		{
			// ���������ļ���
			GetTextureName(pTexture);
		}
		// �����MATERIAL_NAME��ǩ������Ҫ��ò��ʵ�����
		else if (!strcmp(strWord, MATERIAL_NAME))
		{
			// ��ö���Ĳ�������
			GetMaterialName(pTexture);
		}
		// �����UTILE��ǩ������Ҫ���U�ظ���
		else if(!strcmp(strWord, UTILE))
		{
			// ����U���������ϵ�U�ظ���
			pTexture->uTile = ReadFloat();
		}
		// �����VTILE��ǩ������Ҫ���V�ظ���
		else if(!strcmp(strWord, VTILE))
		{
			// ����V���������ϵ�V�ظ���
			pTexture->vTile = ReadFloat();
		}
		// �����Թ���Щ����
		else
		{
			// ������һ��
			fgets (strWord, 100, m_FilePointer);
		}
	}
}

//  ����ĺ��������ǽ��ļ�ָ���Ƶ�Ase�ļ���ָ���Ķ���
void CLoadASE::MoveToObject (int desiredObject)
{
	char strWord[255] = {0};
	int objectCount = 0;

	// ���ļ�ָ�붨λ���ļ��Ŀ�ʼ
	rewind(m_FilePointer);

	// ���������ļ���ֱ���ļ�����
	while(!feof(m_FilePointer))
	{
		// ����ļ�ÿһ���ַ���
		fscanf(m_FilePointer, "%s", &strWord);

		// �ж��Ƿ���һ������
		if(!strcmp(strWord, OBJECT))
		{
			// ���Ӷ�������� 
			objectCount++;

			// �����ָ���Ķ���
			if(objectCount == desiredObject)
				return;
		}
		else
		{
			// ������һ��
			fgets(strWord, 100, m_FilePointer);
		}
	}
}

//  ����ĺ��������Ǵ�Ase�ļ��ж���һ��������
float CLoadASE::ReadFloat()
{
	float v = 0.0f;

	// ����һ��������
	fscanf(m_FilePointer, " %f", &v);

	// ���ظø�����
	return v;
}


//  ����ĺ����Ĺ����Ƕ���������Ϣ
void CLoadASE::ReadObjectInfo(t3DObject *pObject, int desiredObject)
{
	char strWord[255] = {0};

	// ���ļ�ָ���ƶ���ָ���Ķ���
	MoveToObject(desiredObject);
	
	// ֱ���ļ�β�Ž���
	while (!feof(m_FilePointer))
	{
		// ���ļ��ж���ÿ���ַ���
		fscanf(m_FilePointer, "%s", &strWord);

		// ����Ƕ���������ǩ
		if (!strcmp(strWord, NUM_VERTEX))
		{
			// �������Ķ�������
			fscanf(m_FilePointer, "%d", &pObject->numOfVerts);

			// ���䱣�涥����ڴ�ռ�
			pObject->pVerts = new CVector3 [pObject->numOfVerts];
		}
		// ����������Ŀ��ǩ
		else if (!strcmp(strWord, NUM_FACES))
		{
			// ��������������Ŀ
			fscanf(m_FilePointer, "%d", &pObject->numOfFaces);

			// ���䱣������ڴ�ռ�
			pObject->pFaces = new tFace [pObject->numOfFaces];
		}
		// ���������������Ŀ
		else if (!strcmp(strWord, NUM_TVERTEX))
		{
			// ������������������Ŀ
			fscanf(m_FilePointer, "%d", &pObject->numTexVertex);

			// ���䱣��UV������ڴ�ռ�
			pObject->pTexVerts = new CVector2 [pObject->numTexVertex];
		}
		// ����Ƕ����ǩ�����������һ������
		else if (!strcmp(strWord, OBJECT))	
		{
			// ����
			return;
		}
		else 
		{
			// û���ҵ��κζ������������һ��
			fgets(strWord, 100, m_FilePointer);
		}
	}	
}


//  ����ĺ��������Ƕ������������ļ�����
void CLoadASE::GetTextureName(tMaterialInfo *pTexture)
{
	// ����������ļ���
	fscanf (m_FilePointer, " \"%s", &(pTexture->strFile));
	
	// ���ַ�����ĩβ����һ��NULL�ַ�
	pTexture->strFile[strlen (pTexture->strFile) - 1] = '\0';
}

//  ����ĺ��������Ƕ������Ĳ�������
void CLoadASE::GetMaterialName(tMaterialInfo *pTexture)
{
	// �����������
	fscanf (m_FilePointer, " \"%s", &(pTexture->strName));
	
	// ���ַ�����ĩβ����NULL�ַ�
	pTexture->strName[strlen (pTexture->strName)] = '\0';
}

//  ����ĺ��������Ƕ���ָ������ľ�������
void CLoadASE::ReadObjectData(t3DModel *pModel, t3DObject *pObject, int desiredObject)
{

	// �������Ĳ���ID
	GetData(pModel, pObject, MATERIAL_ID, desiredObject);

	// �������Ķ�����
	GetData(pModel, pObject, VERTEX,		 desiredObject);

	// ����������������
	GetData(pModel, pObject, TVERTEX,	 desiredObject);

	// ��������������
	GetData(pModel, pObject, FACE,		 desiredObject);

	// ������������������
	GetData(pModel, pObject, TFACE,		 desiredObject);

	// ����������������
	GetData(pModel, pObject, TEXTURE,	 desiredObject);

	// ��������U�ظ�
	GetData(pModel, pObject, UTILE,		 desiredObject);

	// ��������V�ظ�
	GetData(pModel, pObject, VTILE,		 desiredObject);
}

//  ����ĺ��������ǻ�þ��������
void CLoadASE::GetData(t3DModel *pModel, t3DObject *pObject, char *strDesiredData, int desiredObject)
{
	char strWord[255] = {0};

	// ���ļ�ָ���ƶ���ָ���Ķ���
	MoveToObject(desiredObject);

	// ���������ļ�ֱ���ļ�����
	while(!feof(m_FilePointer))
	{
		// ����ÿһ���ַ���
		fscanf(m_FilePointer, "%s", &strWord);

		// �ж��Ƿ��Ƕ���
		if(!strcmp(strWord, OBJECT))	
		{
			// ����
			return;
		}
		// ����Ƕ����ǩ
		else if(!strcmp(strWord, VERTEX))
		{
			// ȷ����ϣ�����������
			if(!strcmp(strDesiredData, VERTEX)) 
			{
				// ���붥������
				ReadVertex(pObject);
			}
		}
		// �������������
		else if(!strcmp(strWord, TVERTEX))
		{
			// ȷ����ϣ�����������
			if(!strcmp(strDesiredData, TVERTEX)) 
			{
				// ������������
				ReadTextureVertex(pObject, pModel->pMaterials[pObject->materialID]);
			}
		}
		// �������Ķ�������
		else if(!strcmp(strWord, FACE))
		{
			// ȷ����ϣ�����������
			if(!strcmp(strDesiredData, FACE)) 
			{
				// ����������
				ReadFace(pObject);
			}
		}
		// ��������������������
		else if(!strcmp(strWord, TFACE))
		{
			// ȷ����ϣ�����������
			if(!strcmp(strDesiredData, TFACE))
			{
				// ���������������
				ReadTextureFace(pObject);
			}
		}
		// ����Ƕ���Ĳ���ID
		else if(!strcmp(strWord, MATERIAL_ID))
		{
			// ȷ����ϣ�����������
			if(!strcmp(strDesiredData, MATERIAL_ID))
			{
				// �������Ĳ���ID
				pObject->materialID = (int)ReadFloat();
				return;
			}				
		}
		else 
		{
			// ������һ��
			fgets(strWord, 100, m_FilePointer);
		}
	}
}


//  �������Ķ�������
void CLoadASE::ReadVertex(t3DObject *pObject)
{
	int index = 0;

	// �Թ���������
	fscanf(m_FilePointer, "%d", &index);
	
	fscanf(m_FilePointer, "%f %f %f", &pObject->pVerts[index].x, 
									  &pObject->pVerts[index].z,
									  &pObject->pVerts[index].y);
		
	pObject->pVerts[index].z = -pObject->pVerts[index].z;
}


//  ������������
void CLoadASE::ReadTextureVertex(t3DObject *pObject, tMaterialInfo texture)
{
	int index = 0;

	// �Թ����������������
	fscanf(m_FilePointer, "%d", &index);

	// ������������(U, V)
	fscanf(m_FilePointer, "%f %f", &(pObject->pTexVerts[index].x), &(pObject->pTexVerts[index].y));

	pObject->pTexVerts[index].x *= texture.uTile;
	pObject->pTexVerts[index].y *= texture.vTile;

	// �ñ�־λΪtrue
	pObject->bHasTexture = true;
}


//  ������Ķ�������
void CLoadASE::ReadFace(t3DObject *pObject)
{
	int index = 0;

	// �Թ��������
	fscanf(m_FilePointer, "%d:", &index);

	// ������Ķ�������
	fscanf(m_FilePointer, "\tA:\t%d B:\t%d C:\t%d", &(pObject->pFaces[index].vertIndex[0]), 
													&(pObject->pFaces[index].vertIndex[1]), 
													&(pObject->pFaces[index].vertIndex[2])); 
}


//  ���������������
void CLoadASE::ReadTextureFace(t3DObject *pObject)
{
	int index = 0;

	// �Թ�������������
	fscanf(m_FilePointer, "%d:", &index);

	// ���뵱ǰ���UV��������
	fscanf(m_FilePointer, "%d %d %d", &pObject->pFaces[index].coordIndex[0], 
									  &pObject->pFaces[index].coordIndex[1], 
									  &pObject->pFaces[index].coordIndex[2]);
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
void CLoadASE::ComputeNormals(t3DModel *pModel)
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



