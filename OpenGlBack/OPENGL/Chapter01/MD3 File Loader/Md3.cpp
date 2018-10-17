
#include "main.h"
#include "Md3.h"

//  下面的函数用于判断一个字符串是否在另一个字符串中
bool IsInString(string strString, string strSubString)
{
	// 判断两个字符串是否合法
	if(strString.length() <= 0 || strSubString.length() <= 0) return false;

	// 获得子字符串的位置
	int index = strString.find(strSubString);

	// 判断索引是否合法
	if(index >= 0 && index < strString.length())
		return true;

	// 不存在，则返回false
	return false;
}

//  下面是CModelMD3类的构造函数
CModelMD3::CModelMD3()
{
	// 初始化成员变量
	memset(&m_Head,  0, sizeof(t3DModel));
	memset(&m_Upper, 0, sizeof(t3DModel));
	memset(&m_Lower, 0, sizeof(t3DModel));
	memset(&m_Weapon, 0, sizeof(t3DModel));
}

//  下面是CModelMD3类的析构函数
CModelMD3::~CModelMD3()
{
	// 释放所占用的内存资源
	DestroyModel(&m_Head);
	DestroyModel(&m_Upper);
	DestroyModel(&m_Lower);
	DestroyModel(&m_Weapon);
}	


//  下面的函数的功能是释放模型所占用的内存资源
void CModelMD3::DestroyModel(t3DModel *pModel)
{
	// 释放一个MD3模型占用的资源，必须遍历它所有的子对象，并将其模型数据删除
	// 遍历模型中所有的对象
	for(int i = 0; i < pModel->numOfObjects; i++)
	{
		// 删除面、法向量、顶点和纹理坐标
		if(pModel->pObject[i].pFaces)		delete [] pModel->pObject[i].pFaces;
		if(pModel->pObject[i].pNormals)		delete [] pModel->pObject[i].pNormals;
		if(pModel->pObject[i].pVerts)		delete [] pModel->pObject[i].pVerts;
		if(pModel->pObject[i].pTexVerts)	delete [] pModel->pObject[i].pTexVerts;
	}

	// 删除与模型相关的所有标签
	if(pModel->pTags)		delete [] pModel->pTags;
	if(pModel->pLinks)		free(pModel->pLinks);
}
	

//  下面的函数的功能是装入MD3模型
bool CModelMD3::LoadModel(LPSTR strPath, LPSTR strModel)
{
	char strLowerModel[255] = {0};	// lower.md3的模型文件名
	char strUpperModel[255] = {0};	// upper.md3的模型文件名
	char strHeadModel[255]  = {0};	// head.md3的模型文件名
	char strLowerSkin[255]  = {0};	// lower.md3的皮肤文件名
	char strUpperSkin[255]  = {0};	// upper.md3的皮肤文件名
	char strHeadSkin[255]   = {0};	// head.md3的皮肤文件名
	CLoadMD3 loadMd3;				// 装入每个md3和skin文件

	// 判断路径是否合法
	if(!strPath || !strModel) return false;

	// 读入正确的模型文件名称
	sprintf(strLowerModel, "%s\\%s_lower.md3", strPath, strModel);
	sprintf(strUpperModel, "%s\\%s_upper.md3", strPath, strModel);
	sprintf(strHeadModel,  "%s\\%s_head.md3",  strPath, strModel);
	
	// 读入正确的皮肤文件名称
	sprintf(strLowerSkin, "%s\\%s_lower.skin", strPath, strModel);
	sprintf(strUpperSkin, "%s\\%s_upper.skin", strPath, strModel);
	sprintf(strHeadSkin,  "%s\\%s_head.skin",  strPath, strModel);
	
	// 装入头部网格
	if(!loadMd3.ImportMD3(&m_Head,  strHeadModel))
	{
		// 显示错误信息
		MessageBox(g_hWnd, "Unable to load the HEAD model!", "Error", MB_OK);
		return false;
	}

	// 装入上半部分网格
	if(!loadMd3.ImportMD3(&m_Upper, strUpperModel))		
	{
		// 显示错误信息
		MessageBox(g_hWnd, "Unable to load the UPPER model!", "Error", MB_OK);
		return false;
	}

	// 装入下半部分网格
	if(!loadMd3.ImportMD3(&m_Lower, strLowerModel))
	{
		// 显示错误信息
		MessageBox(g_hWnd, "Unable to load the LOWER model!", "Error", MB_OK);
		return false;
	}

	// 装入下半部分皮肤
	if(!loadMd3.LoadSkin(&m_Lower, strLowerSkin))
	{
		// 显示错误信息
		MessageBox(g_hWnd, "Unable to load the LOWER skin!", "Error", MB_OK);
		return false;
	}

	// 装入上半部分皮肤
	if(!loadMd3.LoadSkin(&m_Upper, strUpperSkin))
	{
		// 显示错误信息
		MessageBox(g_hWnd, "Unable to load the UPPER skin!", "Error", MB_OK);
		return false;
	}

	// 装入头部皮肤
	if(!loadMd3.LoadSkin(&m_Head,  strHeadSkin))
	{
		// 显示错误信息
		MessageBox(g_hWnd, "Unable to load the HEAD skin!", "Error", MB_OK);
		return false;
	}

	// 装入下半部分、上半部分和头部纹理  
	LoadModelTextures(&m_Lower, strPath);
	LoadModelTextures(&m_Upper, strPath);
	LoadModelTextures(&m_Head,  strPath);

	// 连接身体的上半部分和下半部分
	LinkModel(&m_Lower, &m_Upper, "tag_torso");

	// 连接身体的上半部分和头部
	LinkModel(&m_Upper, &m_Head, "tag_head");
		
	return true;
}


