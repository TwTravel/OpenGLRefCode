
#include "main.h"
#include "Md3.h"

//  ����ĺ��������ж�һ���ַ����Ƿ�����һ���ַ�����
bool IsInString(string strString, string strSubString)
{
	// �ж������ַ����Ƿ�Ϸ�
	if(strString.length() <= 0 || strSubString.length() <= 0) return false;

	// ������ַ�����λ��
	int index = strString.find(strSubString);

	// �ж������Ƿ�Ϸ�
	if(index >= 0 && index < strString.length())
		return true;

	// �����ڣ��򷵻�false
	return false;
}

//  ������CModelMD3��Ĺ��캯��
CModelMD3::CModelMD3()
{
	// ��ʼ����Ա����
	memset(&m_Head,  0, sizeof(t3DModel));
	memset(&m_Upper, 0, sizeof(t3DModel));
	memset(&m_Lower, 0, sizeof(t3DModel));
	memset(&m_Weapon, 0, sizeof(t3DModel));
}

//  ������CModelMD3�����������
CModelMD3::~CModelMD3()
{
	// �ͷ���ռ�õ��ڴ���Դ
	DestroyModel(&m_Head);
	DestroyModel(&m_Upper);
	DestroyModel(&m_Lower);
	DestroyModel(&m_Weapon);
}	


//  ����ĺ����Ĺ������ͷ�ģ����ռ�õ��ڴ���Դ
void CModelMD3::DestroyModel(t3DModel *pModel)
{
	// �ͷ�һ��MD3ģ��ռ�õ���Դ��������������е��Ӷ��󣬲�����ģ������ɾ��
	// ����ģ�������еĶ���
	for(int i = 0; i < pModel->numOfObjects; i++)
	{
		// ɾ���桢���������������������
		if(pModel->pObject[i].pFaces)		delete [] pModel->pObject[i].pFaces;
		if(pModel->pObject[i].pNormals)		delete [] pModel->pObject[i].pNormals;
		if(pModel->pObject[i].pVerts)		delete [] pModel->pObject[i].pVerts;
		if(pModel->pObject[i].pTexVerts)	delete [] pModel->pObject[i].pTexVerts;
	}

	// ɾ����ģ����ص����б�ǩ
	if(pModel->pTags)		delete [] pModel->pTags;
	if(pModel->pLinks)		free(pModel->pLinks);
}
	

