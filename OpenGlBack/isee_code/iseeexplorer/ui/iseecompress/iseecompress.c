/********************************************************************

	iseecompress.c

	----------------------------------------------------------------
    软件许可证 － GPL
	版权所有 (C) 2001 VCHelp coPathway ISee workgroup.
	----------------------------------------------------------------
	这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用公共许
	可证条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根
	据你的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用公共许可证。

    你应该已经和程序一起收到一份GNU通用公共许可证的副本（本目录
	GPL.txt文件）。如果还没有，写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA
	----------------------------------------------------------------
	如果你在使用本软件时有什么问题或建议，请用以下地址与我们取得联系：

			http://isee.126.com
			http://www.vchelp.net
			http://www.chinafcu.com

	或发信到：

			yzfree##sina.com
	----------------------------------------------------------------
	本文件用途：	ISee图像浏览器—压缩及解压模块(X-Todo A03)
	
	本文件编写人：	wenbobo		wenbobo##263.net
		
	本文件版本：	11207
	最后修改于：	2001-12-07
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		  2001-12		第一次发布


	----------------------------------------------------------------
	注：本代码使用了UCL(实时数据压缩库)，在此向UCL的开发者们表示感谢！
	----------------------------------------------------------------
  
********************************************************************/


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ucl.h"
#include "uclconf.h"
#include "uclutil.h"

#include "ISeeCompress.h"

#define HEAP_ALLOC(var,size) \
	long __UCL_MMODEL var [ ((size) + (sizeof(long) - 1)) / sizeof(long) ]


/*
* Compress level:
*	0 - do not compress :)
*	1 - fasted
*	2
*	3
*	4
*	5 - normal
*	6
*	7
*	8
*	9 - best but slow :(
*/

int isc_compresslevel = 6;		/* 内部设定压缩级为 6 */



/* 压缩函数原型描述：

	用途：压缩调用者传入的数据，并返回压缩后的数据及长度。

	int ISeeCompress( LPBYTE lpSouData, DWORD dwSouDataLen, 
			LPBYTE *lppCompressData, 
			DWORD  *lpdwCompressLen );

	入口参数说明：
			LPBYTE lpSouData	- 待压缩的数据缓冲区首地址。
			DWORD dwSouDataLen	- 待压缩数据的长度（以字节为单位）。
			LPBYTE *lppCompressData	- 调用者用于接收已压缩数据缓冲区
									  首地址的变量地址。
			DWORD  *lpdwCompressLen	- 调用者用于接收已压缩数据缓冲区
									  尺寸的变量地址。

	返回值：
			0	- 压缩成功
			-1	- UCL库初始化失败
			-2  - 内存分配失败（内存不足）
			-3  - 数据是已被压缩过的（无法再压缩了）
			-4  - UCL压缩函数执行失败
			-5  - 重分配缓冲区失败（由Realloc函数引起）

	注释：
		调用者将把待压缩的数据（在一个内存缓冲区中）经lpSouData传入本函数，并
		将这个缓冲区的尺寸也通过dwSouDataLen 一同传入。函数内部先用CRT函数
		malloc()分配一个大于或等于待压缩数据缓冲区的内存块，然后进行压缩。 压
		缩后的数据放在新分配的内存块中。 压缩结束以后，再根据实际压缩数据的尺
		寸用realloc()将内存块缩小至匹配尺寸。最后，将内存块的首地址填入参数
		lppCompressData指向的地址中，压缩数据的尺寸值填入lpdwCompressLen 指向
		的地址。如果压缩成功，函数返回0 ，如果失败返回非0值。
*/

