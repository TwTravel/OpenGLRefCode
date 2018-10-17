/********************************************************************

	iseecompress.c

	----------------------------------------------------------------
    ������֤ �� GPL
	��Ȩ���� (C) 2001 VCHelp coPathway ISee workgroup.
	----------------------------------------------------------------
	��һ����������������������������������������GNU ͨ�ù�����
	��֤�������޸ĺ����·�����һ���򡣻��������֤�ĵڶ��棬���ߣ���
	�����ѡ�����κθ��µİ汾��

    ������һ�����Ŀ����ϣ�������ã���û���κε���������û���ʺ��ض�
	Ŀ�ص������ĵ���������ϸ����������GNUͨ�ù������֤��

    ��Ӧ���Ѿ��ͳ���һ���յ�һ��GNUͨ�ù������֤�ĸ�������Ŀ¼
	GPL.txt�ļ����������û�У�д�Ÿ���
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA
	----------------------------------------------------------------
	�������ʹ�ñ����ʱ��ʲô������飬�������µ�ַ������ȡ����ϵ��

			http://isee.126.com
			http://www.vchelp.net
			http://www.chinafcu.com

	���ŵ���

			yzfree##sina.com
	----------------------------------------------------------------
	���ļ���;��	ISeeͼ���������ѹ������ѹģ��(X-Todo A03)
	
	���ļ���д�ˣ�	wenbobo		wenbobo##263.net
		
	���ļ��汾��	11207
	����޸��ڣ�	2001-12-07
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		  2001-12		��һ�η���


	----------------------------------------------------------------
	ע��������ʹ����UCL(ʵʱ����ѹ����)���ڴ���UCL�Ŀ������Ǳ�ʾ��л��
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

int isc_compresslevel = 6;		/* �ڲ��趨ѹ����Ϊ 6 */



/* ѹ������ԭ��������

	��;��ѹ�������ߴ�������ݣ�������ѹ��������ݼ����ȡ�

	int ISeeCompress( LPBYTE lpSouData, DWORD dwSouDataLen, 
			LPBYTE *lppCompressData, 
			DWORD  *lpdwCompressLen );

	��ڲ���˵����
			LPBYTE lpSouData	- ��ѹ�������ݻ������׵�ַ��
			DWORD dwSouDataLen	- ��ѹ�����ݵĳ��ȣ����ֽ�Ϊ��λ����
			LPBYTE *lppCompressData	- ���������ڽ�����ѹ�����ݻ�����
									  �׵�ַ�ı�����ַ��
			DWORD  *lpdwCompressLen	- ���������ڽ�����ѹ�����ݻ�����
									  �ߴ�ı�����ַ��

	����ֵ��
			0	- ѹ���ɹ�
			-1	- UCL���ʼ��ʧ��
			-2  - �ڴ����ʧ�ܣ��ڴ治�㣩
			-3  - �������ѱ�ѹ�����ģ��޷���ѹ���ˣ�
			-4  - UCLѹ������ִ��ʧ��
			-5  - �ط��仺����ʧ�ܣ���Realloc��������

	ע�ͣ�
		�����߽��Ѵ�ѹ�������ݣ���һ���ڴ滺�����У���lpSouData���뱾��������
		������������ĳߴ�Ҳͨ��dwSouDataLen һͬ���롣�����ڲ�����CRT����
		malloc()����һ�����ڻ���ڴ�ѹ�����ݻ��������ڴ�飬Ȼ�����ѹ���� ѹ
		��������ݷ����·�����ڴ���С� ѹ�������Ժ��ٸ���ʵ��ѹ�����ݵĳ�
		����realloc()���ڴ����С��ƥ��ߴ硣��󣬽��ڴ����׵�ַ�������
		lppCompressDataָ��ĵ�ַ�У�ѹ�����ݵĳߴ�ֵ����lpdwCompressLen ָ��
		�ĵ�ַ�����ѹ���ɹ�����������0 �����ʧ�ܷ��ط�0ֵ��
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





/* ��ѹ������ԭ��������

	��;�����û�ָ���ġ���ISeeCompress()����ѹ�������ݽ�ѹ���µ��ڴ滺�����С�

	int ISeeDecompress( LPBYTE lpCompressData, DWORD dwCompressLen, 
				LPBYTE *lppSouData, DWORD dwSouDataLen );

	��ڲ���˵����
			LPBYTE lpCompressData	- ����ѹ������ 
			DWORD dwCompressLen		- ����ѹ���ݵĳߴ�
			LPBYTE *lppSouData		- ��ѹ������ݻ������׵�ַ
			DWORD  dwSouDataLen	    - ��ѹ�������Ӧ���еĳߴ�

	����ֵ��
			0   - ��ѹ�ɹ�
			-1  - ʧ�ܣ����ڴ治������
			-2  - ʧ�ܣ�ѹ��������ȱ�������ݲ�����ISeeCompress()����
				  ѹ���ģ�
			-3	- UCL���ʼ��ʧ��

	ע�ͣ�
		����������ISeeCompress()����ѹ�������ݽ�ѹ��һ���·�����ڴ滺�����У�
		����ѹ�����ݻ�������ַ�����ȡ�����ѹ������Ӧ���еĳ���ֵ����ڲ���
		lpCompressData��dwCompressLen��lpdwSouDataLen���롣�������ȸ���
		lpdwSouDataLen��ֵ����һ���ڴ滺������Ȼ��lpCompressData�е����ݽ�
		ѹ���µĻ������С������ѹ�ɹ�����������0 ������ڴ�����ʧ�ܣ�����-1
		�������������ݲ�����Ч��ѹ�����ݣ�������δѹ�����ݻ�����
		ISeeCompress()����ѹ�������ݣ�������-2��UCL���ʼ��ʧ�ܷ���-3��
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
