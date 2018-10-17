/********************************************************************

	tif_win32.c

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
			http://iseeexplorer.cosoft.org.cn

	���ŵ���

			yzfree##sina.com
	----------------------------------------------------------------
	���ļ���;��	ISeeͼ���������TIFFͼ���дģ��WIN32�ļ�IO����ʵ��

	���ļ���д�ˣ�	YZ				yzfree##yeah.net

	���ļ��汾��	20418
	����޸��ڣ�	2002-4-18

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
			
	������ʷ��

		2001-4		��һ��������


********************************************************************/


#if defined(_WIN32)

#include <windows.h>
#include <assert.h>

#ifdef __BORLANDC__
#include <mem.h>
#endif

#include "tiffiop.h"
#include "iseeio.h"


#if defined(_DEBUG) && defined(_MSC_VER)
static char mes[1024];			/* ��������ʹ�õĻ����� */
#endif



/*******************************************************************/
/* ����ļ��޸���libtiff�е�tif_win32.c��ԭʼ������ļ��в�ע����Ϣ*/
/*													  YZ 2002-4	   */
/*******************************************************************/


/* �� */
static tsize_t _tiffReadProc(thandle_t fd, tdata_t buf, tsize_t size)
{
	return (tsize_t)isio_read(buf, 1, size, (ISFILE *)fd);
}


/* д */
static tsize_t _tiffWriteProc(thandle_t fd, tdata_t buf, tsize_t size)
{
	return (tsize_t)isio_write((const void *)buf, 1, size, (ISFILE *)fd);
}


/* ��λ�����ص�ǰ��λ�㣬����ANSI C������Щ������ */
static toff_t _tiffSeekProc(thandle_t fd, toff_t off, int whence)
{
	if( off == 0xFFFFFFFF )
		return 0xFFFFFFFF;

	if (isio_seek((ISFILE*)fd, off, whence) == -1)
	{
		return 0xFFFFFFFF;
	}

	return (toff_t)isio_tell((ISFILE*)fd);
}


/* �ر� */
static int _tiffCloseProc(thandle_t fd)
{
	return (int)(isio_close((ISFILE*)fd)) ? -1:0;
}


/* ������ߴ� */
static toff_t _tiffSizeProc(thandle_t fd)
{
	toff_t	tot = (toff_t)isio_tell((ISFILE*)fd);
	toff_t	tod = (toff_t)isio_seek((ISFILE*)fd, 0, SEEK_END);
	
	isio_seek((ISFILE*)fd, tot, SEEK_SET);

	return tod;
}


/* �ݲ�֧���ļ�ӳ�� */
static int _tiffDummyMapProc(thandle_t fd, tdata_t* pbase, toff_t* psize)
{
	return 0;
}
static int _tiffMapProc(thandle_t fd, tdata_t* pbase, toff_t* psize)
{
	return 0;
}
static void _tiffDummyUnmapProc(thandle_t fd, tdata_t base, toff_t size)
{
}
static void _tiffUnmapProc(thandle_t fd, tdata_t base, toff_t size)
{
}


/* ����TIFF�ṹ������ָ���������� */
TIFF* TIFFFdOpen(int ifd, const char* name, const char* mode)
{
	TIFF* tif;

	tif = TIFFClientOpen(name, mode, (thandle_t)ifd, _tiffReadProc, _tiffWriteProc,
							_tiffSeekProc, _tiffCloseProc, _tiffSizeProc, 
							_tiffMapProc, _tiffUnmapProc);
	if (tif)
		tif->tif_fd = ifd;

	return (tif);
}


/* ��TIFFͼ�����������ع�����TIFF�ṹ */
TIFF* TIFFOpen(const char* name, const char* mode)
{
	static const char module[] = "TIFFOpen";

	ISFILE	*fp;

	/* �˴���ʹ����ڲ��������mode������ΪTIFF��mode����ANSI C�ļ�ģʽ�����岻ͬ */
	/* "w" ��ʽ֮����תΪ"w+b"������Ϊlibtiff����дͼ��ʱ��Ȼ����ö����� */
	if ((fp=isio_open(name, ((mode[0] == 'w') ? "w+b" : "rb"))) == 0)
	{
		TIFFError(module, "%s: Cannot open", name);
		return (TIFF*)0;
	}
	else
	{
		return (TIFFFdOpen((int)fp, name, mode));
	}
}


