#include <windows.h>
#include <string.h>
#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "menu.h"

#define TOTAL_MI 9				// 菜单项总数

struct menu_item
{
	char flag;						// 标志
	char pressed;					// 菜单项状态
	float posx, posy;				// 菜单项开始位置
	int what;						// 菜单项字符串
	float width, height;			// 菜单项尺寸宽度和高度
};

struct menu_item menu[TOTAL_MI];
float a=0;
float cur_x, cur_y;				// 光标坐标位置

int cur_model=0;

// 菜单初始化
void InitMenu()
{
	int i;
  
	// 初始化菜单项的位置
	for (i=0; i<TOTAL_MI; i++)
	{
		menu[i].posx = -0.65f;
		menu[i].posy = 0.15f-0.05f*(float)i;
		menu[i].width = 0.2f;
		menu[i].height = 0.05f;
	}

	// 初始化菜单项含义
	menu[0].what = LINE_T;
	menu[1].what = CUBE_T;
	menu[2].what = PYRAMID_T;
	menu[3].what = SPHERE_T;
	menu[4].what = SPINDLE_T;
	menu[5].what = TORUS_T;
	menu[6].what = CBOX_T;
	menu[7].what = LINE_T;
	menu[8].what = QUIT_T;
}

// 绘制外部.m模型
void DrawModel(struct model *p)
{
	int i, v1, v2, v3;
	int n1, n2, n3;
  
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	for (i=0; i<p->pn; i++)
	{
		v1 = p->p[i*6];
		v2 = p->p[i*6+1];
		v3 = p->p[i*6+2];
		n1 = p->p[i*6+3];
		n2 = p->p[i*6+4];
		n3 = p->p[i*6+5];
    
		glNormal3f(p->n[n1*3], p->n[n1*3+1], p->n[n1*3+2]);  
		glVertex3f(p->v[v1*3], p->v[v1*3+1], p->v[v1*3+2]);  

		glNormal3f(p->n[n2*3], p->n[n2*3+1], p->n[n2*3+2]);  
		glVertex3f(p->v[v2*3], p->v[v2*3+1], p->v[v2*3+2]);  

		glNormal3f(p->n[n3*3], p->n[n3*3+1], p->n[n3*3+2]);  
		glVertex3f(p->v[v3*3], p->v[v3*3+1], p->v[v3*3+2]);  
	}
	glEnd();
	glPopMatrix();
}

