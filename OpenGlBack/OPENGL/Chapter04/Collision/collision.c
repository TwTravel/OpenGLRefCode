
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>

// 光源定义
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};  
GLfloat light_position[] = {10.0, 10.0, 30.0, 1.0};  
int win_id;
static GLuint texName;

// SETUP DATASTRUCTURE FOR OBJECTS
#define MAXGUYS 300
#define MAXOBJS 80
int NUMOBJS;
int TEXENABLE;
int DEBUGENABLE;
int frame;
//  点的数据结构
struct point 
{
	float x; 
	float y; 
	float z;
};
//  有关弹性碰撞的数据结构
struct spring 
{
	struct point* p1; 
	struct point* p2;
	struct point* v1; 
	struct point* v2;
	float* m1; 
	float* m2;
	float length; 
	float stiff; 
	float damp; 
	float breaklen;
};
//  有关碰撞线的数据结构
struct cdline 
{
	struct point* p1; 
	struct point* p2; 
	struct point* v1; 
	struct point* v2;
	struct point* vi1; 
	struct point* vi2;
	struct point min; 
	struct point max;
	float* mass;
	float fric;
};
//  对象的面的数据结构
struct face 
{
	struct point* p1; 
	struct point* p2;
	struct point* p3;
	struct point* v1; 
	struct point* v2; 
	struct point* v3;
	struct point* vi1; 
	struct point* vi2; 
	struct point* vi3; 
    float* m1; 
	float* m2; 
	float* m3;
    struct point min; 
	struct point max;
    float fric; 
	struct point nor;
};
//  有关对象的数据结构
struct object 
{
	int NUMPTS;
	int NUMSPRINGS;
	int NUMFACES;
	int NUMCDLINES;
	float mass[MAXGUYS];
	float r,g,b,a;
	struct point points[MAXGUYS];
	struct point vel[MAXGUYS];
	struct point velinc[MAXGUYS];
	struct spring springs[MAXGUYS];
	struct cdline cdlines[MAXGUYS];
	struct face faces[MAXGUYS];
	struct point max;                             // 对象的边界盒
	struct point min;
	int texture;
	int NUMTEXPTS;
	int texmap[64][64];
	struct point texpoints[MAXGUYS]; 
	struct face texfaces[MAXGUYS];
};
struct object objects[MAXOBJS];

// 定义一些底层函数
#define sqa(A) (A*fabs(A))
#define length(A) sqrt(A.x*A.x+A.y*A.y+A.z*A.z)
#define dot(A,B) (A.x*B.x+A.y*B.y+A.z*B.z)
#define cross(D,A,B) D.x=A.y*B.z-A.z*B.y; D.y=A.z*B.x-A.x*B.z; D.z=A.x*B.y-A.y*B.x
#define vsub(D,A,B) D.x=A.x-B.x; D.y=A.y-B.y; D.z=A.z-B.z
#define fmaxinc(X,Y) X.x=(fabs(X.x)>fabs(Y.x)) ? X.x:Y.x; X.y=(fabs(X.y)>fabs(Y.y)) ? X.y:Y.y; X.z=(fabs(X.z)>fabs(Y.z)) ? X.z:Y.z

// 物理定义，步长和重力加速度
int STEP;
float GRAVITY;

// Spring variables
struct point sdistvec, relvel, sforcevec;
float springforce;

void drawcross(float x, float y, float z)
{
  glBegin(GL_LINES);
    glColor4f(.3,.3,.3,1.0);
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(x+1.0,y    ,z    );
    glVertex3f(x-1.0,y    ,z    );
    glVertex3f(x    ,y+1.0,z    );
    glVertex3f(x    ,y-1.0,z    );
    glVertex3f(x    ,y    ,z+1.0);
    glVertex3f(x    ,y    ,z-1.0);
  glEnd();
}

