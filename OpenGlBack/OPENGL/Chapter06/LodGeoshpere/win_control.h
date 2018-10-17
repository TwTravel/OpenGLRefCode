#ifndef __WIN_CONTROL_H__
#define __WIN_CONTROL_H__

#define VK_PAGEUP VK_PRIOR
#define VK_PAGEDOWN VK_NEXT

extern HWND hWnd;
extern char TheKey;
extern int TheSysKey;
extern BOOL b_done;
extern float fDeltaTime;
extern int pixelwidth;
extern int pixelheight;
extern float glAspect;


HWND CreateWin(MSG *msg,WNDPROC WindowProc);
int SetUpOpenGL(HWND hWnd);

void draw_screen(void);
void key_check(void);
void destroy(void);
void initialize(void);

#endif