//  绘制菜单模块
void doRedraw(HDC *hDC)
{
	int i;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	// 绘制当前模型
	glPushMatrix();
	glTranslatef(1, 0, -20);
	glRotatef(a, 0, 1, 1);
	a += 1;
	glColor3f(1, 1, 1);
	DrawModel(&models[cur_model]);
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	// 绘制各菜单项
	for (i=0; i<TOTAL_MI; i++)
	{
		glBindTexture(GL_TEXTURE_2D, texture[menu[i].what]); // 选择每个菜单项的纹理
		if ((menu[i].flag)) 
			glColor4f(0.95f, 0.95f, 0.95f, 1);	// 鼠标在该菜单项上移动时，颜色变化
		else 
			glColor4f(1, 1, 1, 1.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); 
			glVertex3f(menu[i].posx, menu[i].posy, -2.01f);
			glTexCoord2f(1, 0); 
			glVertex3f(menu[i].posx+menu[i].width, menu[i].posy, -2.01f);
			glTexCoord2f(1, 1); 
			glVertex3f(menu[i].posx+menu[i].width, menu[i].posy+menu[i].height, -2.01f);
			glTexCoord2f(0, 1); 
			glVertex3f(menu[i].posx, menu[i].posy+menu[i].height, -2.01f);
		glEnd();
	}
 
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	// 绘制右边界
	glBindTexture(GL_TEXTURE_2D, texture[BORDER1_T]);
	glBegin(GL_QUADS);
	for (i=0; i<TOTAL_MI; i++)
	{
		glTexCoord2f(0.1f, 0); 
		glVertex3f(menu[i].posx+menu[i].width-0.022f, menu[i].posy, -2.01f);
		glTexCoord2f(1, 0); 
		glVertex3f(menu[i].posx+menu[i].width, menu[i].posy, -2.01f);
		glTexCoord2f(1, 1); 
		glVertex3f(menu[i].posx+menu[i].width, menu[i].posy+menu[i].height, -2.01f);
		glTexCoord2f(0.1f, 1); 
		glVertex3f(menu[i].posx+menu[i].width-0.022f, menu[i].posy+menu[i].height, -2.01f);
	}
	glEnd();
	// 绘制左边界
	glBindTexture(GL_TEXTURE_2D, texture[BORDER2_T]);
	glBegin(GL_QUADS);
	for (i=0; i<TOTAL_MI; i++)
	{
		glTexCoord2f(0, 0); 
		glVertex3f(menu[i].posx, menu[i].posy, -2.01f);
		glTexCoord2f(0.9f, 0); 
		glVertex3f(menu[i].posx+0.022f, menu[i].posy, -2.01f);
		glTexCoord2f(0.9f, 1); 
		glVertex3f(menu[i].posx+0.022f, menu[i].posy+menu[i].height, -2.01f);
		glTexCoord2f(0, 1); 
		glVertex3f(menu[i].posx, menu[i].posy+menu[i].height, -2.01f);
	}
	glEnd();
	// 绘制上边界和下边界
	glBindTexture(GL_TEXTURE_2D, texture[BORDER3_T]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0.1f); 
		glVertex3f(menu[0].posx, menu[0].posy+menu[0].height, -2.01f);
		glTexCoord2f(1, 0.1f); 
		glVertex3f(menu[0].posx+menu[0].width, menu[0].posy+menu[0].height, -2.01f);
		glTexCoord2f(1, 0.99f); 
		glVertex3f(menu[0].posx+menu[0].width, menu[0].posy+menu[0].height*2, -2.01f);
		glTexCoord2f(0, 0.99f); 
		glVertex3f(menu[0].posx, menu[0].posy+menu[0].height*2, -2.01f);

		glTexCoord2f(0, 0.1f); 
		glVertex3f(menu[TOTAL_MI-1].posx, menu[TOTAL_MI-1].posy, -2.01f);
		glTexCoord2f(1, 0.1f); 
		glVertex3f(menu[TOTAL_MI-1].posx+menu[TOTAL_MI-1].width, menu[TOTAL_MI-1].posy, -2.01f);
		glTexCoord2f(1, 0.99f); 
		glVertex3f(menu[TOTAL_MI-1].posx+menu[TOTAL_MI-1].width, menu[TOTAL_MI-1].posy-menu[TOTAL_MI-1].height, -2.01f);
		glTexCoord2f(0, 0.99f); 
		glVertex3f(menu[TOTAL_MI-1].posx, menu[TOTAL_MI-1].posy-menu[TOTAL_MI-1].height, -2.01f);
	glEnd();

	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	glEnable(GL_BLEND);
	glColor4f(1, 1, 1, 1);
	// 绘制光标透明遮罩
	glBindTexture(GL_TEXTURE_2D, texture[CURSOR_TRANSP_T]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(cur_x-0.03f, cur_y-0.03f, -2.001f);
		glTexCoord2f(1, 0); glVertex3f(cur_x+0.03f, cur_y-0.03f, -2.001f);
		glTexCoord2f(1, 1); glVertex3f(cur_x+0.03f, cur_y+0.03f, -2.001f);
		glTexCoord2f(0, 1); glVertex3f(cur_x-0.03f, cur_y+0.03f, -2.001f);
	glEnd();
	glBlendFunc(GL_ONE, GL_ONE);
	// 绘制光标
	glBindTexture(GL_TEXTURE_2D, texture[CURSOR_T]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(cur_x-0.03f, cur_y-0.03f, -2.001f);
		glTexCoord2f(1, 0); glVertex3f(cur_x+0.03f, cur_y-0.03f, -2.001f);
		glTexCoord2f(1, 1); glVertex3f(cur_x+0.03f, cur_y+0.03f, -2.001f);
		glTexCoord2f(0, 1); glVertex3f(cur_x-0.03f, cur_y+0.03f, -2.001f);
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glFlush();
	SwapBuffers(*hDC); 
}

//  判断用户是否按下菜单项
void check_menu(int state, int x, int y, int ww, int wh, HWND hWnd)
{
	int i, j;
	float xt, yt; // 鼠标位置坐标
	xt = ((float)(x-(ww/2))/(float)(ww/2))*0.66666f;
	yt = -((float)(y-(wh/2))/(float)(wh/2))*0.5f;

	cur_x = xt;		//  绘制光标的位置
	cur_y = yt;		//  绘制光标的位置

	for (i=0; i<TOTAL_MI; i++)
	{
		if ((xt>=menu[i].posx) && (xt<=(menu[i].posx+menu[i].width)) &&
			(yt>=menu[i].posy) && (yt<=(menu[i].posy+menu[i].height)))
		{
			if ((i != 0) && (i != 7)) 
				menu[i].flag = 1;	// 如果光标停留
			if (state) 
			{
				for (j=0; j<TOTAL_MI; j++) 
					menu[j].pressed = 0;	//  全部清零
				menu[i].pressed = 1;		//  按下鼠标左键
				if ((i > 0) && (i < 7)) 
					cur_model = i-1;
			}
			if ((state) && (i==8)) 
				DestroyWindow(hWnd); // 按下Quit菜单项
		}
		else menu[i].flag = 0;
	}
}