
#include "main.h"

//  本文件主要包含了所有与地形有关的函数。在地形的渲染过程中使用的是三角形面片，而不是四边形面片。
//  下面的函数的功能是获取高程值
int Height(BYTE *pHeightMap, int X, int Y)
{
	// 确保x,y没有超过数组大小
	int x = X % MAP_SIZE;
	int y = Y % MAP_SIZE;

	if(!pHeightMap) return 0;				// 确保高程数据存在

	// 获取高程值
	return pHeightMap[x + (y * MAP_SIZE)];	
}


//  下面的函数功能是根据X和Z坐标设置当前位置的纹理坐标
void SetTextureCoord(float x, float z)
{
	// 给出当前x和z位置处的纹理坐标
	glTexCoord2f(   (float)x / (float)MAP_SIZE,	
				  - (float)z / (float)MAP_SIZE	);
}


//  下面的函数的功能是以三角形面片的形式渲染地形
void RenderHeightMap(BYTE pHeightMap[])
{
	int X = 0, Y = 0;						// 地形的网格的位置
	int x, y, z;
	bool bSwitchSides = false;

	// 判断高程数据是否存在
	if(!pHeightMap) return;		
	
	// 捆绑纹理
	glBindTexture(GL_TEXTURE_2D, g_Texture[0]);

	// 以三角形面片的形式绘制地形
	glBegin( GL_TRIANGLE_STRIP );			

	// 遍历高程数据中所有行
	for ( X = 0; X <= MAP_SIZE; X += STEP_SIZE )
	{
		// 判断渲染的面
		if(bSwitchSides)
		{	
			//  遍历所有列
			for ( Y = MAP_SIZE; Y >= 0; Y -= STEP_SIZE )
			{
				// 获得高程值		
				x = X;							
				y = Height(pHeightMap, X, Y );	
				z = Y;							

				// 设置当前的纹理坐标
				SetTextureCoord( (float)x, (float)z );
				glVertex3i(x, y, z);		

				// 获得高程值		
				x = X + STEP_SIZE; 
				y = Height(pHeightMap, X + STEP_SIZE, Y ); 
				z = Y;

				// 设置当前的纹理坐标
				SetTextureCoord( (float)x, (float)z );
				glVertex3i(x, y, z);			
			}
		}
		else
		{	
			//  遍历所有的行
			for ( Y = 0; Y <= MAP_SIZE; Y += STEP_SIZE )
			{
				// 获得高程值		
				x = X + STEP_SIZE; 
				y = Height(pHeightMap, X + STEP_SIZE, Y ); 
				z = Y;

				// 设置纹理坐标
				SetTextureCoord( (float)x, (float)z );
				glVertex3i(x, y, z);

				// 获得高程值		
				x = X;							
				y = Height(pHeightMap, X, Y );	
				z = Y;							

				// 设置当前的纹理坐标
				SetTextureCoord( (float)x, (float)z );
				glVertex3i(x, y, z);		
			}
		}

		bSwitchSides = !bSwitchSides;
	}

	// 绘制完成
	glEnd();
}

//  下面的函数的功能是将raw文件中的数据读出到一个字节队列中，其中的值就为高程值。
void LoadRawFile(LPSTR strName, int nSize, BYTE *pHeightMap)
{
	FILE *pFile = NULL;

	// 以只读/二进制模式打开文件
	pFile = fopen( strName, "rb" );

	// 判断文件是否存在
	if ( pFile == NULL )	
	{
		// 如果文件没有打开，显示错误信息
		MessageBox(NULL, "Can't find the height map!", "Error", MB_OK);
		return;
	}

	// 将数据读出到数据队列中
	fread( pHeightMap, 1, nSize, pFile );

	// 判断操作是否成功
	int result = ferror( pFile );

	// 如果收到错误信息
	if (result)
	{
		MessageBox(NULL, "Can't get data!", "Error", MB_OK);
	}

	// 关闭文件
	fclose(pFile);
}


