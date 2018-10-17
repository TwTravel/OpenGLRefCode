// FontGL.cpp: implementation of the CFontGL class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tetris.h"
#include "FontGL.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFontGL::CFontGL()
{

}

CFontGL::~CFontGL()
{

}

void CFontGL::BuildFont(struct HDC__ *hhdc)					
{
	HFONT	font;							
	//  生成显示列表的索引
	m_iBaseFont = glGenLists(96);			
	//  定义字体
	font = CreateFont(	25,					
						0,					
						0,					
						0,								
						FW_BOLD,						
						FALSE,							
						FALSE,							
						FALSE,							
						ANSI_CHARSET,					
						OUT_TT_PRECIS,					
						CLIP_DEFAULT_PRECIS,			
						ANTIALIASED_QUALITY,			
						FF_DONTCARE|DEFAULT_PITCH,		
						"ARIAL");					

	SelectObject(hhdc, font);							
	//  生成显示列表
	wglUseFontBitmaps(hhdc, 32, 96, m_iBaseFont);		
}

void CFontGL::glPrint(const char *fmt, ...)		
{
	char		text[256];							
	va_list		ap;									

	if (fmt == NULL)								
		return;										

	va_start(ap, fmt);								
	vsprintf(text, fmt, ap);						
	va_end(ap);											

	glPushAttrib(GL_LIST_BIT);							
	glListBase(m_iBaseFont - 32);						
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	
	glPopAttrib();										
}


void CFontGL::KillFont(void)					
{
	glDeleteLists(m_iBaseFont, 96);		
}