//  ����ĺ����Ĺ�����װ��MD3ģ��
bool CModelMD3::LoadModel(LPSTR strPath, LPSTR strModel)
{
	char strLowerModel[255] = {0};	// lower.md3��ģ���ļ���
	char strUpperModel[255] = {0};	// upper.md3��ģ���ļ���
	char strHeadModel[255]  = {0};	// head.md3��ģ���ļ���
	char strLowerSkin[255]  = {0};	// lower.md3��Ƥ���ļ���
	char strUpperSkin[255]  = {0};	// upper.md3��Ƥ���ļ���
	char strHeadSkin[255]   = {0};	// head.md3��Ƥ���ļ���
	CLoadMD3 loadMd3;				// װ��ÿ��md3��skin�ļ�

	// �ж�·���Ƿ�Ϸ�
	if(!strPath || !strModel) return false;

	// ������ȷ��ģ���ļ�����
	sprintf(strLowerModel, "%s\\%s_lower.md3", strPath, strModel);
	sprintf(strUpperModel, "%s\\%s_upper.md3", strPath, strModel);
	sprintf(strHeadModel,  "%s\\%s_head.md3",  strPath, strModel);
	
	// ������ȷ��Ƥ���ļ�����
	sprintf(strLowerSkin, "%s\\%s_lower.skin", strPath, strModel);
	sprintf(strUpperSkin, "%s\\%s_upper.skin", strPath, strModel);
	sprintf(strHeadSkin,  "%s\\%s_head.skin",  strPath, strModel);
	
	// װ��ͷ������
	if(!loadMd3.ImportMD3(&m_Head,  strHeadModel))
	{
		// ��ʾ������Ϣ
		MessageBox(g_hWnd, "Unable to load the HEAD model!", "Error", MB_OK);
		return false;
	}

	// װ���ϰ벿������
	if(!loadMd3.ImportMD3(&m_Upper, strUpperModel))		
	{
		// ��ʾ������Ϣ
		MessageBox(g_hWnd, "Unable to load the UPPER model!", "Error", MB_OK);
		return false;
	}

	// װ���°벿������
	if(!loadMd3.ImportMD3(&m_Lower, strLowerModel))
	{
		// ��ʾ������Ϣ
		MessageBox(g_hWnd, "Unable to load the LOWER model!", "Error", MB_OK);
		return false;
	}

	// װ���°벿��Ƥ��
	if(!loadMd3.LoadSkin(&m_Lower, strLowerSkin))
	{
		// ��ʾ������Ϣ
		MessageBox(g_hWnd, "Unable to load the LOWER skin!", "Error", MB_OK);
		return false;
	}

	// װ���ϰ벿��Ƥ��
	if(!loadMd3.LoadSkin(&m_Upper, strUpperSkin))
	{
		// ��ʾ������Ϣ
		MessageBox(g_hWnd, "Unable to load the UPPER skin!", "Error", MB_OK);
		return false;
	}

	// װ��ͷ��Ƥ��
	if(!loadMd3.LoadSkin(&m_Head,  strHeadSkin))
	{
		// ��ʾ������Ϣ
		MessageBox(g_hWnd, "Unable to load the HEAD skin!", "Error", MB_OK);
		return false;
	}

	// װ���°벿�֡��ϰ벿�ֺ�ͷ������  
	LoadModelTextures(&m_Lower, strPath);
	LoadModelTextures(&m_Upper, strPath);
	LoadModelTextures(&m_Head,  strPath);

	// ����������ϰ벿�ֺ��°벿��
	LinkModel(&m_Lower, &m_Upper, "tag_torso");

	// ����������ϰ벿�ֺ�ͷ��
	LinkModel(&m_Upper, &m_Head, "tag_head");
		
	return true;
}


//  ����ĺ����Ĺ�����װ������ģ��
bool CModelMD3::LoadWeapon(LPSTR strPath, LPSTR strModel)
{
	char strWeaponModel[255]  = {0};	// ����ģ�͵��ļ�����
	char strWeaponShader[255] = {0};	// ����ģ�͵��ⲿ������
	CLoadMD3 loadMd3;					// װ������ģ���븲����

	// �ж�·���Ƿ�Ϸ�
	if(!strPath || !strModel) return false;

	// ����·����ģ���ļ���
	sprintf(strWeaponModel, "%s\\%s.md3", strPath, strModel);
	
	// װ����������ģ��
	if(!loadMd3.ImportMD3(&m_Weapon,  strWeaponModel))
	{
		// ��ʾ������Ϣ
		MessageBox(g_hWnd, "Unable to load the WEAPON model!", "Error", MB_OK);
		return false;
	}

	// ���������ⲿ������·��
	sprintf(strWeaponShader, "%s\\%s.shader", strPath, strModel);

	// װ���������ⲿ������
	if(!loadMd3.LoadShader(&m_Weapon, strWeaponShader))
	{
		// ��ʾ������Ϣ
		MessageBox(g_hWnd, "Unable to load the SHADER file!", "Error", MB_OK);
		return false;
	}

	//  װ������������  
	LoadModelTextures(&m_Weapon, strPath);

	// ����������ģ�͵��ϰ벿��(��)
	LinkModel(&m_Upper, &m_Weapon, "tag_weapon");
		
	// ����true
	return true;
}