//  下面的函数的功能是装入武器模型
bool CModelMD3::LoadWeapon(LPSTR strPath, LPSTR strModel)
{
	char strWeaponModel[255]  = {0};	// 武器模型的文件名称
	char strWeaponShader[255] = {0};	// 武器模型的外部覆盖体
	CLoadMD3 loadMd3;					// 装入武器模型与覆盖体

	// 判断路径是否合法
	if(!strPath || !strModel) return false;

	// 连接路径与模型文件名
	sprintf(strWeaponModel, "%s\\%s.md3", strPath, strModel);
	
	// 装入武器网格模型
	if(!loadMd3.ImportMD3(&m_Weapon,  strWeaponModel))
	{
		// 显示错误信息
		MessageBox(g_hWnd, "Unable to load the WEAPON model!", "Error", MB_OK);
		return false;
	}

	// 连接武器外部覆盖与路径
	sprintf(strWeaponShader, "%s\\%s.shader", strPath, strModel);

	// 装入武器的外部覆盖物
	if(!loadMd3.LoadShader(&m_Weapon, strWeaponShader))
	{
		// 显示错误信息
		MessageBox(g_hWnd, "Unable to load the SHADER file!", "Error", MB_OK);
		return false;
	}

	//  装入武器的纹理  
	LoadModelTextures(&m_Weapon, strPath);

	// 连接武器与模型的上半部分(手)
	LinkModel(&m_Upper, &m_Weapon, "tag_weapon");
		
	// 返回true
	return true;
}

//  下面的函数功能是装入模型的纹理
void CModelMD3::LoadModelTextures(t3DModel *pModel, LPSTR strPath)
{
	// 遍历模型中的所有的材质
	for(int i = 0; i < pModel->numOfMaterials; i++)
	{
		// 判断在材质中是否有一个文件名
		if(strlen(pModel->pMaterials[i].strFile) > 0)
		{
			// 设置新纹理的标识为true
			bool bNewTexture = true;

			// 遍历所有的纹理
			for(int j = 0; j < strTextures.size(); j++)
			{
				// 如果纹理已经存在，则略过
				if(!strcmp(pModel->pMaterials[i].strFile, strTextures[j].c_str()) )
				{
					// 纹理标识设置为false
					bNewTexture = false;

					//  设置纹理ID号
					pModel->pMaterials[i].texureId = j;
				}
			}

			// 如果纹理标识为false，则继续
			if(bNewTexture == false) continue;
			
			char strFullPath[255] = {0};

			// 增加纹理的文件名和路径
			sprintf(strFullPath, "%s\\%s", strPath, pModel->pMaterials[i].strFile);

			//  根据文件名创建纹理
			CreateTexture(m_Textures[strTextures.size()], strFullPath);								

			// 设置纹理ID
			pModel->pMaterials[i].texureId = strTextures.size();

			strTextures.push_back(pModel->pMaterials[i].strFile);
		}
	}
}


