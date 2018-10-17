
#include "Main.h"
#include "Ase.h"


//  下面的函数功能是将ASE模型读入到t3DModel模型结构体中
bool CLoadASE::ImportASE(t3DModel *pModel, char *strFileName)
{
	char strMessage[255] = {0};				// 用于显示错误信息

	// 判断是否是一个合法的模型和文件类型
	if(!pModel || !strFileName) return false;

	// 以只读方式打开文件，返回文件指针
	m_FilePointer = fopen(strFileName, "r");

	// 判断文件指针是否正确
	if(!m_FilePointer) {
		// 如果文件指针不正确，则显示错误信息
		sprintf(strMessage, "Unable to find or open the file: %s", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// 读入文件信息
	ReadAseFile(pModel);

	// 计算顶点的法向量，用于光照
	ComputeNormals(pModel);

	// 关闭打开的文件
	fclose(m_FilePointer);

	return true;
}


//  下面的函数用来读入一个Ase文件
void CLoadASE::ReadAseFile(t3DModel *pModel)
{
	tMaterialInfo newMaterial = {0};		// 新材质
	t3DObject     newObject   = {0};		// 新对象
	int i = 0;	

	// 获得ase文件中对象的数量
	pModel->numOfObjects   = GetObjectCount();

	// 获得ase文件中材质的数量
	pModel->numOfMaterials = GetMaterialCount();

	// 遍历所有的材质
	for(i = 0; i < pModel->numOfMaterials; i++)
	{
		// 在材质链表中添加一个新的材质
		pModel->pMaterials.push_back(newMaterial);

		// 获得当前材质的材质信息
		GetTextureInfo(&(pModel->pMaterials[i]), i + 1);
	}

	// 遍历所有的对象
	for(i = 0; i < pModel->numOfObjects; i++)
	{	
		// 在对象链表中添加一个新的对象
		pModel->pObject.push_back(newObject);

		// 初始化材质ID，设置为-1
		pModel->pObject[i].materialID = -1;

		// 将文件指针移动到指定的对象
		MoveToObject(i + 1);

		// 确定对象的顶点数、面的数量和纹理坐标数量，然后分配足够的内存空间
		ReadObjectInfo(&(pModel->pObject[i]), i + 1);
		
		// 读入对象的顶点、面和纹理坐标数据
		ReadObjectData(pModel, &(pModel->pObject[i]), i + 1);
	}
}

//  下面的函数的功能是获得ase文件中对象的数量
int CLoadASE::GetObjectCount()
{
	char strWord[255] = {0};
	int objectCount = 0;

	// 将文件指针的定位在文件的开始
	rewind(m_FilePointer);

	// 遍历整个文件
	while (!feof(m_FilePointer))
	{
		// 获得文件中的每个字符串
		fscanf(m_FilePointer, "%s", &strWord);

		// 判断是否是对象
		if (!strcmp(strWord, OBJECT))
		{
			// 增加对象计数器 
			objectCount++;
		}
		else
		{
			// 进入下一行
			fgets(strWord, 100, m_FilePointer);
		}
	}

	// 返回ase文件中的对象数量
	return objectCount;
}

//  下面的函数功能是获得ase文件中的材质
int CLoadASE::GetMaterialCount()
{
	char strWord[255] = {0};
	int materialCount = 0;

	// 将文件指针定位于文件开始处
	rewind(m_FilePointer);

	// 遍历整个文件
	while (!feof(m_FilePointer))
	{
		// 获得文件中的每个字符串
		fscanf(m_FilePointer, "%s", &strWord);

		// 判断是否是对象的材质数量
		if (!strcmp(strWord, MATERIAL_COUNT))
		{
			// 读入材质数
			fscanf(m_FilePointer, "%d", &materialCount);

			// 返回材质数
			return materialCount;
		}
		else
		{
			// 进入下一行
			fgets(strWord, 100, m_FilePointer);
		}
	}

	// 如果没有材质，则返回0
	return 0;
}

//  获得指定材质的材质信息
void CLoadASE::GetTextureInfo (tMaterialInfo *pTexture, int desiredMaterial)
{
	char strWord[255] = {0};
	int materialCount= 0;
	
	// 将文件指针定位于文件开始处
	rewind(m_FilePointer);

	// 遍历整个文件
	while (!feof(m_FilePointer))
	{
		// 获得文件中每个字符串
		fscanf(m_FilePointer, "%s", &strWord);

		// 判断是否是材质
		if (!strcmp(strWord, MATERIAL))
		{
			// 增加材质计数器 
			materialCount++;

			// 判断材质计数器是否与具体的材质匹配
			if(materialCount == desiredMaterial)
				break;
		}
		else
		{
			// 进入下一行
			fgets(strWord, 100, m_FilePointer);
		}
	}

	// 现在进入所需要读入的材质

	// 遍历余下的文件，直到结束
	while (!feof(m_FilePointer))
	{
		// 获得文件中的每个字符串
		fscanf(m_FilePointer, "%s", &strWord);

		// 判断是否是材质标签
		if (!strcmp (strWord, MATERIAL))
		{
			// 新的材质，则返回
			return;
		}
		// 如果是MATERIAL_COLOR标签，需要获得材质的颜色
		else if (!strcmp(strWord, MATERIAL_COLOR))
		{
			// 获得对象材质的RGB颜色
			fscanf(m_FilePointer, " %f %f %f", &(pTexture->fColor[0]), 
											   &(pTexture->fColor[1]), 
											   &(pTexture->fColor[2]));
		}
		// 如果是TEXTURE标签，需要获得纹理名称
		else if (!strcmp(strWord, TEXTURE))
		{
			// 获得纹理的文件名
			GetTextureName(pTexture);
		}
		// 如果是MATERIAL_NAME标签，则需要获得材质的名称
		else if (!strcmp(strWord, MATERIAL_NAME))
		{
			// 获得对象的材质名称
			GetMaterialName(pTexture);
		}
		// 如果是UTILE标签，则需要获得U重复率
		else if(!strcmp(strWord, UTILE))
		{
			// 读入U纹理坐标上的U重复率
			pTexture->uTile = ReadFloat();
		}
		// 如果是VTILE标签，则需要获得V重复率
		else if(!strcmp(strWord, VTILE))
		{
			// 读入V纹理坐标上的V重复率
			pTexture->vTile = ReadFloat();
		}
		// 否则，略过这些数据
		else
		{
			// 进入下一行
			fgets (strWord, 100, m_FilePointer);
		}
	}
}

//  下面的函数功能是将文件指针移到Ase文件中指定的对象
void CLoadASE::MoveToObject (int desiredObject)
{
	char strWord[255] = {0};
	int objectCount = 0;

	// 将文件指针定位的文件的开始
	rewind(m_FilePointer);

	// 遍历整个文件，直到文件结束
	while(!feof(m_FilePointer))
	{
		// 获得文件每一个字符串
		fscanf(m_FilePointer, "%s", &strWord);

		// 判断是否是一个对象
		if(!strcmp(strWord, OBJECT))
		{
			// 增加对象计数器 
			objectCount++;

			// 如果是指定的对象
			if(objectCount == desiredObject)
				return;
		}
		else
		{
			// 进入下一行
			fgets(strWord, 100, m_FilePointer);
		}
	}
}

//  下面的函数功能是从Ase文件中读入一个浮点数
float CLoadASE::ReadFloat()
{
	float v = 0.0f;

	// 读入一个浮点数
	fscanf(m_FilePointer, " %f", &v);

	// 返回该浮点数
	return v;
}


//  下面的函数的功能是读入对象的信息
void CLoadASE::ReadObjectInfo(t3DObject *pObject, int desiredObject)
{
	char strWord[255] = {0};

	// 将文件指针移动到指定的对象
	MoveToObject(desiredObject);
	
	// 直到文件尾才结束
	while (!feof(m_FilePointer))
	{
		// 从文件中读入每个字符串
		fscanf(m_FilePointer, "%s", &strWord);

		// 如果是顶点数量标签
		if (!strcmp(strWord, NUM_VERTEX))
		{
			// 读入对象的顶点数量
			fscanf(m_FilePointer, "%d", &pObject->numOfVerts);

			// 分配保存顶点的内存空间
			pObject->pVerts = new CVector3 [pObject->numOfVerts];
		}
		// 如果是面的数目标签
		else if (!strcmp(strWord, NUM_FACES))
		{
			// 读入对象中面的数目
			fscanf(m_FilePointer, "%d", &pObject->numOfFaces);

			// 分配保存面的内存空间
			pObject->pFaces = new tFace [pObject->numOfFaces];
		}
		// 如果是纹理坐标数目
		else if (!strcmp(strWord, NUM_TVERTEX))
		{
			// 读入对象的纹理坐标数目
			fscanf(m_FilePointer, "%d", &pObject->numTexVertex);

			// 分配保存UV坐标的内存空间
			pObject->pTexVerts = new CVector2 [pObject->numTexVertex];
		}
		// 如果是对象标签，则进入了下一个对象
		else if (!strcmp(strWord, OBJECT))	
		{
			// 返回
			return;
		}
		else 
		{
			// 没有找到任何东西，则进入下一行
			fgets(strWord, 100, m_FilePointer);
		}
	}	
}


//  下面的函数功能是读入对象的纹理文件名称
void CLoadASE::GetTextureName(tMaterialInfo *pTexture)
{
	// 读入纹理的文件名
	fscanf (m_FilePointer, " \"%s", &(pTexture->strFile));
	
	// 在字符串的末尾增加一个NULL字符
	pTexture->strFile[strlen (pTexture->strFile) - 1] = '\0';
}

//  下面的函数功能是读入对象的材质名称
void CLoadASE::GetMaterialName(tMaterialInfo *pTexture)
{
	// 读入材质名称
	fscanf (m_FilePointer, " \"%s", &(pTexture->strName));
	
	// 在字符串的末尾加上NULL字符
	pTexture->strName[strlen (pTexture->strName)] = '\0';
}

//  下面的函数功能是读入指定对象的具体数据
void CLoadASE::ReadObjectData(t3DModel *pModel, t3DObject *pObject, int desiredObject)
{

	// 读入对象的材质ID
	GetData(pModel, pObject, MATERIAL_ID, desiredObject);

	// 读入对象的顶点数
	GetData(pModel, pObject, VERTEX,		 desiredObject);

	// 读入对象的纹理坐标
	GetData(pModel, pObject, TVERTEX,	 desiredObject);

	// 读入对象的面链表
	GetData(pModel, pObject, FACE,		 desiredObject);

	// 读入对象的纹理面链表
	GetData(pModel, pObject, TFACE,		 desiredObject);

	// 读入对象的纹理数据
	GetData(pModel, pObject, TEXTURE,	 desiredObject);

	// 读入对象的U重复
	GetData(pModel, pObject, UTILE,		 desiredObject);

	// 读入对象的V重复
	GetData(pModel, pObject, VTILE,		 desiredObject);
}

//  下面的函数功能是获得具体的数据
void CLoadASE::GetData(t3DModel *pModel, t3DObject *pObject, char *strDesiredData, int desiredObject)
{
	char strWord[255] = {0};

	// 将文件指针移动到指定的对象
	MoveToObject(desiredObject);

	// 遍历整个文件直到文件结束
	while(!feof(m_FilePointer))
	{
		// 读入每一个字符串
		fscanf(m_FilePointer, "%s", &strWord);

		// 判断是否是对象
		if(!strcmp(strWord, OBJECT))	
		{
			// 返回
			return;
		}
		// 如果是顶点标签
		else if(!strcmp(strWord, VERTEX))
		{
			// 确定是希望读入的数据
			if(!strcmp(strDesiredData, VERTEX)) 
			{
				// 读入顶点数据
				ReadVertex(pObject);
			}
		}
		// 如果是纹理坐标
		else if(!strcmp(strWord, TVERTEX))
		{
			// 确保是希望读入的数据
			if(!strcmp(strDesiredData, TVERTEX)) 
			{
				// 读入纹理坐标
				ReadTextureVertex(pObject, pModel->pMaterials[pObject->materialID]);
			}
		}
		// 如果是面的顶点索引
		else if(!strcmp(strWord, FACE))
		{
			// 确保是希望读入的数据
			if(!strcmp(strDesiredData, FACE)) 
			{
				// 读入面数据
				ReadFace(pObject);
			}
		}
		// 如果是面的纹理坐标索引
		else if(!strcmp(strWord, TFACE))
		{
			// 确保是希望读入的数据
			if(!strcmp(strDesiredData, TFACE))
			{
				// 读入面的纹理索引
				ReadTextureFace(pObject);
			}
		}
		// 如果是对象的材质ID
		else if(!strcmp(strWord, MATERIAL_ID))
		{
			// 确保是希望读入的数据
			if(!strcmp(strDesiredData, MATERIAL_ID))
			{
				// 读入对象的材质ID
				pObject->materialID = (int)ReadFloat();
				return;
			}				
		}
		else 
		{
			// 进入下一行
			fgets(strWord, 100, m_FilePointer);
		}
	}
}


//  读入对象的顶点数据
void CLoadASE::ReadVertex(t3DObject *pObject)
{
	int index = 0;

	// 略过顶点索引
	fscanf(m_FilePointer, "%d", &index);
	
	fscanf(m_FilePointer, "%f %f %f", &pObject->pVerts[index].x, 
									  &pObject->pVerts[index].z,
									  &pObject->pVerts[index].y);
		
	pObject->pVerts[index].z = -pObject->pVerts[index].z;
}


//  读入纹理坐标
void CLoadASE::ReadTextureVertex(t3DObject *pObject, tMaterialInfo texture)
{
	int index = 0;

	// 略过纹理坐标的索引号
	fscanf(m_FilePointer, "%d", &index);

	// 读入纹理坐标(U, V)
	fscanf(m_FilePointer, "%f %f", &(pObject->pTexVerts[index].x), &(pObject->pTexVerts[index].y));

	pObject->pTexVerts[index].x *= texture.uTile;
	pObject->pTexVerts[index].y *= texture.vTile;

	// 置标志位为true
	pObject->bHasTexture = true;
}


//  读入面的顶点索引
void CLoadASE::ReadFace(t3DObject *pObject)
{
	int index = 0;

	// 略过面的索引
	fscanf(m_FilePointer, "%d:", &index);

	// 读入面的顶点索引
	fscanf(m_FilePointer, "\tA:\t%d B:\t%d C:\t%d", &(pObject->pFaces[index].vertIndex[0]), 
													&(pObject->pFaces[index].vertIndex[1]), 
													&(pObject->pFaces[index].vertIndex[2])); 
}


//  读入面的纹理坐标
void CLoadASE::ReadTextureFace(t3DObject *pObject)
{
	int index = 0;

	// 略过纹理坐标索引
	fscanf(m_FilePointer, "%d:", &index);

	// 读入当前面的UV坐标索引
	fscanf(m_FilePointer, "%d %d %d", &pObject->pFaces[index].coordIndex[0], 
									  &pObject->pFaces[index].coordIndex[1], 
									  &pObject->pFaces[index].coordIndex[2]);
}
  

// 下面的宏定义计算一个矢量的长度
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))