void display(void)
{
	int x,y,i,j,step;
	float len,u,t1,t2,t3,distdiff,relmass,SFORCE,FFORCE,frico;
	struct point tmp1,tmp2, cpt, n1, n2, n3, avgfacevel, vdiff, vdiff_norm, vdiff_notnorm;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 弹性碰撞计算
	for(step=0;step<STEP;step++)
	{
		for(i=0;i<NUMOBJS;i++) // 遍历每个对象
		{
			// 进行弹性碰撞
			for(x=0;x<objects[i].NUMSPRINGS;x++)
			{
				// 相对位置
				vsub(sdistvec,(*objects[i].springs[x].p1),(*objects[i].springs[x].p2));
				// 相对速度
				vsub(relvel,(*objects[i].springs[x].v1),(*objects[i].springs[x].v2));
				distdiff =    length(sdistvec)-objects[i].springs[x].length;
				springforce = sqa(distdiff)*objects[i].springs[x].stiff;
				springforce+= objects[i].springs[x].damp*dot(relvel,sdistvec); 

				// 如果距离超过一定范围，则解除弹性力
				if(fabs(distdiff)>objects[i].springs[x].breaklen) 
				{
					objects[i].springs[x].stiff=0.0; 
					objects[i].springs[x].damp=0.0;
				}

				sforcevec.x = sdistvec.x*springforce;
				sforcevec.y = sdistvec.y*springforce;
				sforcevec.z = sdistvec.z*springforce;

				relmass = (*objects[i].springs[x].m1)/
					( (*objects[i].springs[x].m1) + (*objects[i].springs[x].m2) );

				// 应用弹性力 
				(*objects[i].springs[x].v1).x -= (1.0-relmass)*sforcevec.x;
				(*objects[i].springs[x].v1).y -= (1.0-relmass)*sforcevec.y;
				(*objects[i].springs[x].v1).z -= (1.0-relmass)*sforcevec.z;

				(*objects[i].springs[x].v2).x += relmass*sforcevec.x;
				(*objects[i].springs[x].v2).y += relmass*sforcevec.y;
				(*objects[i].springs[x].v2).z += relmass*sforcevec.z;
			}

			// 更新对象的各点
			for(x=0;x<objects[i].NUMPTS;x++)
			{
				// 应用重力加速度
				objects[i].vel[x].y-=GRAVITY;

				if(objects[i].points[x].y<-15.0) 
				{
					objects[i].points[x].y=-15.0; 
					objects[i].vel[x].y=0.0;
					objects[i].vel[x].x/=1.1; 
					objects[i].vel[x].z/=1.1;
				}
			}
		}

		// 碰撞初始化
		for(i=0;i<NUMOBJS;i++)
			for(x=0;x<objects[i].NUMFACES;x++)
			{
				// 计算边界盒
				objects[i].faces[x].max.x = (*objects[i].faces[x].p1).x;
				objects[i].faces[x].max.x = (objects[i].faces[x].max.x>(*objects[i].faces[x].p2).x) ?
					                         objects[i].faces[x].max.x:(*objects[i].faces[x].p2).x;
				objects[i].faces[x].max.x = (objects[i].faces[x].max.x>(*objects[i].faces[x].p3).x) ?
                                             objects[i].faces[x].max.x:(*objects[i].faces[x].p3).x;
				objects[i].faces[x].max.y = (*objects[i].faces[x].p1).y;
				objects[i].faces[x].max.y = (objects[i].faces[x].max.y>(*objects[i].faces[x].p2).y) ?
                                             objects[i].faces[x].max.y:(*objects[i].faces[x].p2).y;
				objects[i].faces[x].max.y = (objects[i].faces[x].max.y>(*objects[i].faces[x].p3).y) ?
                                             objects[i].faces[x].max.y:(*objects[i].faces[x].p3).y;
				objects[i].faces[x].max.z = (*objects[i].faces[x].p1).z;
				objects[i].faces[x].max.z = (objects[i].faces[x].max.z>(*objects[i].faces[x].p2).z) ?
                                             objects[i].faces[x].max.z:(*objects[i].faces[x].p2).z;
				objects[i].faces[x].max.z = (objects[i].faces[x].max.z>(*objects[i].faces[x].p3).z) ?
                                             objects[i].faces[x].max.z:(*objects[i].faces[x].p3).z;
				objects[i].faces[x].min.x = (*objects[i].faces[x].p1).x;
				objects[i].faces[x].min.x = (objects[i].faces[x].min.x<(*objects[i].faces[x].p2).x) ?
                                             objects[i].faces[x].min.x:(*objects[i].faces[x].p2).x;
				objects[i].faces[x].min.x = (objects[i].faces[x].min.x<(*objects[i].faces[x].p3).x) ?
                                             objects[i].faces[x].min.x:(*objects[i].faces[x].p3).x;
				objects[i].faces[x].min.y = (*objects[i].faces[x].p1).y;
				objects[i].faces[x].min.y = (objects[i].faces[x].min.y<(*objects[i].faces[x].p2).y) ?
                                             objects[i].faces[x].min.y:(*objects[i].faces[x].p2).y;
				objects[i].faces[x].min.y = (objects[i].faces[x].min.y<(*objects[i].faces[x].p3).y) ?
                                             objects[i].faces[x].min.y:(*objects[i].faces[x].p3).y;
				objects[i].faces[x].min.z = (*objects[i].faces[x].p1).z;
				objects[i].faces[x].min.z = (objects[i].faces[x].min.z<(*objects[i].faces[x].p2).z) ?
                                             objects[i].faces[x].min.z:(*objects[i].faces[x].p2).z;
				objects[i].faces[x].min.z = (objects[i].faces[x].min.z<(*objects[i].faces[x].p3).z) ?
                                             objects[i].faces[x].min.z:(*objects[i].faces[x].p3).z;

				// 计算法向量
				vsub(tmp1,(*objects[i].faces[x].p1),(*objects[i].faces[x].p2));
				vsub(tmp2,(*objects[i].faces[x].p2),(*objects[i].faces[x].p3));
				cross(objects[i].faces[x].nor,tmp1,tmp2);
				len=length(objects[i].faces[x].nor);
				objects[i].faces[x].nor.x/=len;
				objects[i].faces[x].nor.y/=len;
				objects[i].faces[x].nor.z/=len;
			}
			
		for(i=0;i<NUMOBJS;i++)
			for(x=0;x<objects[i].NUMCDLINES;x++) 
			{
				// 获得碰撞检测边界线
				objects[i].cdlines[x].max.x = (*objects[i].cdlines[x].p1).x;
				objects[i].cdlines[x].max.x = (objects[i].cdlines[x].max.x>(*objects[i].cdlines[x].p2).x) ?
                                                  objects[i].cdlines[x].max.x:(*objects[i].cdlines[x].p2).x;
				objects[i].cdlines[x].max.y = (*objects[i].cdlines[x].p1).y;
				objects[i].cdlines[x].max.y = (objects[i].cdlines[x].max.y>(*objects[i].cdlines[x].p2).y) ?
                                                  objects[i].cdlines[x].max.y:(*objects[i].cdlines[x].p2).y;
				objects[i].cdlines[x].max.z = (*objects[i].cdlines[x].p1).z;
				objects[i].cdlines[x].max.z = (objects[i].cdlines[x].max.z>(*objects[i].cdlines[x].p2).z) ?
                                                  objects[i].cdlines[x].max.z:(*objects[i].cdlines[x].p2).z;

				objects[i].cdlines[x].min.x = (*objects[i].cdlines[x].p1).x;
				objects[i].cdlines[x].min.x = (objects[i].cdlines[x].min.x<(*objects[i].cdlines[x].p2).x) ?
                                                  objects[i].cdlines[x].min.x:(*objects[i].cdlines[x].p2).x;
				objects[i].cdlines[x].min.y = (*objects[i].cdlines[x].p1).y;
				objects[i].cdlines[x].min.y = (objects[i].cdlines[x].min.y<(*objects[i].cdlines[x].p2).y) ?
                                                  objects[i].cdlines[x].min.y:(*objects[i].cdlines[x].p2).y;
				objects[i].cdlines[x].min.z = (*objects[i].cdlines[x].p1).z;
				objects[i].cdlines[x].min.z = (objects[i].cdlines[x].min.z<(*objects[i].cdlines[x].p2).z) ?
                                                  objects[i].cdlines[x].min.z:(*objects[i].cdlines[x].p2).z;
			}
		for(i=0;i<NUMOBJS;i++)
			for(x=0;x<objects[i].NUMPTS;x++)
			{
				// 计算对象边界盒
				objects[i].max.x=-HUGE; 
				objects[i].max.y=-HUGE; 
				objects[i].max.z=-HUGE;
				objects[i].min.x= HUGE; 
				objects[i].min.y= HUGE; 
				objects[i].min.z= HUGE;
				for(x=0;x<objects[i].NUMPTS;x++)
				{
					objects[i].max.x=(objects[i].max.x<objects[i].points[x].x) ? objects[i].points[x].x:objects[i].max.x;
					objects[i].max.y=(objects[i].max.y<objects[i].points[x].y) ? objects[i].points[x].y:objects[i].max.y;
					objects[i].max.z=(objects[i].max.z<objects[i].points[x].z) ? objects[i].points[x].z:objects[i].max.z;

					objects[i].min.x=(objects[i].min.x>objects[i].points[x].x) ? objects[i].points[x].x:objects[i].min.x;
					objects[i].min.y=(objects[i].min.y>objects[i].points[x].y) ? objects[i].points[x].y:objects[i].min.y;
					objects[i].min.z=(objects[i].min.z>objects[i].points[x].z) ? objects[i].points[x].z:objects[i].min.z;
				}
			}

		// 碰撞检测开始
		for(i=0;i<NUMOBJS;i++)   // 对任意两个物体都进行计算
			for(j=0;j<NUMOBJS;j++) 
			{
				//  同一物体，继续下一个
				if(i==j) continue;
							
				// 没有碰撞，继续下一个
				if( (objects[i].max.x-objects[j].min.x)*(objects[i].min.x-objects[j].max.x)>0.0 ||
					(objects[i].max.y-objects[j].min.y)*(objects[i].min.y-objects[j].max.y)>0.0 ||
					(objects[i].max.z-objects[j].min.z)*(objects[i].min.z-objects[j].max.z)>0.0 ) 
					continue;
				// 碰撞处理
				for(x=0;x<objects[i].NUMFACES;x++)
					for(y=0;y<objects[j].NUMCDLINES;y++)
					{
						// 面/碰撞线边界盒检测
						if( (objects[i].faces[x].max.x-objects[j].cdlines[y].min.x)*
							(objects[i].faces[x].min.x-objects[j].cdlines[y].max.x)>0.0 || 
							(objects[i].faces[x].max.y-objects[j].cdlines[y].min.y)*
							(objects[i].faces[x].min.y-objects[j].cdlines[y].max.y)>0.0 || 
								(objects[i].faces[x].max.z-objects[j].cdlines[y].min.z)*
								(objects[i].faces[x].min.z-objects[j].cdlines[y].max.z)>0.0 ) 
							continue;
						vsub(tmp1,(*objects[j].cdlines[y].p2),(*objects[j].cdlines[y].p1));
						if(dot(tmp1,objects[i].faces[x].nor)>0.0) 
							continue;

						// 碰撞线穿过面
						vsub(tmp1,(*objects[i].faces[x].p1),  (*objects[j].cdlines[y].p1) );
						vsub(tmp2,(*objects[j].cdlines[y].p2),(*objects[j].cdlines[y].p1) );
						u   = dot(objects[i].faces[x].nor,tmp1)/
							dot(objects[i].faces[x].nor,tmp2);
									
						if(u<0.0 || u>1.0) 
							continue;  

						cpt.x = (*objects[j].cdlines[y].p1).x + u*tmp2.x;
						cpt.y = (*objects[j].cdlines[y].p1).y + u*tmp2.y;
						cpt.z = (*objects[j].cdlines[y].p1).z + u*tmp2.z;

						// 找出边界法向量
						vsub(tmp1,(*objects[i].faces[x].p2),(*objects[i].faces[x].p1) );
						cross(n1,tmp1,objects[i].faces[x].nor);
						vsub(tmp1,(*objects[i].faces[x].p3),(*objects[i].faces[x].p2) );
						cross(n2,tmp1,objects[i].faces[x].nor);
						vsub(tmp1,(*objects[i].faces[x].p1),(*objects[i].faces[x].p3) );
						cross(n3,tmp1,objects[i].faces[x].nor);
									
						// 检测三角形边界
						vsub(tmp1,(*objects[i].faces[x].p1),cpt);
						t1=dot(tmp1,n1);
						vsub(tmp1,(*objects[i].faces[x].p2),cpt);
						t2=dot(tmp1,n2);
						vsub(tmp1,(*objects[i].faces[x].p3),cpt);
						t3=dot(tmp1,n3);
						if(t1<0.0||t2<0.0||t3<0.0) 
							u=-999.0;

						if(u<0.0) continue; 
						if(DEBUGENABLE) drawcross(cpt.x,cpt.y,cpt.z);  // 绘制碰撞点

						// 计算面的平均速度
						avgfacevel.x = ((*objects[i].faces[x].v1).x+
							(*objects[i].faces[x].v2).x+(*objects[i].faces[x].v3).x)*.3333333333333333333333333333;
						avgfacevel.y = ((*objects[i].faces[x].v1).y+
							(*objects[i].faces[x].v2).y+(*objects[i].faces[x].v3).y)*.3333333333333333333333333333;
						avgfacevel.z = ((*objects[i].faces[x].v1).z+
							(*objects[i].faces[x].v2).z+(*objects[i].faces[x].v3).z)*.3333333333333333333333333333;
						vsub(vdiff,avgfacevel,(*objects[j].cdlines[y].v2));

						FFORCE=(*objects[j].cdlines[y].mass)/((*objects[j].cdlines[y].mass)+
							(*objects[i].faces[x].m1)+(*objects[i].faces[x].m2)+(*objects[i].faces[x].m3));

						SFORCE=1.0-FFORCE;

						vdiff_norm.x = dot(vdiff, objects[i].faces[x].nor) * objects[i].faces[x].nor.x;
						vdiff_norm.y = dot(vdiff, objects[i].faces[x].nor) * objects[i].faces[x].nor.y;
						vdiff_norm.z = dot(vdiff, objects[i].faces[x].nor) * objects[i].faces[x].nor.z;
						vsub(vdiff_notnorm,vdiff,vdiff_norm);
						// 产生摩擦系数
						frico = objects[i].faces[x].fric+objects[j].cdlines[y].fric;  

						// 解决碰撞
						if(dot(vdiff,objects[i].faces[x].nor)>0.0) 
						{
							fmaxinc((*objects[j].cdlines[y].vi2),SFORCE*vdiff_norm);
							fmaxinc((*objects[i].faces[x].vi1),  -FFORCE*vdiff_norm);
							fmaxinc((*objects[i].faces[x].vi2),  -FFORCE*vdiff_norm);
							fmaxinc((*objects[i].faces[x].vi3),  -FFORCE*vdiff_norm);
							fmaxinc((*objects[j].cdlines[y].vi2),SFORCE*frico*vdiff_notnorm);
							fmaxinc((*objects[i].faces[x].vi1),  -FFORCE*frico*vdiff_notnorm);
							fmaxinc((*objects[i].faces[x].vi2),  -FFORCE*frico*vdiff_notnorm);
							fmaxinc((*objects[i].faces[x].vi3),  -FFORCE*frico*vdiff_notnorm);
						}
					}
			}
						
		for(i=0;i<NUMOBJS;i++)
			for(x=0;x<objects[i].NUMPTS;x++)
			{
								
				// 获取速度
				objects[i].vel[x].x+=objects[i].velinc[x].x;
				objects[i].vel[x].y+=objects[i].velinc[x].y;
				objects[i].vel[x].z+=objects[i].velinc[x].z;
				objects[i].velinc[x].x=0.0;
				objects[i].velinc[x].y=0.0;
				objects[i].velinc[x].z=0.0;
				// 更新点的位置
				objects[i].points[x].x+=objects[i].vel[x].x;
				objects[i].points[x].y+=objects[i].vel[x].y;
				objects[i].points[x].z+=objects[i].vel[x].z;
			}
	}  

	// 场景显示
	for(i=0;i<NUMOBJS;i++)
	{
		if(DEBUGENABLE)
		{
			glColor4f(0.4,1.0,0.4,1.0);
			//  绘制个定点与中心连线
			for(x=0;x<objects[i].NUMCDLINES;x++)
			{
				glBegin(GL_LINES);
					glNormal3f(0.0,1.0,0.0);
					glVertex3f((*objects[i].cdlines[x].p1).x,(*objects[i].cdlines[x].p1).y,
						(*objects[i].cdlines[x].p1).z);
					glVertex3f((*objects[i].cdlines[x].p2).x,(*objects[i].cdlines[x].p2).y,
						(*objects[i].cdlines[x].p2).z);
				glEnd();
			}
			//  绘制对象的各个面
			for(x=0;x<objects[i].NUMFACES;x++)
			{
				glBegin(GL_LINES);
					glNormal3f(0.0,1.0,0.0);
					glColor4f(1.0,0.4,0.4,1.0);
					glVertex3f((*objects[i].faces[x].p1).x,(*objects[i].faces[x].p1).y,
						(*objects[i].faces[x].p1).z);
					glVertex3f((*objects[i].faces[x].p2).x,(*objects[i].faces[x].p2).y,
					     (*objects[i].faces[x].p2).z);
					glVertex3f((*objects[i].faces[x].p2).x,(*objects[i].faces[x].p2).y,
						 (*objects[i].faces[x].p2).z);
					glVertex3f((*objects[i].faces[x].p3).x,(*objects[i].faces[x].p3).y,
						(*objects[i].faces[x].p3).z);
					glVertex3f((*objects[i].faces[x].p3).x,(*objects[i].faces[x].p3).y,
						(*objects[i].faces[x].p3).z);
					glVertex3f((*objects[i].faces[x].p1).x,(*objects[i].faces[x].p1).y,
						(*objects[i].faces[x].p1).z);
					glNormal3f(0.0,1.0,0.0);
					glColor4f(0.4,0.4,1.0,1.0);
					glVertex3f((*objects[i].faces[x].p1).x,(*objects[i].faces[x].p1).y,
						(*objects[i].faces[x].p1).z);
					glVertex3f((*objects[i].faces[x].p1).x+(objects[i].faces[x].nor).x,
						(*objects[i].faces[x].p1).y+(objects[i].faces[x].nor).y,
						(*objects[i].faces[x].p1).z+(objects[i].faces[x].nor).z);

					glVertex3f((*objects[i].faces[x].p2).x,(*objects[i].faces[x].p2).y,
						(*objects[i].faces[x].p2).z);
					glVertex3f((*objects[i].faces[x].p2).x+(objects[i].faces[x].nor).x,
						(*objects[i].faces[x].p2).y+(objects[i].faces[x].nor).y,
						(*objects[i].faces[x].p2).z+(objects[i].faces[x].nor).z);

					glVertex3f((*objects[i].faces[x].p3).x,(*objects[i].faces[x].p3).y,
						(*objects[i].faces[x].p3).z);
					glVertex3f((*objects[i].faces[x].p3).x+(objects[i].faces[x].nor).x,
						(*objects[i].faces[x].p3).y+(objects[i].faces[x].nor).y,
						(*objects[i].faces[x].p3).z+(objects[i].faces[x].nor).z);

				glEnd();
			}
		} 
		else // 标准显示
		{
			glColor4f(objects[i].r,objects[i].g,objects[i].b,objects[i].a);
			// 使用纹理
			if(objects[i].texture && TEXENABLE)   
			{
				
				glEnable(GL_TEXTURE_2D);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, objects[i].texmap);
				glBindTexture(GL_TEXTURE_2D, texName);
				for(x=0;x<objects[i].NUMFACES;x++)
				{
					
					glBegin(GL_TRIANGLES); 
						glNormal3f(objects[i].faces[x].nor.x,objects[i].faces[x].nor.y,
							objects[i].faces[x].nor.z);
						glTexCoord2f((*objects[i].texfaces[x].p1).x,(*objects[i].texfaces[x].p1).y);
						glVertex3f((*objects[i].faces[x].p1).x,(*objects[i].faces[x].p1).y,
							(*objects[i].faces[x].p1).z);
						glTexCoord2f((*objects[i].texfaces[x].p2).x,(*objects[i].texfaces[x].p2).y);
						glVertex3f((*objects[i].faces[x].p2).x,(*objects[i].faces[x].p2).y,
							(*objects[i].faces[x].p2).z);
						glTexCoord2f((*objects[i].texfaces[x].p3).x,(*objects[i].texfaces[x].p3).y);
						glVertex3f((*objects[i].faces[x].p3).x,(*objects[i].faces[x].p3).y,
							(*objects[i].faces[x].p3).z);
					glEnd();
				}
				glDisable(GL_TEXTURE_2D);
			}
			// 不使用纹理
			else
			{
				for(x=0;x<objects[i].NUMFACES;x++)
				{
					glBegin(GL_TRIANGLES);
						glNormal3f(objects[i].faces[x].nor.x,objects[i].faces[x].nor.y,
							objects[i].faces[x].nor.z);
						glVertex3f((*objects[i].faces[x].p1).x,(*objects[i].faces[x].p1).y,
							(*objects[i].faces[x].p1).z);
						glVertex3f((*objects[i].faces[x].p2).x,(*objects[i].faces[x].p2).y,
							(*objects[i].faces[x].p2).z);
						glVertex3f((*objects[i].faces[x].p3).x,(*objects[i].faces[x].p3).y,
							(*objects[i].faces[x].p3).z);
					glEnd();
				}
			}
		
		}
	} 

	// 绘制地板
	glColor4f(.2,.4,.2,1.0);
	glNormal3f(0.0,1.0,0.0);
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0,1.0); glVertex3f(-20.0,-15.4,20.0);
	glTexCoord2f(1.0,1.0); glVertex3f(20.0,-15.4,20.0);
	glTexCoord2f(0.0,0.0); glVertex3f(-20.0,-15.4,-20.0);

	glTexCoord2f(1.0,0.0); glVertex3f(20.0,-15.4,-20.0);
	glTexCoord2f(0.0,0.0); glVertex3f(-20.0,-15.4,-20.0);
	glTexCoord2f(1.0,1.0); glVertex3f(20.0,-15.4,20.0);
	glEnd();
	glutSwapBuffers();

}