/* �����ڴ� */
tdata_t _TIFFmalloc(tsize_t s)
{
	return (tdata_t)malloc(s);
}


/* �ͷ��ڴ� */
void _TIFFfree(tdata_t p)
{
	if (p)
		free(p);
}


/* �ط����ڴ� */
tdata_t _TIFFrealloc(tdata_t p, tsize_t s)
{
	return (tdata_t)realloc(p, s);
}


/* �ڴ����� */
void _TIFFmemset(void* p, int v, tsize_t c)
{
	assert(p);
	memset(p, v, c);
}


/* �ڴ濽�� */
void _TIFFmemcpy(void* d, const tdata_t s, tsize_t c)
{
	assert(d);
	memcpy(d, (const void *)s, c);
}


/* �ڴ�Ƚ� */
int _TIFFmemcmp(const tdata_t p1, const tdata_t p2, tsize_t c)
{
	assert(p1&&p2);
	return memcmp((const void *)p1, (const void *)p2, c);
}


/* ISee TIFF������洦����(for windows) */
static void ISeeWarningHandler(const char* module, const char* fmt, va_list ap)
{
#if defined(_DEBUG) && defined(_MSC_VER)
	char *p;
	int i1, i2;
	static char *conc = "[TIFF W] ---------> ";
	
	p   = va_arg(ap, char*);
	i1	= va_arg(ap, int);
	i2	= va_arg(ap, int);

	sprintf(mes, fmt, p, i1, i2);
	strcat((char*)mes, (const char *)".\n");

	memmove((void*)(mes+strlen(conc)), (const void *)mes, strlen(mes));
	memmove((void*)mes, (const void *)conc, strlen(conc));

	/* ����Ϣд��VC++�ĵ��������� */
	OutputDebugStringA(mes);
#endif
	return;
}
TIFFErrorHandler _TIFFwarningHandler = ISeeWarningHandler;


/* ISee TIFF�����������(for windows) */
static void ISeeErrorHandler(const char* module, const char* fmt, va_list ap)
{
#if defined(_DEBUG) && defined(_MSC_VER)
	char *p;
	int i1, i2;
	static char *conc2 = "[TIFF E] ---------> ";
	
	p   = va_arg(ap, char*);
	i1	= va_arg(ap, int);
	i2	= va_arg(ap, int);
	
	sprintf(mes, fmt, p, i1, i2);
	strcat((char*)mes, (const char *)".\n");
	
	memmove((void*)(mes+strlen(conc2)), (const void *)mes, strlen(mes));
	memmove((void*)mes, (const void *)conc2, strlen(conc2));
	
	OutputDebugStringA(mes);
#else
	(void)module; (void)fmt; (void)ap;
#endif
	return;
}
TIFFErrorHandler _TIFFerrorHandler = ISeeErrorHandler;

#endif	/* _WIN32 */





/*******************************************************************/
/* �Ӵ�����Ϊ libtiff �� tif_win32.c ԭʼ���롣					   */
/*													YZ 2002-4	   */
/*******************************************************************/

#if 0

/* $Header: /cvsroot/osrs/libtiff/libtiff/tif_win32.c,v 1.6 2000/04/04 14:54:34 mwelles Exp $ */

/*
 * Copyright (c) 1988-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

/*
 * TIFF Library Win32-specific Routines.  Adapted from tif_unix.c 4/5/95 by
 * Scott Wagner (wagner@itek.com), Itek Graphix, Rochester, NY USA
 */

#include <windows.h>
#include "tiffiop.h"


static tsize_t
_tiffReadProc(thandle_t fd, tdata_t buf, tsize_t size)
{
	DWORD dwSizeRead;
	if (!ReadFile(fd, buf, size, &dwSizeRead, NULL))
		return(0);
	return ((tsize_t) dwSizeRead);
}

static tsize_t
_tiffWriteProc(thandle_t fd, tdata_t buf, tsize_t size)
{
	DWORD dwSizeWritten;
	if (!WriteFile(fd, buf, size, &dwSizeWritten, NULL))
		return(0);
	return ((tsize_t) dwSizeWritten);
}

