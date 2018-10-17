
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/glut.h>
#include "texture.h"

#define PI 3.14159

struct particle
{
  float t;       // ���ӵ�����
  float v;       // ���ӵ��˶��ٶ�
  float d;       // ���ӵ��˶�����
  float x, y, z; // ���ӵ�λ������
  float xd, zd;  // X �� Z �����ϵ�����
  char type;     // ���ӵ�״̬���ƶ��򵭻���
  float a;       // ���ӵ����� alpha ֵ
  struct particle *next, *prev;
};
struct point
{
  float x, y, z;
};

int WindW, WindH;
unsigned *teximage;    // ���ӵ�����
GLuint texture[2];     // ����ID
float a=0;
struct particle *fn[3];   /* Fountains */
char temp[128]="FPS: 0.000000";
// ���������ڼ���֡�ʵı���
int frame_counter=0;
time_t curtime;
clock_t last_time;
float cur_time;
float total_frames;
struct point upv, cam;        //  �������λ�ú����ϵķ���ʸ��

//  ��ʼ������
void Init()
{
  fn[0] = NULL;
  fn[1] = NULL;
  fn[2] = NULL;
  upv.x = -5;
  upv.y = 5;
  upv.z = -5;
  cam.x = 200;
  cam.y = 200;
  cam.z = 200;
  glGenTextures(3, texture); 
  glClearColor(0, 0, 0, 0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
}

// ����Ļ�ϻ����ַ���
void draw_string(void *font, const char* string) 
{
  while(*string)
    glutStrokeCharacter(font, *string++);
}

static float gettime(void)
{
  clock_t time_new, time_raz;

  time_new=clock();
  time_raz=time_new-last_time;
  last_time=time_new;

  return(time_raz/(float)CLOCKS_PER_SEC);
}

// װ������
void LoadTexture(char *fn, int t_num)
{
  int texwid, texht;
  int texcomps;
 
  teximage = read_texture(fn, &texwid, &texht, &texcomps);
  if (!teximage)
  {
    printf("Sorry, can't read texture file...");
    exit(0);
  }
  glBindTexture(GL_TEXTURE_2D, texture[t_num]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, texwid, texht, 0, GL_RGBA, GL_UNSIGNED_BYTE, teximage);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 

  if ((t_num == 0) || (t_num == 2)) glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); /* Special for particles */
  if (t_num == 1)
  {
    // ���õ�����ظ��������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
  }
  free(teximage);
} 

void Reshape(int width, int height)
{
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-0.5, 0.5, -0.5, 0.5, 1, 1000);
  glMatrixMode(GL_MODELVIEW);
  
  WindW = width;
  WindH = height;
}

//����µ�����
void AddParticles()
{
  struct particle *tempp;
  int i, j;
  
  for (j=0; j<3; j++)
    for (i=0; i<2; i++)
    {
      tempp = (struct particle *)malloc(sizeof(struct particle));
      if (fn[j]) fn[j]->prev = tempp;
      tempp->next = fn[j];
      fn[j] = tempp;
  
      tempp->t = -9.9;
      tempp->v = (float)(rand() % 200000)/100000+1; // �ٶ�
      tempp->d = (float)(rand() % 400)/100-2;     // ����Ƕ�
      // ��ʼ�˶�������
      tempp->x = 0;
      tempp->y = 0;
      tempp->z = 0;
      tempp->xd = cos((tempp->d*3.14159)/180)*tempp->v/4;
      tempp->zd = sin((tempp->d*3.14159)/180)*tempp->v;
      tempp->type = 0; // ���ӵ�״̬���˶���
      tempp->a = 1; // ���ӵ�Alpha��ʼֵ��1
    }
}

//  �ƶ�����
void MoveParticles()
{
  struct particle *tempp;
  int j;

  for (j=0; j<3; j++)
  {
    tempp = fn[j]; // ѡ����Ȫ
    while (tempp)
    {
      if (tempp->type == 0) // ����������˶���
      {
        tempp->x += tempp->xd;
        tempp->z += tempp->zd;
        tempp->y = -(9.8*(tempp->t*tempp->t/4))/2+122.5; // ����߶�
        tempp->t += 0.1; // ��������
        if (tempp->y < 0) tempp->type = 1; // ��������䵽���ϣ������ӵ���
      }
      else // �����䵽����
      {
        tempp->y = -(9.8*(tempp->t*tempp->t/4))/2+122.5; 
        tempp->a -= 0.1; // ���ӵ���
      }
      tempp = tempp->next; //������һ�����ӵļ���
    }
  }
}

//  ɾ������
void DeleteParticles()
{
  struct particle *tempp, *temp1;
  int j;

  for (j=0; j<3; j++)
  {
    tempp = fn[j];
    while (tempp)
    {
      if ((tempp->type == 1) && (tempp->a <= 0)) // ��������
      {
        // ɾ������
        temp1 = tempp->prev;
        tempp->prev->next = tempp->next;
        if (tempp->next) tempp->next->prev = temp1;
        free(tempp);
        tempp = temp1;
      }
      tempp = tempp->next;
    }
  }
}