//  下面的函数功能是连接模型
void  CModelMD3::LinkModel(t3DModel *pModel, t3DModel *pLink, LPSTR strTagName)
{
	// 确保模型合法
	if(!pModel || !pLink || !strTagName) return;

	// 遍历所有的标签
	for(int i = 0; i < pModel->numOfTags; i++)
	{
		// 如果当前的标签索引中有所找的标签名
		if( !_stricmp(pModel->pTags[i].strName, strTagName) )
		{
			// 连接模型
			pModel->pLinks[i] = pLink;
			return;
		}
	}
}


//  下面的函数功能是绘制模型，这是一个递归函数
void CModelMD3::DrawModel()
{

	// 旋转模型
	glRotatef(-90, 1, 0, 0);

	// 绘制第一个连接
	DrawLink(&m_Lower);
}


//  下面的函数的功能是绘制连接，这是一个递归函数
void CModelMD3::DrawLink(t3DModel *pModel)
{
	// 渲染当前的模型(开始位置是脚)
	RenderModel(pModel);

	// 遍历所有的模型标签，并绘制
	for(int i = 0; i < pModel->numOfTags; i++)
	{
		// 获得当前的连接
		t3DModel *pLink = pModel->pLinks[i];

		// 如果连接合法
		if(pLink)
		{			
			// 新模型的变换位置 
			CVector3 vPosition = pModel->pTags[i].vPosition;

			// 压入矩阵
			glPushMatrix();
			
				// 平移变换
				glTranslatef(vPosition.x, vPosition.y, vPosition.z);

				//  绘制连接
				DrawLink(pLink);

			// 弹出矩阵
			glPopMatrix();
		}
	}

}


//  下面的函数功能是在屏幕上渲染模型
void CModelMD3::RenderModel(t3DModel *pModel)
{
	// 确保对象合法
	if(pModel->pObject.size() <= 0) return;

	// 遍历模型中所有的对象
	for(int i = 0; i < pModel->numOfObjects; i++)
	{
		// 获得当前显示的对象
		t3DObject *pObject = &pModel->pObject[i];

		//  如果对象具有纹理
		if(pObject->bHasTexture)
		{
			// 开启纹理映射
			glEnable(GL_TEXTURE_2D);

			// 获得纹理ID
			int textureID = pModel->pMaterials[pObject->materialID].texureId;

			// 捆绑纹理
			glBindTexture(GL_TEXTURE_2D, m_Textures[textureID]);
		}
		else
		{
			// 关闭纹理映射
			glDisable(GL_TEXTURE_2D);
		}

		// 开始绘制模型三角形
		glBegin(GL_TRIANGLES);

			// 遍历所有的多边形面
			for(int j = 0; j < pObject->numOfFaces; j++)
			{
				// 遍历三角形的每个顶点
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					// 获得面列表中的当前点的索引
					int index = pObject->pFaces[j].vertIndex[whichVertex];
								
					// 判断纹理坐标
					if(pObject->pTexVerts) 
					{
						// 给顶点赋予纹理坐标
						glTexCoord2f(pObject->pTexVerts[ index ].x, 
									 pObject->pTexVerts[ index ].y);
					}
					
					// 获得顶点
					CVector3 vPoint1 = pObject->pVerts[ index ];

					// 渲染当前顶点
					glVertex3f(vPoint1.x, vPoint1.y, vPoint1.z);
				}
			}

		// 绘制结束
		glEnd();
	}
}


//  初始化CLoadMD3类
CLoadMD3::CLoadMD3()
{
	// 初始化头部结构体
	memset(&m_Header, 0, sizeof(tMd3Header));

	// 设置指针为NULL
	m_pSkins=NULL;
	m_pTexCoords=NULL;
	m_pTriangles=NULL;
	m_pBones=NULL;
}