//  ����ĺ���������װ��ģ�͵�����
void CModelMD3::LoadModelTextures(t3DModel *pModel, LPSTR strPath)
{
	// ����ģ���е����еĲ���
	for(int i = 0; i < pModel->numOfMaterials; i++)
	{
		// �ж��ڲ������Ƿ���һ���ļ���
		if(strlen(pModel->pMaterials[i].strFile) > 0)
		{
			// ����������ı�ʶΪtrue
			bool bNewTexture = true;

			// �������е�����
			for(int j = 0; j < strTextures.size(); j++)
			{
				// ��������Ѿ����ڣ����Թ�
				if(!strcmp(pModel->pMaterials[i].strFile, strTextures[j].c_str()) )
				{
					// �����ʶ����Ϊfalse
					bNewTexture = false;

					//  ��������ID��
					pModel->pMaterials[i].texureId = j;
				}
			}

			// ��������ʶΪfalse�������
			if(bNewTexture == false) continue;
			
			char strFullPath[255] = {0};

			// ����������ļ�����·��
			sprintf(strFullPath, "%s\\%s", strPath, pModel->pMaterials[i].strFile);

			//  �����ļ�����������
			CreateTexture(m_Textures[strTextures.size()], strFullPath);								

			// ��������ID
			pModel->pMaterials[i].texureId = strTextures.size();

			strTextures.push_back(pModel->pMaterials[i].strFile);
		}
	}
}


//  ����ĺ�������������ģ��
void  CModelMD3::LinkModel(t3DModel *pModel, t3DModel *pLink, LPSTR strTagName)
{
	// ȷ��ģ�ͺϷ�
	if(!pModel || !pLink || !strTagName) return;

	// �������еı�ǩ
	for(int i = 0; i < pModel->numOfTags; i++)
	{
		// �����ǰ�ı�ǩ�����������ҵı�ǩ��
		if( !_stricmp(pModel->pTags[i].strName, strTagName) )
		{
			// ����ģ��
			pModel->pLinks[i] = pLink;
			return;
		}
	}
}


//  ����ĺ��������ǻ���ģ�ͣ�����һ���ݹ麯��
void CModelMD3::DrawModel()
{

	// ��תģ��
	glRotatef(-90, 1, 0, 0);

	// ���Ƶ�һ������
	DrawLink(&m_Lower);
}


//  ����ĺ����Ĺ����ǻ������ӣ�����һ���ݹ麯��
void CModelMD3::DrawLink(t3DModel *pModel)
{
	// ��Ⱦ��ǰ��ģ��(��ʼλ���ǽ�)
	RenderModel(pModel);

	// �������е�ģ�ͱ�ǩ��������
	for(int i = 0; i < pModel->numOfTags; i++)
	{
		// ��õ�ǰ������
		t3DModel *pLink = pModel->pLinks[i];

		// ������ӺϷ�
		if(pLink)
		{			
			// ��ģ�͵ı任λ�� 
			CVector3 vPosition = pModel->pTags[i].vPosition;

			// ѹ�����
			glPushMatrix();
			
				// ƽ�Ʊ任
				glTranslatef(vPosition.x, vPosition.y, vPosition.z);

				//  ��������
				DrawLink(pLink);

			// ��������
			glPopMatrix();
		}
	}

}


