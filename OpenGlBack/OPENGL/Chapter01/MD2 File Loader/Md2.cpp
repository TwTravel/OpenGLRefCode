
#include "main.h"
#include "Md2.h"

//  下面的函数的功能是初始化类CLoadMD2中的各成员变量
CLoadMD2::CLoadMD2()
{
	// 初始化结构体
	memset(&m_Header, 0, sizeof(tMd2Header));

	// 设置各指针为NULL
	m_pSkins=NULL;
	m_pTexCoords=NULL;
	m_pTriangles=NULL;
	m_pFrames=NULL;
}


//  下面的函数的功能是打开MD2文件，读出MD2文件中的数据，最后释放内存资源并关闭文件
bool CLoadMD2::ImportMD2(t3DModel *pModel, char *strFileName, char *strTexture)
{
	char strMessage[255] = {0};

	// 以只读、二进制模式打开MD2文件
	m_FilePointer = fopen(strFileName, "rb");

	// 判断文件指针释放合法
	if(!m_FilePointer) 
	{
		// 如果非法，显示错误信息
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	// 与大多数文件格式一样，需要从MD2文件中读出一个文件头信息

	// 读出头部信息，并将其保存在成员变量m_Header中
	fread(&m_Header, 1, sizeof(tMd2Header), m_FilePointer);

	// 对于MD2格式的文件，文件的版本必须是8
	if(m_Header.version != 8)
	{
		// 如果不是8，则显示错误信息后返回
		sprintf(strMessage, "Invalid file format (Version not 8): %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	//  继续读入余下的数据
	ReadMD2Data();
	
	// 将其转换到pModel模型结构体中
	ConvertDataStructures(pModel);

	// 读入整个MD2文件之后，再计算模型的法向量
	ComputeNormals(pModel);

	// 如果文件中有一个具体的纹理名称，则需读入纹理数据
	if(strTexture)
	{
		// 生成一个材质信息结构体
		tMaterialInfo texture;

		// 将文件名称拷贝到纹理名称变量中
		strcpy(texture.strFile, strTexture);

		// 由于在在MD2文件中只有唯一的纹理，ID号总是0
		texture.texureId = 0;

		// 纹理的UV重复都是1
		texture.uTile = texture.uTile = 1;

		// 一个模型中只有一个材质
		pModel->numOfMaterials = 1;

		// 添加材质信息到模型的材质列表中
		pModel->pMaterials.push_back(texture);
	}

	// 释放内存资源
	CleanUp();

	// 返回true
	return true;
}

//  下面的函数功能是读入模型的除了动画帧之外的其它所有数据
void CLoadMD2::ReadMD2Data()
{
	// 为帧动画创建一个比较大的存储空间
	unsigned char buffer[MD2_MAX_FRAMESIZE];
	int j = 0;

	// 为所有的头部信息结构体分配存储空间
	m_pSkins     = new tMd2Skin [m_Header.numSkins];
	m_pTexCoords = new tMd2TexCoord [m_Header.numTexCoords];
	m_pTriangles = new tMd2Face [m_Header.numTriangles];
	m_pFrames    = new tMd2Frame [m_Header.numFrames];

	// 定位文件指针于模型的皮肤数据处
	fseek(m_FilePointer, m_Header.offsetSkins, SEEK_SET);
	
	// 根据皮肤数量，读入模型的皮肤数据
	fread(m_pSkins, sizeof(tMd2Skin), m_Header.numSkins, m_FilePointer);
	
	// 将文件指针移到纹理坐标处
	fseek(m_FilePointer, m_Header.offsetTexCoords, SEEK_SET);
	
	// 读入所有的纹理坐标
	fread(m_pTexCoords, sizeof(tMd2TexCoord), m_Header.numTexCoords, m_FilePointer);

	// 将文件指针移动到三角形/面数据处
	fseek(m_FilePointer, m_Header.offsetTriangles, SEEK_SET);
	
	// 读入每个三角形的面数据(顶点和纹理坐标)
	fread(m_pTriangles, sizeof(tMd2Face), m_Header.numTriangles, m_FilePointer);
			
	// 将文件指针移动到动画帧处
	fseek(m_FilePointer, m_Header.offsetFrames, SEEK_SET);

	// 将帧数据结构体指针指向内存缓冲区
	tMd2AliasFrame *pFrame = (tMd2AliasFrame *) buffer;

	// 分配存储空间
	m_pFrames[0].pVertices = new tMd2Triangle [m_Header.numVertices];

	// 读入动画的第一帧
	fread(pFrame, 1, m_Header.frameSize, m_FilePointer);

	// 将动画的名称拷贝到帧队列中
	strcpy(m_pFrames[0].strName, pFrame->name);
			
	tMd2Triangle *pVertices = m_pFrames[0].pVertices;

	//  遍历所有的顶点
	for (j=0; j < m_Header.numVertices; j++)
	{
		pVertices[j].vertex[0] = pFrame->aliasVertices[j].vertex[0] * pFrame->scale[0] + pFrame->translate[0];
		pVertices[j].vertex[2] = -1 * (pFrame->aliasVertices[j].vertex[1] * pFrame->scale[1] + pFrame->translate[1]);
		pVertices[j].vertex[1] = pFrame->aliasVertices[j].vertex[2] * pFrame->scale[2] + pFrame->translate[2];
	}
}


//  下面的函数的功能是将MD2结构体转换到OpenGL模型结构中
void CLoadMD2::ConvertDataStructures(t3DModel *pModel)
{
	int j = 0, i = 0;
	
	//  由于只读入了动画的一帧，因此模型中只有一个对象
	pModel->numOfObjects = 1;

	// 创建一个对象结构体保存第一帧的数据
	t3DObject currentFrame = {0};

	// 将顶点、纹理坐标和面数量赋给新的结构体
	currentFrame.numOfVerts   = m_Header.numVertices;
	currentFrame.numTexVertex = m_Header.numTexCoords;
	currentFrame.numOfFaces   = m_Header.numTriangles;

	// 分配存储空间
	currentFrame.pVerts    = new CVector3 [currentFrame.numOfVerts];
	currentFrame.pTexVerts = new CVector2 [currentFrame.numTexVertex];
	currentFrame.pFaces    = new tFace [currentFrame.numOfFaces];

	// 遍历所有的顶点，将所有的顶点赋予新的结构体中
	for (j=0; j < currentFrame.numOfVerts; j++)
	{
		currentFrame.pVerts[j].x = m_pFrames[0].pVertices[j].vertex[0];
		currentFrame.pVerts[j].y = m_pFrames[0].pVertices[j].vertex[1];
		currentFrame.pVerts[j].z = m_pFrames[0].pVertices[j].vertex[2];
	}

	// 删除使用的空间
	delete m_pFrames[0].pVertices;

	//  遍历所有的UV坐标，将所有的纹理坐标赋给新的结构体中
	for (j=0; j < currentFrame.numTexVertex; j++)
	{
		currentFrame.pTexVerts[j].x = m_pTexCoords[j].u / float(m_Header.skinWidth);
		currentFrame.pTexVerts[j].y = 1 - m_pTexCoords[j].v / float(m_Header.skinHeight);
	}

	// 遍历所有的面数据，将其赋给新的结构体中
	for(j=0; j < currentFrame.numOfFaces; j++)
	{
		// 将顶点数据赋给结构体中
		currentFrame.pFaces[j].vertIndex[0] = m_pTriangles[j].vertexIndices[0];
		currentFrame.pFaces[j].vertIndex[1] = m_pTriangles[j].vertexIndices[1];
		currentFrame.pFaces[j].vertIndex[2] = m_pTriangles[j].vertexIndices[2];

		// 将纹理坐标赋给结构体中
		currentFrame.pFaces[j].coordIndex[0] = m_pTriangles[j].textureIndices[0];
		currentFrame.pFaces[j].coordIndex[1] = m_pTriangles[j].textureIndices[1];
		currentFrame.pFaces[j].coordIndex[2] = m_pTriangles[j].textureIndices[2];
	}

	// 将当前的对象添加到对象列表中
	pModel->pObject.push_back(currentFrame);
}


//  下面的函数功能是清除占用的内存空间，并关闭文件
void CLoadMD2::CleanUp()
{
	//  关闭文件
	fclose(m_FilePointer);						

	if(m_pSkins)	 delete [] m_pSkins;		// 删除皮肤数据
	if(m_pTexCoords) delete m_pTexCoords;		// 删除皮肤坐标数据
	if(m_pTriangles) delete m_pTriangles;		// 删除三角形面数据
	if(m_pFrames)	 delete m_pFrames;			// 删除动画帧数据
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
void CLoadMD2::ComputeNormals(t3DModel *pModel)
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


