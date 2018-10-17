#ifndef _TERRAIN_H
#define _TERRAIN_H

#define MAP_SIZE		1024				// .raw�߳�ӳ���ļ��Ĵ�С
#define STEP_SIZE		16					// ÿ������������Ŀ�Ⱥ͸߶�
#define HEIGHT_RATIO	1.5f				// This is the ratio that the Y is scaled according to the X and Z

// ���ݸ�����X��Y���꣬�Ӹ߳�ӳ�������л�ø߳�ֵ
int Height(BYTE *pHeightMap, int X, int Y);

// ��.raw�ļ���װ��߳�ӳ������
void LoadRawFile(LPSTR strName, int nSize, BYTE *pHeightMap);

// ���ݸ߳�ӳ�����ݻ���ͼ��
void RenderHeightMap(BYTE *pHeightMap);


#endif