int CALLAGREEMENT ISeeCompress(LPBYTE lpSouData, 
								 DWORD dwSouDataLen, 
								 LPBYTE *lppCompressData, 
								 DWORD  *lpdwCompressLen)
{
	int r;
	
	/*
	*Step 1: initialize the UCL library
	*/
    if (ucl_init() != UCL_E_OK)
    {
#ifdef _DEBUG
        fprintf(stderr, "Error: ucl_init()\n");
#endif
        return -1;
    }

    /*
 	*Step 2: allocate blocks
 	*/
    *lppCompressData = malloc(dwSouDataLen*3/2);
    if (*lppCompressData == 0)
    {
#ifdef _DEBUG
    	fprintf(stderr, "Error: *lppCompressData = malloc(0x%08X)\n", dwSouDataLen);
#endif
        return -2;
    }

    /*
 	*Step 3: compress from `lpSouData' to `lpCompressData' with UCL NRV2B
 	*/
    r = ucl_nrv2b_99_compress(lpSouData, dwSouDataLen, *lppCompressData, lpdwCompressLen, NULL, isc_compresslevel, NULL, NULL);
    if (r == UCL_E_OK)
    {
    	if ((*lpdwCompressLen) >= dwSouDataLen)
    	{
#ifdef _DEBUG
    		fprintf(stderr, "Hint: data is already been compressed\n");
#endif
    		return -3;
    	}
    	else
    	{
#ifdef _DEBUG
        	fprintf(stderr, 
        		"Hint: compressed %lu bytes into %lu bytes\n",
            	(long)dwSouDataLen, (long)*lpdwCompressLen );
#endif
        }
    }
    else
    {
        /*This should NEVER happen*/
#ifdef _DEBUG
        fprintf(stderr, "Error: r = ucl_nrv2b_99_compress(...) return 0x%08X, compression failed\n", r);
#endif
        return -4;
    }
    
    /*
 	*Step 4: Reallocate output buffer size
 	*/
 	*lppCompressData = realloc(*lppCompressData, *lpdwCompressLen);
 	if((*lppCompressData) ==  NULL)
	{
		/*This should NEVER happen*/
#ifdef _DEBUG
        fprintf(stderr, "Error: *lppCompressData = realloc(*lppCompressData, *lpdwCompressLen)\n");
#endif
        return -5;
	}
 	
    return 0;
}





/* 解压缩函数原型描述：

	用途：将用户指定的、由ISeeCompress()函数压缩的数据解压到新的内存缓冲区中。

	int ISeeDecompress( LPBYTE lpCompressData, DWORD dwCompressLen, 
				LPBYTE *lppSouData, DWORD dwSouDataLen );

	入口参数说明：
			LPBYTE lpCompressData	- 待解压的数据 
			DWORD dwCompressLen		- 待解压数据的尺寸
			LPBYTE *lppSouData		- 解压后的数据缓冲区首地址
			DWORD  dwSouDataLen	    - 解压后的数据应具有的尺寸

	返回值：
			0   - 解压成功
			-1  - 失败（由内存不足引起）
			-2  - 失败（压缩数据有缺损或该数据不是由ISeeCompress()函数
				  压缩的）
			-3	- UCL库初始化失败

	注释：
		本函数将由ISeeCompress()函数压缩的数据解压到一个新分配的内存缓冲区中，
		待解压的数据缓冲区地址、长度、及解压后数据应具有的长度值由入口参数
		lpCompressData、dwCompressLen、lpdwSouDataLen传入。本函数先根据
		lpdwSouDataLen的值申请一个内存缓冲区，然后将lpCompressData中的数据解
		压到新的缓冲区中。如果解压成功，函数返回0 ，如果内存申请失败，返回-1
		，如果传入的数据不是有效的压缩数据（可能是未压缩数据或不是由
		ISeeCompress()函数压缩的数据），返回-2。UCL库初始化失败返回-3。
*/

int CALLAGREEMENT ISeeDecompress(LPBYTE lpCompressData, 
								   DWORD dwCompressLen, 
								   LPBYTE *lppSouData, 
								   DWORD dwSouDataLen)
{
	int r;
	unsigned long new_len;
	
	/*
	*Step 1: initialize the UCL library
	*/
    if (ucl_init() != UCL_E_OK)
    {
#ifdef _DEBUG
        fprintf(stderr, "Error: ucl_init()\n");
#endif
        return -3;
    }

    /*
 	*Step 2: allocate blocks
 	*/
    *lppSouData = malloc(dwSouDataLen);
    if (*lppSouData == 0)
    {
#ifdef _DEBUG
    	fprintf(stderr, "Error: *lppSouData = malloc(0x%08X)\n", dwSouDataLen);
#endif
        return -1;
    }

    /*
 	*Step 3: decompres from `lpCompressData' to `lppSouData' with UCL NRV2B
 	*/
 	r = ucl_nrv2b_decompress_8(lpCompressData,dwCompressLen,*lppSouData,&new_len,NULL);
 	if (r == UCL_E_OK)
 	{
		if (new_len == dwSouDataLen)
		{
#ifdef _DEBUG
        	fprintf(stderr,
        		"Hint: decompressed %lu bytes back into %lu bytes\n",
            	(long) dwCompressLen, (long) dwSouDataLen );
#endif
        }
        else
        {
#ifdef _DEBUG
        	fprintf(stderr,
        		"Error: decompressed %lu bytes back into %lu bytes(but should be %lu bytes)\n",
            	(long) dwCompressLen, new_len, (long) dwSouDataLen );
#endif
        	return -2;
        }
    }
    else
    {
        /* this should NEVER happen */
#ifdef _DEBUG
        fprintf(stderr, "Error: r = ucl_nrv2b_decompress_8(...) return 0x%08X, decompression failed\n", r);
#endif
        return -2;
    }
 	
    return 0;
}