//  ����ĺ�������������Ļ����Ⱦģ��
void CModelMD3::RenderModel(t3DModel *pModel)
{
	// ȷ������Ϸ�
	if(pModel->pObject.size() <= 0) return;

	// ����ģ�������еĶ���
	for(int i = 0; i < pModel->numOfObjects; i++)
	{
		// ��õ�ǰ��ʾ�Ķ���
		t3DObject *pObject = &pModel->pObject[i];

		//  ��������������
		if(pObject->bHasTexture)
		{
			// ��������ӳ��
			glEnable(GL_TEXTURE_2D);

			// �������ID
			int textureID = pModel->pMaterials[pObject->materialID].texureId;

			// ��������
			glBindTexture(GL_TEXTURE_2D, m_Textures[textureID]);
		}
		else
		{
			// �ر�����ӳ��
			glDisable(GL_TEXTURE_2D);
		}

		// ��ʼ����ģ��������
		glBegin(GL_TRIANGLES);

			// �������еĶ������
			for(int j = 0; j < pObject->numOfFaces; j++)
			{
				// ���������ε�ÿ������
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					// ������б��еĵ�ǰ�������
					int index = pObject->pFaces[j].vertIndex[whichVertex];
								
					// �ж���������
					if(pObject->pTexVerts) 
					{
						// �����㸳����������
						glTexCoord2f(pObject->pTexVerts[ index ].x, 
									 pObject->pTexVerts[ index ].y);
					}
					
					// ��ö���
					CVector3 vPoint1 = pObject->pVerts[ index ];

					// ��Ⱦ��ǰ����
					glVertex3f(vPoint1.x, vPoint1.y, vPoint1.z);
				}
			}

		// ���ƽ���
		glEnd();
	}
}


//  ��ʼ��CLoadMD3��
CLoadMD3::CLoadMD3()
{
	// ��ʼ��ͷ���ṹ��
	memset(&m_Header, 0, sizeof(tMd3Header));

	// ����ָ��ΪNULL
	m_pSkins=NULL;
	m_pTexCoords=NULL;
	m_pTriangles=NULL;
	m_pBones=NULL;
}


