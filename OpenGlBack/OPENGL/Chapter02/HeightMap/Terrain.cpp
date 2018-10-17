
#include "main.h"

//  ���ļ���Ҫ����������������йصĺ������ڵ��ε���Ⱦ������ʹ�õ�����������Ƭ���������ı�����Ƭ��
//  ����ĺ����Ĺ����ǻ�ȡ�߳�ֵ
int Height(BYTE *pHeightMap, int X, int Y)
{
	// ȷ��x,yû�г��������С
	int x = X % MAP_SIZE;
	int y = Y % MAP_SIZE;

	if(!pHeightMap) return 0;				// ȷ���߳����ݴ���

	// ��ȡ�߳�ֵ
	return pHeightMap[x + (y * MAP_SIZE)];	
}


//  ����ĺ��������Ǹ���X��Z�������õ�ǰλ�õ���������
void SetTextureCoord(float x, float z)
{
	// ������ǰx��zλ�ô�����������
	glTexCoord2f(   (float)x / (float)MAP_SIZE,	
				  - (float)z / (float)MAP_SIZE	);
}


//  ����ĺ����Ĺ���������������Ƭ����ʽ��Ⱦ����
void RenderHeightMap(BYTE pHeightMap[])
{
	int X = 0, Y = 0;						// ���ε������λ��
	int x, y, z;
	bool bSwitchSides = false;

	// �жϸ߳������Ƿ����
	if(!pHeightMap) return;		
	
	// ��������
	glBindTexture(GL_TEXTURE_2D, g_Texture[0]);

	// ����������Ƭ����ʽ���Ƶ���
	glBegin( GL_TRIANGLE_STRIP );			

	// �����߳�������������
	for ( X = 0; X <= MAP_SIZE; X += STEP_SIZE )
	{
		// �ж���Ⱦ����
		if(bSwitchSides)
		{	
			//  ����������
			for ( Y = MAP_SIZE; Y >= 0; Y -= STEP_SIZE )
			{
				// ��ø߳�ֵ		
				x = X;							
				y = Height(pHeightMap, X, Y );	
				z = Y;							

				// ���õ�ǰ����������
				SetTextureCoord( (float)x, (float)z );
				glVertex3i(x, y, z);		

				// ��ø߳�ֵ		
				x = X + STEP_SIZE; 
				y = Height(pHeightMap, X + STEP_SIZE, Y ); 
				z = Y;

				// ���õ�ǰ����������
				SetTextureCoord( (float)x, (float)z );
				glVertex3i(x, y, z);			
			}
		}
		else
		{	
			//  �������е���
			for ( Y = 0; Y <= MAP_SIZE; Y += STEP_SIZE )
			{
				// ��ø߳�ֵ		
				x = X + STEP_SIZE; 
				y = Height(pHeightMap, X + STEP_SIZE, Y ); 
				z = Y;

				// ������������
				SetTextureCoord( (float)x, (float)z );
				glVertex3i(x, y, z);

				// ��ø߳�ֵ		
				x = X;							
				y = Height(pHeightMap, X, Y );	
				z = Y;							

				// ���õ�ǰ����������
				SetTextureCoord( (float)x, (float)z );
				glVertex3i(x, y, z);		
			}
		}

		bSwitchSides = !bSwitchSides;
	}

	// �������
	glEnd();
}

//  ����ĺ����Ĺ����ǽ�raw�ļ��е����ݶ�����һ���ֽڶ����У����е�ֵ��Ϊ�߳�ֵ��
void LoadRawFile(LPSTR strName, int nSize, BYTE *pHeightMap)
{
	FILE *pFile = NULL;

	// ��ֻ��/������ģʽ���ļ�
	pFile = fopen( strName, "rb" );

	// �ж��ļ��Ƿ����
	if ( pFile == NULL )	
	{
		// ����ļ�û�д򿪣���ʾ������Ϣ
		MessageBox(NULL, "Can't find the height map!", "Error", MB_OK);
		return;
	}

	// �����ݶ��������ݶ�����
	fread( pHeightMap, 1, nSize, pFile );

	// �жϲ����Ƿ�ɹ�
	int result = ferror( pFile );

	// ����յ�������Ϣ
	if (result)
	{
		MessageBox(NULL, "Can't get data!", "Error", MB_OK);
	}

	// �ر��ļ�
	fclose(pFile);
}