static toff_t
_tiffSeekProc(thandle_t fd, toff_t off, int whence)
{
	DWORD dwMoveMethod, dwMoveHigh;

        /* we use this as a special code, so avoid accepting it */
        if( off == 0xFFFFFFFF )
            return 0xFFFFFFFF;
        
	switch(whence)
	{
	case SEEK_SET:
		dwMoveMethod = FILE_BEGIN;
		break;
	case SEEK_CUR:
		dwMoveMethod = FILE_CURRENT;
		break;
	case SEEK_END:
		dwMoveMethod = FILE_END;
		break;
	default:
		dwMoveMethod = FILE_BEGIN;
		break;
	}
        dwMoveHigh = 0;
	return ((toff_t)SetFilePointer(fd, (LONG) off, (PLONG)&dwMoveHigh,
                                       dwMoveMethod));
}

static int
_tiffCloseProc(thandle_t fd)
{
	return (CloseHandle(fd) ? 0 : -1);
}

static toff_t
_tiffSizeProc(thandle_t fd)
{
	return ((toff_t)GetFileSize(fd, NULL));
}

#ifdef __BORLANDC__
#pragma argsused
#endif
static int
_tiffDummyMapProc(thandle_t fd, tdata_t* pbase, toff_t* psize)
{
	return (0);
}

/*
 * From "Hermann Josef Hill" <lhill@rhein-zeitung.de>:
 *
 * Windows uses both a handle and a pointer for file mapping,
 * but according to the SDK documentation and Richter's book
 * "Advanced Windows Programming" it is safe to free the handle
 * after obtaining the file mapping pointer
 *
 * This removes a nasty OS dependency and cures a problem
 * with Visual C++ 5.0
 */
static int
_tiffMapProc(thandle_t fd, tdata_t* pbase, toff_t* psize)
{
	toff_t size;
	HANDLE hMapFile;

	if ((size = _tiffSizeProc(fd)) == 0xFFFFFFFF)
		return (0);
	hMapFile = CreateFileMapping(fd, NULL, PAGE_READONLY, 0, size, NULL);
	if (hMapFile == NULL)
		return (0);
	*pbase = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hMapFile);
	if (*pbase == NULL)
		return (0);
	*psize = size;
	return(1);
}

#ifdef __BORLANDC__
#pragma argsused
#endif
static void
_tiffDummyUnmapProc(thandle_t fd, tdata_t base, toff_t size)
{
}

static void
_tiffUnmapProc(thandle_t fd, tdata_t base, toff_t size)
{
	UnmapViewOfFile(base);
}

/*
 * Open a TIFF file descriptor for read/writing.
 * Note that TIFFFdOpen and TIFFOpen recognise the character 'u' in the mode
 * string, which forces the file to be opened unmapped.
 */
TIFF*
TIFFFdOpen(int ifd, const char* name, const char* mode)
{
	TIFF* tif;
	BOOL fSuppressMap = (mode[1] == 'u' || (mode[1]!=0 && mode[2] == 'u'));

	tif = TIFFClientOpen(name, mode,
		 (thandle_t)ifd,
	    _tiffReadProc, _tiffWriteProc,
	    _tiffSeekProc, _tiffCloseProc, _tiffSizeProc,
		 fSuppressMap ? _tiffDummyMapProc : _tiffMapProc,
		 fSuppressMap ? _tiffDummyUnmapProc : _tiffUnmapProc);
	if (tif)
		tif->tif_fd = ifd;
	return (tif);
}

/*
 * Open a TIFF file for read/writing.
 */
TIFF*
TIFFOpen(const char* name, const char* mode)
{
	static const char module[] = "TIFFOpen";
	thandle_t fd;
	int m;
	DWORD dwMode;

	m = _TIFFgetMode(mode, module);

	switch(m)
	{
	case O_RDONLY:
		dwMode = OPEN_EXISTING;
		break;
	case O_RDWR:
		dwMode = OPEN_ALWAYS;
		break;
	case O_RDWR|O_CREAT:
		dwMode = OPEN_ALWAYS;
		break;
	case O_RDWR|O_TRUNC:
		dwMode = CREATE_ALWAYS;
		break;
	case O_RDWR|O_CREAT|O_TRUNC:
		dwMode = CREATE_ALWAYS;
		break;
	default:
		return ((TIFF*)0);
	}
	fd = (thandle_t)CreateFile(name, (m == O_RDONLY) ? GENERIC_READ :
			(GENERIC_READ | GENERIC_WRITE), FILE_SHARE_READ, NULL, dwMode,
			(m == O_RDONLY) ? FILE_ATTRIBUTE_READONLY : FILE_ATTRIBUTE_NORMAL, NULL);
	if (fd == INVALID_HANDLE_VALUE) {
		TIFFError(module, "%s: Cannot open", name);
		return ((TIFF *)0);
	}
	return (TIFFFdOpen((int)fd, name, mode));
}

