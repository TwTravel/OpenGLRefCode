// FontGL.h: interface for the CFontGL class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FONTGL_H__B28DBAD5_CFBC_49E8_8CDE_0F5214EC657C__INCLUDED_)
#define AFX_FONTGL_H__B28DBAD5_CFBC_49E8_8CDE_0F5214EC657C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFontGL  
{
public:
	CFontGL();
	virtual ~CFontGL();

	void BuildFont(struct HDC__ *hhdc);				
	void glPrint(const char *fmt, ...);
	void KillFont(void);
	
	GLuint  m_iBaseFont;
};

#endif // !defined(AFX_FONTGL_H__B28DBAD5_CFBC_49E8_8CDE_0F5214EC657C__INCLUDED_)