void init()
{
	char rval,gval,bval;
	int x,i;
	int windbackwards;
	FILE *file;
	int p1,p2,p3,o1,o2;
	char filename[MAXOBJS][100];
	char modfile[MAXOBJS][100];
	char texfile[MAXOBJS][100];
	struct point offset[MAXOBJS];
	float scale;
	struct point tmp;
	float damp, fric, breaklen;

	frame=0;
	TEXENABLE=1;
	DEBUGENABLE=0;

	if((file=fopen("config.txt","r"))==NULL) 
	{
		printf("Can't open config.txt\n"); 
		exit(-1);
	}
	fscanf(file,"%d %f",&STEP,&GRAVITY);	//  读出步数，重力加速度
	fscanf(file,"%d",&NUMOBJS);				//  读出对象的数量
	for(x=0;x<NUMOBJS;x++) 
		//  读出对象的名称和对象的初始位置
		fscanf(file,"%s %f %f %f",&filename[x][0],&offset[x].x, &offset[x].y,&offset[x].z);
	fclose(file);

	for(i=0;i<NUMOBJS;i++)
	{
		sprintf(modfile[i],"models/%s.mod",filename[i]);
		sprintf(texfile[i],"models/%s.tga",filename[i]);
		printf("Attempting to open model file %s\n",modfile[i]);
		if((file=fopen(modfile[i],"r"))==NULL) {printf("Can't open model\n"); exit(-1);}
		//  读出对象的缩放系数
		fscanf(file,"%f",&scale);	
		//  读出对象的RGBA颜色值 
		fscanf(file,"%f %f %f %f",&objects[i].r,&objects[i].g,&objects[i].b,&objects[i].a);
		fscanf(file,"%d",&objects[i].NUMPTS);
		fscanf(file,"%d",&objects[i].NUMSPRINGS);
		fscanf(file,"%d",&objects[i].NUMFACES);
		fscanf(file,"%d",&objects[i].NUMCDLINES);
		fscanf(file,"%f",&damp);
		fscanf(file,"%f",&fric);
		fscanf(file,"%f",&breaklen);
		fscanf(file,"%d %d",&objects[i].texture,&windbackwards);

		for(x=0;x<objects[i].NUMPTS;x++) 
		{
			fscanf(file,"%f",&objects[i].mass[x]);
		}
		for(x=0;x<objects[i].NUMPTS;x++)
		{
			fscanf(file,"%f %f %f",&objects[i].points[x].x,&objects[i].points[x].y,&objects[i].points[x].z);
			objects[i].points[x].x*=scale;
			objects[i].points[x].y*=scale;
			objects[i].points[x].z*=scale;
		}
		for(x=0;x<objects[i].NUMPTS;x++)
		{
			objects[i].points[x].x+=offset[i].x;
			objects[i].points[x].y+=offset[i].y;
			objects[i].points[x].z+=offset[i].z;
		}
		for(x=0;x<objects[i].NUMPTS;x++)
			fscanf(file,"%f %f %f",&objects[i].vel[x].x,&objects[i].vel[x].y,&objects[i].vel[x].z);
		for(x=0;x<objects[i].NUMSPRINGS;x++)
		{
			fscanf(file,"%d %d %d %d %f",&o1,&p1,&o2,&p2,&objects[i].springs[x].stiff);
			if(o1<0) o1=i; if(o2<0) o2=i;
			objects[i].springs[x].damp=damp;
			objects[i].springs[x].p1=&objects[o1].points[p1];
			objects[i].springs[x].p2=&objects[o2].points[p2];
			vsub(tmp,(*objects[i].springs[x].p1),(*objects[i].springs[x].p2));
			objects[i].springs[x].length=length(tmp);
			objects[i].springs[x].v1=&objects[o1].vel[p1];
			objects[i].springs[x].v2=&objects[o2].vel[p2];
			objects[i].springs[x].m1=&objects[o1].mass[p1];
			objects[i].springs[x].m2=&objects[o2].mass[p2];
			objects[i].springs[x].breaklen=breaklen;
		}
		for(x=0;x<objects[i].NUMFACES;x++)
		{
			if(windbackwards==0)
				fscanf(file,"%d %d %d",&p1,&p2,&p3);
			else
				fscanf(file,"%d %d %d",&p2,&p1,&p3);
			objects[i].faces[x].fric=fric;
			objects[i].faces[x].p1 =&objects[i].points[p1];
			objects[i].faces[x].p2 =&objects[i].points[p2];
			objects[i].faces[x].p3 =&objects[i].points[p3];
			objects[i].faces[x].v1 =&objects[i].vel[p1];
			objects[i].faces[x].v2 =&objects[i].vel[p2];
			objects[i].faces[x].v3 =&objects[i].vel[p3];
			objects[i].faces[x].vi1=&objects[i].velinc[p1];
			objects[i].faces[x].vi2=&objects[i].velinc[p2];
			objects[i].faces[x].vi3=&objects[i].velinc[p3];
			objects[i].faces[x].m1=&objects[i].mass[p1];
			objects[i].faces[x].m2=&objects[i].mass[p2];
			objects[i].faces[x].m3=&objects[i].mass[p3];
		}
		for(x=0;x<objects[i].NUMCDLINES;x++)
		{
			fscanf(file,"%d %d",&p1,&p2);
			objects[i].cdlines[x].p1=&objects[i].points[p1];
			objects[i].cdlines[x].p2=&objects[i].points[p2];
			objects[i].cdlines[x].v1=&objects[i].vel[p1];
			objects[i].cdlines[x].v2=&objects[i].vel[p2];
			objects[i].cdlines[x].vi1=&objects[i].velinc[p1];
			objects[i].cdlines[x].vi2=&objects[i].velinc[p2];
			objects[i].cdlines[x].fric=fric;
			objects[i].cdlines[x].mass=&objects[i].mass[p2];
		}
		if(objects[i].texture==1)
		{
			fscanf(file,"%d",&objects[i].NUMTEXPTS);
			for(x=0;x<objects[i].NUMTEXPTS;x++)
				fscanf(file,"%f %f",&objects[i].texpoints[x].x,&objects[i].texpoints[x].y);
			for(x=0;x<objects[i].NUMTEXPTS;x++)
			{
				objects[i].texpoints[x].x/=256.0; 
				objects[i].texpoints[x].y/=256.0; 
			}
			for(x=0;x<objects[i].NUMFACES;x++)
			{
				if(windbackwards==0)
					fscanf(file,"%d %d %d",&p1,&p2,&p3);
				else
					fscanf(file,"%d %d %d",&p2,&p1,&p3);
				objects[i].texfaces[x].p1=&objects[i].texpoints[p1];
				objects[i].texfaces[x].p2=&objects[i].texpoints[p2];
				objects[i].texfaces[x].p3=&objects[i].texpoints[p3];
			}
		}
		fclose(file);
		
		if(objects[i].texture==1)
		{
			if((file=fopen(texfile[i],"rb"))==NULL) 
			{
				printf("Can't open texture \n"); 
				exit(-1);
			}
			for(x=0;x<60;x++) fscanf(file,"%c",&rval); 
			for(x=0;x<64*64;x++)
			{
				fscanf(file,"%c",&bval);
				fscanf(file,"%c",&gval);
				fscanf(file,"%c",&rval);
				objects[i].texmap[0][x]=0xFF000000|(0xFF&bval)<<16|(0xFF&gval)<<8|(0xFF&rval); // ABGR
			}
		}
	} 
}