//��ʸ����� C = A x B 
void vect_mult(struct point *A, struct point *B, struct point *C)
{
  /* Vector multiply */
  C->x = A->y*B->z - A->z*B->y;
  C->y = A->z*B->x - A->x*B->z;
  C->z = A->x*B->y - A->y*B->x;
}

//  ʸ����λ��
void normalize(struct point *V)
{
  float d;

  // ʸ������
  d = sqrt(V->x*V->x + V->y*V->y + V->z*V->z);

  // ��λ��
  V->x /= d; 
  V->y /= d; 
  V->z /= d; 
}

void DrawFountain()
{
  int j;
  struct particle *tempp;
  struct point vectd, vectl; 
  float alpha, ttx, ttz;

  glBindTexture(GL_TEXTURE_2D, texture[0]);

  AddParticles();
  MoveParticles();
  DeleteParticles();
  
  glPushMatrix();
  for (j=0; j<3; j++)
  {
    glBegin(GL_QUADS);
    tempp = fn[j];
    while (tempp)
    {
      // ��ת��Ȫ
      alpha = ((j*120+a)*PI)/180;
      ttx = tempp->x*cos(alpha)-tempp->z*sin(alpha);
      ttz = tempp->x*sin(alpha)+tempp->z*cos(alpha);
      // ���㷽��ʸ��A
      vectd.x = ttx - cam.x;
      vectd.y = tempp->y - cam.y;
      vectd.z = ttz - cam.z;
      vect_mult(&vectd, &upv, &vectl);
      normalize(&vectl);
      vectl.x *= 5;
      vectl.y *= 5;
      vectl.z *= 5;
      glColor4f(0.5, 0.5, 1, tempp->a);
      // �������Ӽ�����ӳ��
      glTexCoord2f(0, 0); glVertex3f((ttx-vectl.x), (tempp->y-upv.y), (ttz-vectl.z));
      glTexCoord2f(1, 0); glVertex3f((ttx+vectl.x), (tempp->y-upv.y), (ttz+vectl.z));
      glTexCoord2f(1, 1); glVertex3f((ttx+vectl.x), (tempp->y+upv.y), (ttz+vectl.z));
      glTexCoord2f(0, 1); glVertex3f((ttx-vectl.x), (tempp->y+upv.y), (ttz-vectl.z));
      tempp = tempp->next; // ��һ������
    }
    glEnd();
  }
  glPopMatrix();
}

//  �������ƺ���
void Draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  
  glLoadIdentity();
  glBindTexture(GL_TEXTURE_2D, texture[1]);
  a += 0.2;
  gluLookAt(cam.x, cam.y, cam.z, 0, 0, 0, upv.x, upv.y, upv.z); 
  
  // ���Ƶ���
  glColor3f(0.9, 0.9, 1);
  glPushMatrix();
  glRotatef(a, 0, -1, 0);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-100, 0, -100);
    glTexCoord2f(2, 0); glVertex3f(-100, 0, 100);
    glTexCoord2f(2, 2); glVertex3f(100, 0, 100);
    glTexCoord2f(0, 2); glVertex3f(100, 0, -100);
  glEnd();
  glPopMatrix();

  // ������Ȫ
  DrawFountain();

  glBindTexture(GL_TEXTURE_2D, texture[2]);
  glColor4f(1, 1, 0, 0.8);
  glPushMatrix();
  glRotatef(a, 0, -1, 0);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-100, 0, -100);
    glTexCoord2f(1, 0); glVertex3f(-100, 0, 100);
    glTexCoord2f(1, 1); glVertex3f(100, 0, 100);
    glTexCoord2f(0, 1); glVertex3f(100, 0, -100);
  glEnd();
  glPopMatrix();

  // ����֡��
  frame_counter++;
  if((frame_counter % 40) == 0)
  {
    frame_counter = 0;
    cur_time=gettime();
    sprintf(temp, "FPS: %f", 40.0/cur_time);
  }
  // ������ʾ֡�ʵ��ַ���
  glColor3f(1, 1, 1);
  glDisable(GL_TEXTURE_2D);
  glLoadIdentity();
  glTranslatef(-4.9, 4.5, -10);
  glScalef(0.003, 0.003, 0.004);
  draw_string(GLUT_STROKE_ROMAN, temp);
  glEnable(GL_TEXTURE_2D);
  
  glFlush();  
  glutSwapBuffers();
}

static void Key(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27: exit(0);
             break;
  }
}

void timf(int value)
{
  glutPostRedisplay();
  glutTimerFunc(16, timf, 0);
}

int main(int argc, char *argv[])
{

  WindW = 400;
  WindH = 300;
 
  glutInit(&argc, argv);
  glutInitWindowSize(WindW, WindH);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  (void)glutCreateWindow("��ά��Ȫ");
  Init();
  LoadTexture("particle.rgb", 0);
  LoadTexture("ground1.rgb", 1);
  LoadTexture("ground2.rgb", 2);
  glutReshapeFunc(Reshape);
  glutDisplayFunc(Draw);
  glutKeyboardFunc(Key);
  glutTimerFunc(16, timf, 0); // ���ö�ʱ��
  glutSetCursor(GLUT_CURSOR_NONE); // ���ع��
  glutMainLoop();

  return 0;
}