//  ����ĺ����Ĺ����ǽ�MD3�ļ����뵽ģ����
bool CLoadMD3::ImportMD3(t3DModel *pModel, char *strFileName)
{
	char strMessage[255] = {0};

	// �Զ�����ֻ����ģʽ��MD3�ļ�
	m_FilePointer = fopen(strFileName, "rb");

	// ȷ���ļ�ָ��Ϸ�
	if(!m_FilePointer) 
	{
		// ��ʾ������Ϣ
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	// �����ļ�ͷ��Ϣ
	fread(&m_Header, 1, sizeof(tMd3Header), m_FilePointer);

	// ���ID��
	char *ID = m_Header.fileID;

	// �ж�ID�Ƿ�ΪIDP3���汾�Ƿ�Ϊ15
	if((ID[0] != 'I' || ID[1] != 'D' || ID[2] != 'P' || ID[3] != '3') || m_Header.version != 15)
	{
		// ��ʾ������Ϣ
		sprintf(strMessage, "Invalid file format (Version not 15): %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	// ����ģ�ͺͶ�������
	ReadMD3Data(pModel);

	// ɾ���ڴ���Դ
	CleanUp();

	// ����true
	return true;
}


//  ����MD3ģ���г��˶���֮֡�����������
void CLoadMD3::ReadMD3Data(t3DModel *pModel)
{
	int i = 0;

	// Ϊ������Ϣ����洢�ռ�
	m_pBones = new tMd3Bone [m_Header.numFrames];
	fread(m_pBones, sizeof(tMd3Bone), m_Header.numFrames, m_FilePointer);

	// ɾ��û��ʹ�õĹ���
	delete [] m_pBones;

	pModel->pTags = new tMd3Tag [m_Header.numFrames * m_Header.numTags];
	fread(pModel->pTags, sizeof(tMd3Tag), m_Header.numFrames * m_Header.numTags, m_FilePointer);

	// ����ǩ��Ŀ����ģ��
	pModel->numOfTags = m_Header.numTags;
	
	//  ��ʼ������
	pModel->pLinks = (t3DModel **) malloc(sizeof(t3DModel) * m_Header.numTags);
	
	// ��ʼ������ָ��
	for (i = 0; i < m_Header.numTags; i++)
		pModel->pLinks[i] = NULL;

	// ��õ�ǰ�ļ�ָ���ƫ����
	long meshOffset = ftell(m_FilePointer);

	tMd3MeshInfo meshHeader;

	// �������������е��Ӷ���
	for (i = 0; i < m_Header.numMeshes; i++)
	{
		// ����������Ϣ
		fseek(m_FilePointer, meshOffset, SEEK_SET);
		fread(&meshHeader, sizeof(tMd3MeshInfo), 1, m_FilePointer);

		// ����洢�ռ�
		m_pSkins     = new tMd3Skin [meshHeader.numSkins];
		m_pTexCoords = new tMd3TexCoord [meshHeader.numVertices];
		m_pTriangles = new tMd3Face [meshHeader.numTriangles];
		m_pVertices  = new tMd3Triangle [meshHeader.numVertices * meshHeader.numMeshFrames];

		// ����Ƥ����Ϣ
		fread(m_pSkins, sizeof(tMd3Skin), meshHeader.numSkins, m_FilePointer);
		
		// �����������������
		fseek(m_FilePointer, meshOffset + meshHeader.triStart, SEEK_SET);
		fread(m_pTriangles, sizeof(tMd3Face), meshHeader.numTriangles, m_FilePointer);

		// ����UV��������
		fseek(m_FilePointer, meshOffset + meshHeader.uvStart, SEEK_SET);
		fread(m_pTexCoords, sizeof(tMd3TexCoord), meshHeader.numVertices, m_FilePointer);

		// ���붥��/���������Ϣ
		fseek(m_FilePointer, meshOffset + meshHeader.vertexStart, SEEK_SET);
		fread(m_pVertices, sizeof(tMd3Triangle), meshHeader.numMeshFrames * meshHeader.numVertices, m_FilePointer);

		//  ������ת����ģ����
		ConvertDataStructures(pModel, meshHeader);

		// ɾ�����е��ڴ���Դ
		delete [] m_pSkins;    
		delete [] m_pTexCoords;
		delete [] m_pTriangles;
		delete [] m_pVertices;   

		// �����ļ�ָ���ƫ��
		meshOffset += meshHeader.meshSize;
	}
}

//  ���溯���Ĺ�����ת��MD3���ݵ�ģ�ͽṹ����
void CLoadMD3::ConvertDataStructures(t3DModel *pModel, tMd3MeshInfo meshHeader)
{
	int i = 0;

	// ģ����Ŀ����
	pModel->numOfObjects++;
		
	// �����ն���ṹ��
	t3DObject currentMesh = {0};

	// ����������ƿ���������ṹ����
	strcpy(currentMesh.strName, meshHeader.strName);

	// �������㡢������������������Ϣ
	currentMesh.numOfVerts   = meshHeader.numVertices;
	currentMesh.numTexVertex = meshHeader.numVertices;
	currentMesh.numOfFaces   = meshHeader.numTriangles;

	//  ����洢�ռ�
	currentMesh.pVerts    = new CVector3 [currentMesh.numOfVerts * meshHeader.numMeshFrames];
	currentMesh.pTexVerts = new CVector2 [currentMesh.numOfVerts];
	currentMesh.pFaces    = new tFace [currentMesh.numOfFaces];

	// �������еĶ���
	for (i=0; i < currentMesh.numOfVerts * meshHeader.numMeshFrames; i++)
	{
		currentMesh.pVerts[i].x =  m_pVertices[i].vertex[0] / 64.0f;
		currentMesh.pVerts[i].y =  m_pVertices[i].vertex[1] / 64.0f;
		currentMesh.pVerts[i].z =  m_pVertices[i].vertex[2] / 64.0f;
	}

	// �������е�UV��������
	for (i=0; i < currentMesh.numTexVertex; i++)
	{
		currentMesh.pTexVerts[i].x =  m_pTexCoords[i].textureCoord[0];
		currentMesh.pTexVerts[i].y = -m_pTexCoords[i].textureCoord[1];
	}

	// �������е�������
	for(i=0; i < currentMesh.numOfFaces; i++)
	{
		currentMesh.pFaces[i].vertIndex[0] = m_pTriangles[i].vertexIndices[0];
		currentMesh.pFaces[i].vertIndex[1] = m_pTriangles[i].vertexIndices[1];
		currentMesh.pFaces[i].vertIndex[2] = m_pTriangles[i].vertexIndices[2];

		currentMesh.pFaces[i].coordIndex[0] = m_pTriangles[i].vertexIndices[0];
		currentMesh.pFaces[i].coordIndex[1] = m_pTriangles[i].vertexIndices[1];
		currentMesh.pFaces[i].coordIndex[2] = m_pTriangles[i].vertexIndices[2];
	}

	// ��ӵ�ǰ�Ķ��󵽶����б���
	pModel->pObject.push_back(currentMesh);
}


//  ����ĺ���������װ��ģ�͵�Ƥ��
bool CLoadMD3::LoadSkin(t3DModel *pModel, LPSTR strSkin)
{
	// �ж������Ƿ�Ϸ�
	if(!pModel || !strSkin) return false;

	// ��Ƥ���ļ�
	ifstream fin(strSkin);

	// ȷ���ļ��Ѿ���
	if(fin.fail())
	{
		// ��ʾ������Ϣ
		MessageBox(NULL, "Unable to load skin!", "Error", MB_OK);
		return false;
	}

	string strLine = "";
	int textureNameStart = 0;

	// ����Ƥ���ļ��е�ÿһ������
	while(getline(fin, strLine))
	{
		// ���������еĶ���
		for(int i = 0; i < pModel->numOfObjects; i++)
		{
			// �ж϶��������Ƿ����
			if( IsInString(strLine, pModel->pObject[i].strName) )			
			{			
				for(int j = strLine.length() - 1; j > 0; j--)
				{
					if(strLine[j] == '/')
					{
						textureNameStart = j + 1;
						break;
					}	
				}

				tMaterialInfo texture;
				strcpy(texture.strFile, &strLine[textureNameStart]);
				texture.uTile = texture.uTile = 1;
				pModel->pObject[i].materialID = pModel->numOfMaterials;
				pModel->pObject[i].bHasTexture = true;

				// ���Ӳ�����Ŀ
				pModel->numOfMaterials++;

				// ��Ӳ�����Ϣ��ģ�͵Ĳ����б���
				pModel->pMaterials.push_back(texture);
			}
		}
	}

	// �ر��ļ�
	fin.close();
	return true;
}


//  ����ĺ���������װ���������ⲿ����������Ϣ
bool CLoadMD3::LoadShader(t3DModel *pModel, LPSTR strShader)
{
	// ȷ�����ݺϷ�
	if(!pModel || !strShader) return false;

	// �򿪸����ļ�
	ifstream fin(strShader);

	// ȷ���ļ��Ѿ���
	if(fin.fail())
	{
		// ��ʾ������Ϣ
		MessageBox(NULL, "Unable to load shader!", "Error", MB_OK);
		return false;
	}

	string strLine = "";
	int currentIndex = 0;
	
	// �����ļ��е�������
	while(getline(fin, strLine))
	{
		// ����һ��������Ϣ�ṹ��
		tMaterialInfo texture;

		// ���������ļ���
		strcpy(texture.strFile, strLine.c_str());
				
		texture.uTile = texture.uTile = 1;

		// �������ID
		pModel->pObject[currentIndex].materialID = pModel->numOfMaterials;
		pModel->pObject[currentIndex].bHasTexture = true;

		// ����ģ���в�����Ŀ
		pModel->numOfMaterials++;

		pModel->pMaterials.push_back(texture);

		currentIndex++;
	}

	// �ر��ļ�
	fin.close();
	return true;
}


//  ����ĺ����������Ƿ�ռ�õ��ڴ���Դ
void CLoadMD3::CleanUp()
{
	// �رյ�ǰ���ļ�ָ��
	fclose(m_FilePointer);						
}