// 下面的函数求两点决定的矢量
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2)
{
	CVector3 vVector;							

	vVector.x = vPoint1.x - vPoint2.x;			
	vVector.y = vPoint1.y - vPoint2.y;			
	vVector.z = vPoint1.z - vPoint2.z;			

	return vVector;								
}


// 下面的函数两个矢量相加
CVector3 AddVector(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vResult;							
	
	vResult.x = vVector2.x + vVector1.x;		
	vResult.y = vVector2.y + vVector1.y;		
	vResult.z = vVector2.z + vVector1.z;		

	return vResult;								
}

// 下面的函数处理矢量的缩放
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler)
{
	CVector3 vResult;							
	
	vResult.x = vVector1.x / Scaler;			
	vResult.y = vVector1.y / Scaler;			
	vResult.z = vVector1.z / Scaler;			

	return vResult;								
}

// 下面的函数返回两个矢量的叉积
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vCross;								
												
	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
												
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
												
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vCross;								
}


// 下面的函数规范化矢量
CVector3 Normalize(CVector3 vNormal)
{
	double Magnitude;							

	Magnitude = Mag(vNormal);					// 获得矢量的长度

	vNormal.x /= (float)Magnitude;				
	vNormal.y /= (float)Magnitude;				
	vNormal.z /= (float)Magnitude;				

	return vNormal;								
}