void glinit()
{
	glGenTextures(1,&texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	//  设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// 使用光照
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_COLOR_MATERIAL); 

	//  使用深度测试
	glEnable(GL_DEPTH_TEST);
	
	glMatrixMode(GL_PROJECTION);
	gluPerspective( 60.0, 1.0, 1.0, 10000.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(10.0, 10.0, 30.0,   0.0, -10.0, 0.0,  0.0, 1.0, 0.); 
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) 
	{
		case 27:  // 按下'ESC'键，退出程序
			{
				glutDestroyWindow(win_id);
				exit(0); 
			}
			break;
	    case 'r':   //  场景复位
			init();
			break;
        case 't':	//  是否使用纹理开关
			if(TEXENABLE) 
				TEXENABLE=0;
			else 
				TEXENABLE=1;
			break;
        case 'd':  //  是否以线框模式绘制开关
            if(DEBUGENABLE) 
				DEBUGENABLE=0;
            else 
				DEBUGENABLE=1;
            break;
	    case '5':
			{
				for(x=0;x<objects[0].NUMPTS;x++) 
					objects[0].vel[x].x+=.004;
			}
		    break;
	    case '8':
			{
				for(x=0;x<objects[0].NUMPTS;x++) 
					objects[0].vel[x].x-=.004;
			}
		    break;
	    case '4':
			{
				for(x=0;x<objects[0].NUMPTS;x++) 
					objects[0].vel[x].z+=.004;
			}
		    break;
	    case '6':
			{
				for(x=0;x<objects[0].NUMPTS;x++)
					objects[0].vel[x].z-=.004;
			}
		    break;
	    case '1': // 按下'1'键，向左旋转场景
			gluLookAt(0.0, 0.0, 0.01,  
		    0.001, 0.0, 0.0,      
			0.0, 1.0, 0.0);      
            break;
	    case '3': // 按下'3'键，向右旋转场景
			gluLookAt(0.0, 0.0, 0.01,  
		    -0.001, 0.0, 0.0,      
		    0.0, 1.0, 0.0);      
			break;
	}
}

void redisp(void) {glutPostRedisplay();}

int main(int argc, char **argv)
{
  printf("\n\n碰撞检测 ver 1.0\n\n -使用键盘交互-\n r:\t\t restart simulation\n t:\t\t toggle texturing\n d:\t\t toggle debug mode\n 4,5,6,8:\t move first object\n ESC:\t\t halt simulation\n\n");
  glutInit(&argc, argv);
  glutInitWindowPosition(0,0);
  glutInitWindowSize(400,300);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  win_id = glutCreateWindow("碰撞检测");
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  glutIdleFunc(redisp);
  init();
  glinit();
  glutMainLoop();
  return 0;
}

   