tdata_t
_TIFFmalloc(tsize_t s)
{
	return ((tdata_t)GlobalAlloc(GMEM_FIXED, s));
}

void
_TIFFfree(tdata_t p)
{
	GlobalFree(p);
	return;
}

tdata_t
_TIFFrealloc(tdata_t p, tsize_t s)
{
  void* pvTmp;
  tsize_t old=GlobalSize(p);
  if (old>=s)
    {
      if ((pvTmp = GlobalAlloc(GMEM_FIXED, s)) != NULL) {
	CopyMemory(pvTmp, p, s);
	GlobalFree(p);
      }
    }
  else
    {
      if ((pvTmp = GlobalAlloc(GMEM_FIXED, s)) != NULL) {
	CopyMemory(pvTmp, p, old);
	GlobalFree(p);
      }
    }
  return ((tdata_t)pvTmp);
}

void
_TIFFmemset(void* p, int v, tsize_t c)
{
	FillMemory(p, c, (BYTE)v);
}

void
_TIFFmemcpy(void* d, const tdata_t s, tsize_t c)
{
	CopyMemory(d, s, c);
}

int
_TIFFmemcmp(const tdata_t p1, const tdata_t p2, tsize_t c)
{
	register const BYTE *pb1 = (const BYTE *) p1;
	register const BYTE *pb2 = (const BYTE *) p2;
	register DWORD dwTmp = c;
	register int iTmp;
	for (iTmp = 0; dwTmp-- && !iTmp; iTmp = (int)*pb1++ - (int)*pb2++)
		;
	return (iTmp);
}


static void
Win32WarningHandler(const char* module, const char* fmt, va_list ap)
{
#ifndef TIF_PLATFORM_CONSOLE
	LPTSTR szTitle;
	LPTSTR szTmp;
	LPCTSTR szTitleText = "%s Warning";
	LPCTSTR szDefaultModule = "TIFFLIB";
	szTmp = (module == NULL) ? (LPTSTR)szDefaultModule : (LPTSTR)module;
	if ((szTitle = (LPTSTR)LocalAlloc(LMEM_FIXED, (lstrlen(szTmp) +
			lstrlen(szTitleText) + lstrlen(fmt) + 128)*sizeof(TCHAR))) == NULL)
		return;
	wsprintf(szTitle, szTitleText, szTmp);
	szTmp = szTitle + (lstrlen(szTitle)+2)*sizeof(TCHAR);
	wvsprintf(szTmp, fmt, ap);
	MessageBox(GetFocus(), szTmp, szTitle, MB_OK | MB_ICONINFORMATION);
	LocalFree(szTitle);
	return;
#else
	if (module != NULL)
		fprintf(stderr, "%s: ", module);
	fprintf(stderr, "Warning, ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, ".\n");
#endif        
}
TIFFErrorHandler _TIFFwarningHandler = Win32WarningHandler;

static void
Win32ErrorHandler(const char* module, const char* fmt, va_list ap)
{
#ifndef TIF_PLATFORM_CONSOLE
	LPTSTR szTitle;
	LPTSTR szTmp;
	LPCTSTR szTitleText = "%s Error";
	LPCTSTR szDefaultModule = "TIFFLIB";
	szTmp = (module == NULL) ? (LPTSTR)szDefaultModule : (LPTSTR)module;
	if ((szTitle = (LPTSTR)LocalAlloc(LMEM_FIXED, (lstrlen(szTmp) +
			lstrlen(szTitleText) + lstrlen(fmt) + 128)*sizeof(TCHAR))) == NULL)
		return;
	wsprintf(szTitle, szTitleText, szTmp);
	szTmp = szTitle + (lstrlen(szTitle)+2)*sizeof(TCHAR);
	wvsprintf(szTmp, fmt, ap);
	MessageBox(GetFocus(), szTmp, szTitle, MB_OK | MB_ICONEXCLAMATION);
	LocalFree(szTitle);
	return;
#else
	if (module != NULL)
		fprintf(stderr, "%s: ", module);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, ".\n");
#endif        
}
TIFFErrorHandler _TIFFerrorHandler = Win32ErrorHandler;

#endif /* 0 */
