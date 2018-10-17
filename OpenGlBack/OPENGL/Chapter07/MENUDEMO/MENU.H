#include "mload.h"

#define CURSOR_T 0
#define CURSOR_TRANSP_T 1
#define BORDER1_T 2
#define BORDER2_T 3
#define BORDER3_T 4
#define PYRAMID_T 6
#define CUBE_T 7
#define SPHERE_T 8
#define CBOX_T 9
#define SPINDLE_T 10
#define TORUS_T 11
#define LINE_T 12
#define QUIT_T 13

GLuint texture[14];					// ÎÆÀí
int font_techno, font_alpine;		// ×ÖÌå
struct model models[7];

void InitMenu();
void doRedraw(HDC *hDC);
void check_menu(int state, int x, int y, int ww, int wh, HWND hWnd);