//  下面的函数用于计算对象的法向量
void CLoadASE::ComputeNormals(t3DModel *pModel)
{
	CVector3 vVector1, vVector2, vNormal, vPoly[3];

	// 如果模型中没有对象，则返回
	if(pModel->numOfObjects <= 0)
		return;

	// 遍历模型中所有的对象
	for(int index = 0; index < pModel->numOfObjects; index++)
	{
		// 获得当前的对象
		t3DObject *pObject = &(pModel->pObject[index]);

		// 分配需要的存储空间
		CVector3 *pNormals		= new CVector3 [pObject->numOfFaces];
		CVector3 *pTempNormals	= new CVector3 [pObject->numOfFaces];
		pObject->pNormals		= new CVector3 [pObject->numOfVerts];

		// 遍历对象的所有面
		for(int i=0; i < pObject->numOfFaces; i++)
		{												
			vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
			vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
			vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];

			// 计算面的法向量

			vVector1 = Vector(vPoly[0], vPoly[2]);		// 获得多边形的矢量
			vVector2 = Vector(vPoly[2], vPoly[1]);		// 获得多边形的第二个矢量

			vNormal  = Cross(vVector1, vVector2);		// 获得两个矢量的叉积
			pTempNormals[i] = vNormal;					// 保存非规范化法向量
			vNormal  = Normalize(vNormal);				// 规范化获得的叉积

			pNormals[i] = vNormal;						// 将法向量添加到法向量列表中
		}

		//  下面求顶点法向量
		CVector3 vSum = {0.0, 0.0, 0.0};
		CVector3 vZero = vSum;
		int shared=0;
		// 遍历所有的顶点
		for (i = 0; i < pObject->numOfVerts; i++)			
		{
			for (int j = 0; j < pObject->numOfFaces; j++)	// 遍历所有的三角形面
			{												// 判断该点是否与其它的面共享
				if (pObject->pFaces[j].vertIndex[0] == i || 
					pObject->pFaces[j].vertIndex[1] == i || 
					pObject->pFaces[j].vertIndex[2] == i)
				{
					vSum = AddVector(vSum, pTempNormals[j]);
					shared++;								
				}
			}      
			
			pObject->pNormals[i] = DivideVectorByScaler(vSum, float(-shared));

			// 规范化最后的顶点法向
			pObject->pNormals[i] = Normalize(pObject->pNormals[i]);	

			vSum = vZero;								
			shared = 0;										
		}
	
		// 释放存储空间，开始下一个对象
		delete [] pTempNormals;
		delete [] pNormals;
	}
}



