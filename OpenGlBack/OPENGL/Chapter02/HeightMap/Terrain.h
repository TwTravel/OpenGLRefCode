#ifndef _TERRAIN_H
#define _TERRAIN_H

#define MAP_SIZE		1024				// .raw高程映射文件的大小
#define STEP_SIZE		16					// 每个三角形网格的宽度和高度
#define HEIGHT_RATIO	1.5f				// This is the ratio that the Y is scaled according to the X and Z

// 根据给定的X和Y坐标，从高程映射数据中获得高程值
int Height(BYTE *pHeightMap, int X, int Y);

// 从.raw文件中装入高程映射数据
void LoadRawFile(LPSTR strName, int nSize, BYTE *pHeightMap);

// 根据高程映射数据绘制图形
void RenderHeightMap(BYTE *pHeightMap);


#endif