//  下面的函数的功能是将MD3文件读入到模型中
bool CLoadMD3::ImportMD3(t3DModel *pModel, char *strFileName)
{
	char strMessage[255] = {0};

	// 以二进制只读的模式打开MD3文件
	m_FilePointer = fopen(strFileName, "rb");

	// 确保文件指针合法
	if(!m_FilePointer) 
	{
		// 显示错误信息
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	// 读入文件头信息
	fread(&m_Header, 1, sizeof(tMd3Header), m_FilePointer);

	// 获得ID号
	char *ID = m_Header.fileID;

	// 判断ID是否为IDP3，版本是否为15
	if((ID[0] != 'I' || ID[1] != 'D' || ID[2] != 'P' || ID[3] != '3') || m_Header.version != 15)
	{
		// 显示错误信息
		sprintf(strMessage, "Invalid file format (Version not 15): %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	// 读入模型和动画数据
	ReadMD3Data(pModel);

	// 删除内存资源
	CleanUp();

	// 返回true
	return true;
}


//  读入MD3模型中除了动画帧之外的所有数据
void CLoadMD3::ReadMD3Data(t3DModel *pModel)
{
	int i = 0;

	// 为骨骼信息分配存储空间
	m_pBones = new tMd3Bone [m_Header.numFrames];
	fread(m_pBones, sizeof(tMd3Bone), m_Header.numFrames, m_FilePointer);

	// 删除没有使用的骨骼
	delete [] m_pBones;

	pModel->pTags = new tMd3Tag [m_Header.numFrames * m_Header.numTags];
	fread(pModel->pTags, sizeof(tMd3Tag), m_Header.numFrames * m_Header.numTags, m_FilePointer);

	// 将标签数目赋予模型
	pModel->numOfTags = m_Header.numTags;
	
	//  初始化连接
	pModel->pLinks = (t3DModel **) malloc(sizeof(t3DModel) * m_Header.numTags);
	
	// 初始化连接指针
	for (i = 0; i < m_Header.numTags; i++)
		pModel->pLinks[i] = NULL;

	// 获得当前文件指针的偏移量
	long meshOffset = ftell(m_FilePointer);

	tMd3MeshInfo meshHeader;

	// 遍历网格中所有的子对象
	for (i = 0; i < m_Header.numMeshes; i++)
	{
		// 读入网格信息
		fseek(m_FilePointer, meshOffset, SEEK_SET);
		fread(&meshHeader, sizeof(tMd3MeshInfo), 1, m_FilePointer);

		// 分配存储空间
		m_pSkins     = new tMd3Skin [meshHeader.numSkins];
		m_pTexCoords = new tMd3TexCoord [meshHeader.numVertices];
		m_pTriangles = new tMd3Face [meshHeader.numTriangles];
		m_pVertices  = new tMd3Triangle [meshHeader.numVertices * meshHeader.numMeshFrames];

		// 读入皮肤信息
		fread(m_pSkins, sizeof(tMd3Skin), meshHeader.numSkins, m_FilePointer);
		
		// 读入三角形面的数据
		fseek(m_FilePointer, meshOffset + meshHeader.triStart, SEEK_SET);
		fread(m_pTriangles, sizeof(tMd3Face), meshHeader.numTriangles, m_FilePointer);

		// 读入UV纹理坐标
		fseek(m_FilePointer, meshOffset + meshHeader.uvStart, SEEK_SET);
		fread(m_pTexCoords, sizeof(tMd3TexCoord), meshHeader.numVertices, m_FilePointer);

		// 读入顶点/面的索引信息
		fseek(m_FilePointer, meshOffset + meshHeader.vertexStart, SEEK_SET);
		fread(m_pVertices, sizeof(tMd3Triangle), meshHeader.numMeshFrames * meshHeader.numVertices, m_FilePointer);

		//  将数据转换到模型中
		ConvertDataStructures(pModel, meshHeader);

		// 删除所有的内存资源
		delete [] m_pSkins;    
		delete [] m_pTexCoords;
		delete [] m_pTriangles;
		delete [] m_pVertices;   

		// 增加文件指针的偏移
		meshOffset += meshHeader.meshSize;
	}
}

//  下面函数的功能是转换MD3数据到模型结构体中
void CLoadMD3::ConvertDataStructures(t3DModel *pModel, tMd3MeshInfo meshHeader)
{
	int i = 0;

	// 模型数目递增
	pModel->numOfObjects++;
		
	// 创建空对象结构体
	t3DObject currentMesh = {0};

	// 将对象的名称拷贝到对象结构体中
	strcpy(currentMesh.strName, meshHeader.strName);

	// 拷贝顶点、纹理坐标和面等数据信息
	currentMesh.numOfVerts   = meshHeader.numVertices;
	currentMesh.numTexVertex = meshHeader.numVertices;
	currentMesh.numOfFaces   = meshHeader.numTriangles;

	//  分配存储空间
	currentMesh.pVerts    = new CVector3 [currentMesh.numOfVerts * meshHeader.numMeshFrames];
	currentMesh.pTexVerts = new CVector2 [currentMesh.numOfVerts];
	currentMesh.pFaces    = new tFace [currentMesh.numOfFaces];

	// 遍历所有的顶点
	for (i=0; i < currentMesh.numOfVerts * meshHeader.numMeshFrames; i++)
	{
		currentMesh.pVerts[i].x =  m_pVertices[i].vertex[0] / 64.0f;
		currentMesh.pVerts[i].y =  m_pVertices[i].vertex[1] / 64.0f;
		currentMesh.pVerts[i].z =  m_pVertices[i].vertex[2] / 64.0f;
	}

	// 遍历所有的UV纹理坐标
	for (i=0; i < currentMesh.numTexVertex; i++)
	{
		currentMesh.pTexVerts[i].x =  m_pTexCoords[i].textureCoord[0];
		currentMesh.pTexVerts[i].y = -m_pTexCoords[i].textureCoord[1];
	}

	// 遍历所有的面数据
	for(i=0; i < currentMesh.numOfFaces; i++)
	{
		currentMesh.pFaces[i].vertIndex[0] = m_pTriangles[i].vertexIndices[0];
		currentMesh.pFaces[i].vertIndex[1] = m_pTriangles[i].vertexIndices[1];
		currentMesh.pFaces[i].vertIndex[2] = m_pTriangles[i].vertexIndices[2];

		currentMesh.pFaces[i].coordIndex[0] = m_pTriangles[i].vertexIndices[0];
		currentMesh.pFaces[i].coordIndex[1] = m_pTriangles[i].vertexIndices[1];
		currentMesh.pFaces[i].coordIndex[2] = m_pTriangles[i].vertexIndices[2];
	}

	// 添加当前的对象到对象列表中
	pModel->pObject.push_back(currentMesh);
}


//  下面的函数功能是装入模型的皮肤
bool CLoadMD3::LoadSkin(t3DModel *pModel, LPSTR strSkin)
{
	// 判断数据是否合法
	if(!pModel || !strSkin) return false;

	// 打开皮肤文件
	ifstream fin(strSkin);

	// 确保文件已经打开
	if(fin.fail())
	{
		// 显示错误信息
		MessageBox(NULL, "Unable to load skin!", "Error", MB_OK);
		return false;
	}

	string strLine = "";
	int textureNameStart = 0;

	// 遍历皮肤文件中的每一行数据
	while(getline(fin, strLine))
	{
		// 遍历所所有的对象
		for(int i = 0; i < pModel->numOfObjects; i++)
		{
			// 判断对象名称是否存在
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

				// 增加材质数目
				pModel->numOfMaterials++;

				// 添加材质信息到模型的材质列表中
				pModel->pMaterials.push_back(texture);
			}
		}
	}

	// 关闭文件
	fin.close();
	return true;
}


//  下面的函数功能是装入武器的外部覆盖纹理信息
bool CLoadMD3::LoadShader(t3DModel *pModel, LPSTR strShader)
{
	// 确保数据合法
	if(!pModel || !strShader) return false;

	// 打开覆盖文件
	ifstream fin(strShader);

	// 确保文件已经打开
	if(fin.fail())
	{
		// 显示错误信息
		MessageBox(NULL, "Unable to load shader!", "Error", MB_OK);
		return false;
	}

	string strLine = "";
	int currentIndex = 0;
	
	// 遍历文件中的所有行
	while(getline(fin, strLine))
	{
		// 创建一个纹理信息结构体
		tMaterialInfo texture;

		// 拷贝纹理文件名
		strcpy(texture.strFile, strLine.c_str());
				
		texture.uTile = texture.uTile = 1;

		// 保存材质ID
		pModel->pObject[currentIndex].materialID = pModel->numOfMaterials;
		pModel->pObject[currentIndex].bHasTexture = true;

		// 增加模型中材质数目
		pModel->numOfMaterials++;

		pModel->pMaterials.push_back(texture);

		currentIndex++;
	}

	// 关闭文件
	fin.close();
	return true;
}


//  下面的函数功能是是否占用的内存资源
void CLoadMD3::CleanUp()
{
	// 关闭当前的文件指针
	fclose(m_FilePointer);						
}


