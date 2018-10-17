
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "win_control.h"
#include "matrix.h"

#define IMG_SIZE 128
#define MAX_GEN_DEPTH 4 // What is our maximum tesselation going to be
#define EARTH_SIZE 10.0f // What is the size of earth
#define EARTH_ROTATION 15.0f // How fast does the earth rotate
#define EARTH_DAYS (24.0f*EARTH_ROTATION) // What is one earth day equal to 
#define EARTH_TIMEGETAROUND 1.0f// How many seconds does it take the earth to get around the sun
#define DISTANCE_MULT (23439.795427215016f*EARTH_SIZE)// What is the distance from the sun to the earth
#define ISOPOINTS 12
#define NUM_FACES 20
#define XP 0.525731112119133606f
#define ZP 0.850650808352039932f
#define DEFAULT_LOCK_FPS 30.0f// What do we want to lock fps at?

BOOL animate=TRUE;
float lock_fps = DEFAULT_LOCK_FPS;
float subdivide = 10.0f;// Starting subdivision variable
float distance=-1000.0f;// Starting distance backward
BOOL draw_mode=FALSE;//wireframe or non wireframe

//// The Default GEOSPHERE
vector3 iso_vertex[ISOPOINTS]=
{
	{-XP,0.0f,ZP},{XP,0.0f,ZP},{-XP,0.0f,-ZP},{XP,0.0f,-ZP},
	{0.0f,ZP,XP},{0.0f,ZP,-XP},{0.0f,-ZP,XP},{0.0f,-ZP,-XP},
	{ZP,XP,0.0f},{-ZP,XP,0.0f},{ZP,-XP,0.0f},{-ZP,-XP,0.0f}
};

unsigned int tindices[NUM_FACES][3] = 
{
	{0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
	{8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
	{7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
	{6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}
};

lod_geo *lod_geosphere[NUM_FACES];

geosphere planets;


void key_check(void);
void draw_screen(void);
void initialize(void);
void destroy(void);
void get_texture_coord(vector3 *vo, float *tu, float *tv);
void initialize_geosphere(void);
void free_geoside(lod_geo *geoside);
void initialize_geoside(lod_geo *side,unsigned int depth);
void geoside_generate_info(lod_geo *side);
void initialize_point(float *point,vertex *vert);
void draw_lod_geoside(lod_geo *side,float depth);
void draw_geosphere(geosphere *geo,float distance);
int load_texture(const char *tex);
void initialize_geospheres_realistic(void);
void initialize_geospheres_close(void);

float v1[3],v2[3],v3[3];
float temp;

///////////////////////////////////////////////////////////////////////
/// void draw_lod_geoside(lod_geo *side)
/// recursively draws a geosphere side
/// inputs:
/// side what is the current vertices for the side we are rendering
/// depth current depth
///////////////////////////////////////////////////////////////////////
void draw_lod_geoside(lod_geo *side,float depth)
{
	if(depth<=1.0f || !side->next[0])
	{
		
		glColor3f(1.0f,1.0f,1.0);

		if(!side->next[0])
			depth = 1.0f;
		if(side->tri[0].from && side->tri[0].from<1.0f)
		{
			temp = depth*(1.0f-side->tri[0].from)+side->tri[0].from;
			v1[0] = side->tri[0].ind[0]*temp;
			v1[1] = side->tri[0].ind[1]*temp;
			v1[2] = side->tri[0].ind[2]*temp;
		}
		else
			memcpy(v1,side->tri[0].ind,sizeof(float)*3);

		if(side->tri[1].from && side->tri[1].from<1.0f)
		{
			temp = depth*(1.0f-side->tri[1].from)+side->tri[1].from;
			v2[0] = side->tri[1].ind[0]*temp;
			v2[1] = side->tri[1].ind[1]*temp;
			v2[2] = side->tri[1].ind[2]*temp;
		}
		else
			memcpy(v2,side->tri[1].ind,sizeof(float)*3);

		if(side->tri[2].from && side->tri[2].from<1.0f)
		{
			temp = depth*(1.0f-side->tri[2].from)+side->tri[2].from;
			v3[0] = side->tri[2].ind[0]*temp;
			v3[1] = side->tri[2].ind[1]*temp;
			v3[2] = side->tri[2].ind[2]*temp;
		}
		else
			memcpy(v3,side->tri[2].ind,sizeof(float)*3);


		glNormal3fv(side->tri[0].norm);
		glTexCoord2fv(side->tri[0].texc);
		glVertex3fv(v1);

		glNormal3fv(side->tri[1].norm);
		glTexCoord2fv(side->tri[1].texc);
		glVertex3fv(v2);

		glNormal3fv(side->tri[2].norm);
		glTexCoord2fv(side->tri[2].texc);
		glVertex3fv(v3);
	}
	else
	{
		draw_lod_geoside(side->next[0],depth-1.0f);
		draw_lod_geoside(side->next[1],depth-1.0f);
		draw_lod_geoside(side->next[2],depth-1.0f);
		draw_lod_geoside(side->next[3],depth-1.0f);
	}
}

///////////////////////////////////////////////////////////////////////
/// void initiailze_geosphere(void)
/// This function initiailizes the memory for the recursive geosphere
/// structure. and allocated the first faces of the geosphere.
///////////////////////////////////////////////////////////////////////
void initialize_geosphere(void)
{
	lod_geo *geoside;
	int fcnt;
	int cnt;
	memset(lod_geosphere,0,sizeof(void*)*NUM_FACES);

	for(fcnt=0; fcnt<NUM_FACES; fcnt++)
	{
		if(geoside = (lod_geo*)malloc(sizeof(lod_geo)))
		{
			lod_geosphere[fcnt] = geoside;

			memset(geoside,0,sizeof(lod_geo));
			for(cnt=0; cnt<3; cnt++)
			{
				memcpy(geoside->tri[cnt].ind,iso_vertex[tindices[fcnt][cnt]].ind,sizeof(vector3));
			}
			initialize_geoside(geoside,0);
			geoside_generate_info(geoside);
		}
	}
}

///////////////////////////////////////////////////////////////////////
/// void geoside_generate_info(lod_geo *side)
/// This function precomputs the normalized normal, and tex coord for the
/// face. It is recursive
/// inputs:
/// side: The face of interest
///////////////////////////////////////////////////////////////////////
void geoside_generate_info(lod_geo *side)
{
	if(side)
	{
		int cnt;
		float distance;
		float vabs[3];
		float *ve1,*ve2,*ve3;

		for(cnt=0; cnt<3; cnt++)
		{
			memcpy(side->tri[cnt].norm,side->tri[cnt].ind,sizeof(float)*3);

			distance = (float)sqrt(SQ(side->tri[cnt].norm[0])+SQ(side->tri[cnt].norm[1])+SQ(side->tri[cnt].norm[2]));
			if(distance)
				distance = 1.0f/distance;
			side->tri[cnt].norm[0]*=distance;
			side->tri[cnt].norm[1]*=distance;
			side->tri[cnt].norm[2]*=distance;
		}

		ve1 = side->tri[0].texc;
		ve2 = side->tri[1].texc;
		ve3 = side->tri[2].texc;

		get_texture_coord((vector3*)side->tri[0].norm, &ve1[1], &ve1[0]);
		get_texture_coord((vector3*)side->tri[1].norm, &ve2[1], &ve2[0]);
		get_texture_coord((vector3*)side->tri[2].norm, &ve3[1], &ve3[0]);

		vabs[0] = (float)fabs(ve2[0]-ve1[0]);
		vabs[1] = (float)fabs(ve3[0]-ve2[0]);
		vabs[2] = (float)fabs(ve1[0]-ve3[0]);

		if(vabs[0]>0.5f && vabs[1]<0.5f)
			ve1[0] = ve1[0]>0.0f?-ve1[0]:1.0f+ve1[0];

		if(vabs[1]>0.5f && vabs[2]<0.5f)
			ve2[0] = ve2[0]>0.0f?-ve2[0]:1.0f+ve2[0];

		if(vabs[2]>0.5f && vabs[0]<0.5f)
			ve3[0] = ve3[0]>0.0f?-ve3[0]:1.0f+ve3[0];

		for(cnt=0; cnt<4; cnt++)
			geoside_generate_info(side->next[cnt]);
	}
}

///////////////////////////////////////////////////////////////////////
/// void initialize_geoside(lod_geo *side,unsigned int depth)
/// This function recursively allocates the memory for the sides and 
/// create the vertex locations
/// inputs:
/// side: Current Side Of Interest
/// depth: Current Depth That We are Generating for
///////////////////////////////////////////////////////////////////////
void initialize_geoside(lod_geo *side,unsigned int depth)
{	
	if(depth==MAX_GEN_DEPTH)
		return;
	side->next[0] = (lod_geo*)malloc(sizeof(lod_geo));
	memset(side->next[0],0,sizeof(lod_geo));
	side->next[1] = (lod_geo*)malloc(sizeof(lod_geo));
	memset(side->next[1],0,sizeof(lod_geo));
	side->next[2] = (lod_geo*)malloc(sizeof(lod_geo));
	memset(side->next[2],0,sizeof(lod_geo));
	side->next[3] = (lod_geo*)malloc(sizeof(lod_geo));
	memset(side->next[3],0,sizeof(lod_geo));

	{
		lod_geo *newside;
		int cnt;
		float newv1[3],newv2[3],newv3[3];

		for(cnt=0;cnt<3;cnt++)
		{
			newv1[cnt] = (side->tri[0].ind[cnt]+side->tri[1].ind[cnt])*0.5f;
			newv2[cnt] = (side->tri[1].ind[cnt]+side->tri[2].ind[cnt])*0.5f;
			newv3[cnt] = (side->tri[2].ind[cnt]+side->tri[0].ind[cnt])*0.5f;
		}
		newside = side->next[0];

		initialize_point(side->tri[0].ind,&newside->tri[0]);
		initialize_point(newv1,&newside->tri[1]);
		initialize_point(newv3,&newside->tri[2]);

		newside = side->next[1];

		initialize_point(side->tri[1].ind,&newside->tri[0]);
		initialize_point(newv2,&newside->tri[1]);
		initialize_point(newv1,&newside->tri[2]);

		newside = side->next[2];

		initialize_point(side->tri[2].ind,&newside->tri[0]);
		initialize_point(newv3,&newside->tri[1]);
		initialize_point(newv2,&newside->tri[2]);

		newside = side->next[3];

		initialize_point(newv1,&newside->tri[0]);
		initialize_point(newv2,&newside->tri[1]);
		initialize_point(newv3,&newside->tri[2]);
	}

	initialize_geoside(side->next[0],depth+1);
	initialize_geoside(side->next[1],depth+1);
	initialize_geoside(side->next[2],depth+1);
	initialize_geoside(side->next[3],depth+1);
}


///////////////////////////////////////////////////////////////////////
/// void initialize_point(float *point, vertex *vert)
/// This function initializes a point from the given non-normalized
/// vertex of the  base face.
/// inputs:
/// point: Our point which we are copying from
/// vert: Our destination vertex
///////////////////////////////////////////////////////////////////////
void initialize_point(float *point,vertex *vert)
{
	memcpy(vert->ind,point,sizeof(float)*3);
	vert->from = (float)sqrt(SQ(vert->ind[0])+SQ(vert->ind[1])+SQ(vert->ind[2]));
	if(vert->from)
	{
		vert->ind[0]/=vert->from;
		vert->ind[1]/=vert->from;
		vert->ind[2]/=vert->from;
	}
}


///////////////////////////////////////////////////////////////////////
/// void free_geoside(lod_geo *geoside)
/// This function frees the geosides
///////////////////////////////////////////////////////////////////////
//  
void free_geoside(lod_geo *geoside)
{
	int cnt;
	if(geoside)
	{
		for(cnt=0; cnt<4; cnt++)
			free_geoside(geoside->next[cnt]);
		free(geoside);
	}
}


//  下面的函数的功能是设置球面纹理坐标
void get_texture_coord(vector3 *vo, float *tu, float *tv)
{
	float u1[2];
	float D=0.0f;
	D = (float)sqrt(SQ(vo->ind[0]) + SQ(vo->ind[2]));
	if(D)
		D = 1.0f/D;
	u1[0] = vo->ind[0]*D;
	u1[1] = vo->ind[2]*D;
	*tv = (float)acos(u1[0])/M_PI*0.5f;
	if(u1[1] > 0.0f)
		*tv = -*tv;
	*tu = -(0.5f+(float)asin(vo->ind[1])/M_PI);
}

//  下面的函数将raw文件读入到纹理数据中
int load_texture(const char *tex)
{
	FILE *fp;
	unsigned char imgData[IMG_SIZE*IMG_SIZE*3];
	if(fp = fopen(tex,"r+b"))
	{
		int cor_tex_num;
		fread(imgData,sizeof(imgData),1,fp);
		fclose(fp);
		//  生成纹理
		glGenTextures(1,&cor_tex_num);
		glBindTexture(GL_TEXTURE_2D, cor_tex_num);
		//  定义纹理映射参数
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

 		gluBuild2DMipmaps(GL_TEXTURE_2D,3,IMG_SIZE,IMG_SIZE,GL_RGB,GL_UNSIGNED_BYTE,imgData);
		return cor_tex_num;
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////
/// void initialize_geospheres_close(void)
/// This function initializes the planets close enough to see them
///////////////////////////////////////////////////////////////////////
void initialize_geospheres_close(void)
{
	//sun	
	planets.distance = DISTANCE_MULT * 0.0f;
	planets.radius = EARTH_SIZE*109.12f;
	planets.angle_speed = EARTH_DAYS * 0.0833333f;
	planets.rotation_speed= EARTH_TIMEGETAROUND * 0.0f;

}

//  下面的函数的功能是初始化场景
void initialize(void)
{	
	float mat_specular[] = {1.0f,1.0f,1.0f,1.0f};
	float mat_shininess[] = {15.0f};
	float light_ambient[] = {0.5f,0.5f,0.5f,1.0f};
	float light_spec[] = {1.0f,1.0f,1.0f,1.0f};
	float light_diff[] = {1.0f,1.0f,1.0f,1.0f};

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);

	//  设置光照和材质
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
	glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diff);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light_spec);

	initialize_geosphere();

	//  赋予纹理
	planets.texture = load_texture("sun.raw");

	initialize_geospheres_close();

	matrix_init(camera);
	camera_move(0.0f,0.0f,distance);
}

//  下面的函数的功能是删除程序中动态分配的存储资源和纹理
void destroy(void)
{
	int cnt;
	for(cnt=0; cnt<NUM_FACES; cnt++)
		free_geoside(lod_geosphere[cnt]);

		glDeleteTextures(1,&planets.texture);
}

//  下面的函数的功能是求两个矢量的点乘
float dot_product(vector3* u, vector3* v)
{
  return u->ind[0] * v->ind[0] + u->ind[1] * v->ind[1] + u->ind[2] * v->ind[2]; 
}


//  下面函数的功能是调整场景中的LOD
void adjust_lod(void)
{
	if(1.0f/fDeltaTime<lock_fps)
		subdivide+=SQ((1.0f/fDeltaTime-lock_fps)*0.02f);
	else
		subdivide-=SQ((1.0f/fDeltaTime-lock_fps)*0.02f);
	if(subdivide<0.5f)
		subdivide = 0.5f;
	if(subdivide>10.0f)
		subdivide = 10.0f;
}

//  下面的函数的功能是根据摄像机与行星的距离绘制行星
void draw_geosphere(geosphere *geo,float distance)
{
	vector3 normal,v1,v2;
	float inverse_rot[MATRIX];
	float temp[MATRIX];
	float object_subdivide;
	int cnt;
	float max_polys = (1.0f/distance)*(pixelwidth*geo->radius);
	object_subdivide = (float)pow((M_PI*SQ(max_polys))/20.0f,1.0f/3.0f);
	object_subdivide /= subdivide;

	// 对象的细分层次
	if(object_subdivide>10.0f)
		object_subdivide = 10.0f;
	if(object_subdivide<2.0f)
		object_subdivide = 2.0f;

	glBindTexture(GL_TEXTURE_2D,geo->texture);

	glRotatef(geo->angle,0.0f,1.0f,0.0f);
	glScalef(geo->radius,geo->radius,geo->radius);

	matrix_init(inverse_rot);
	inverse_rot[ind(3,0)] = geo->distance;
	inverse_rot[ind(3,1)] = 0.0f;
	inverse_rot[ind(3,2)] = 0.0f;
	matrix_y_rotate(RAD(geo->rotation),inverse_rot);

	matrix_init(temp);
	matrix_y_rotate(RAD(geo->angle),temp);

	matrix_mult(inverse_rot,temp);

	glColor3f(1.0f,1.0f,1.0);
	glBegin(GL_TRIANGLES);
	for(cnt = 0;cnt<NUM_FACES;cnt++)
	{
		v1.ind[0] = iso_vertex[tindices[cnt][2]].ind[0]-iso_vertex[tindices[cnt][0]].ind[0];
		v1.ind[1] = iso_vertex[tindices[cnt][2]].ind[1]-iso_vertex[tindices[cnt][0]].ind[1];
		v1.ind[2] = iso_vertex[tindices[cnt][2]].ind[2]-iso_vertex[tindices[cnt][0]].ind[2];

		v2.ind[0] = iso_vertex[tindices[cnt][1]].ind[0]-iso_vertex[tindices[cnt][0]].ind[0];
		v2.ind[1] = iso_vertex[tindices[cnt][1]].ind[1]-iso_vertex[tindices[cnt][0]].ind[1];
		v2.ind[2] = iso_vertex[tindices[cnt][1]].ind[2]-iso_vertex[tindices[cnt][0]].ind[2];

		cross_product(&v1,&v2,&normal);

		matrix_mult_vector(inverse_rot,&normal);

		if(dot_product(&normal,&view_dir)>=0.0f-(object_subdivide/10.0f))
			draw_lod_geoside(lod_geosphere[cnt],object_subdivide-1.0f);
	}
	glEnd();
}


//  下面的函数处理键盘响应
void key_check(void)
{
	if(TheSysKey == VK_ESCAPE)
		b_done = TRUE;
	if(TheSysKey == VK_LEFT)
		matrix_y_rotate(0.1f,camera);
	if(TheSysKey == VK_RIGHT)
		matrix_y_rotate(-0.1f,camera);
	if(TheSysKey == VK_UP)
		camera_move(0.0f,0.0f,50.0f);
	if(TheSysKey == VK_DOWN)
		camera_move(0.0f,0.0f,-50.0f);
	if(TheKey == 's')
		matrix_x_rotate(-0.1f,camera);
	if(TheKey == 'x')
		matrix_x_rotate(0.1f,camera);
	if(TheSysKey == VK_PAGEUP)
		matrix_z_rotate(-0.1f,camera);
	if(TheSysKey == VK_PAGEDOWN)
		matrix_z_rotate(0.1f,camera);
	if(TheSysKey == VK_END)
		camera_stop();
	if(TheKey == 'a')
		camera_move(0.0f,50.0f,0.0f);
	if(TheKey == 'z')
		camera_move(0.0f,-50.0f,0.0f);
	if(TheKey == 'q')
		camera_move(50.0f,0.0f,0.0f);
	if(TheKey == 'w')
		camera_move(-50.0f,0.0f,0.0f);
	if(TheKey == ' ')
		animate = !animate;
	if(TheKey == '-')
		lock_fps -= 1.0f;
	if(TheKey == '+')
		lock_fps += 1.0f;
	if(TheKey == 'f')
		draw_mode = !draw_mode;
}

//  该函数计算与行星的距离，并绘制这些行星
void draw_screen(void)
{
	float angle_to_cam;
	float distance;
	float near_distance=2.0f;
	float mat[MATRIX];
	vector3 plan_pos;
	float light_pos[] = {0.0f,0.0f,0.0f,1.0f};
	// 判断绘制模式
	if(draw_mode)
	{
		glPolygonMode(GL_FRONT,GL_LINE);
		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		glPolygonMode(GL_FRONT,GL_FILL);
		glEnable(GL_TEXTURE_2D);
	}
	if(animate)
	{
		camera[ind(3,0)] += camera_speed.ind[0]*fDeltaTime;
		camera[ind(3,1)] += camera_speed.ind[1]*fDeltaTime;
		camera[ind(3,2)] += camera_speed.ind[2]*fDeltaTime;
	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-glAspect*near_distance, glAspect*near_distance, -near_distance, near_distance, near_distance*3.0f, 1000000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	camera_position();	//  获取摄像机位置
	glLightfv(GL_LIGHT0,GL_POSITION,light_pos);
	adjust_lod();		//  调整LOD细节层次
	glDisable(GL_LIGHTING);
	glPushMatrix();
	if(animate)
	{
		planets.rotation+=planets.rotation_speed*fDeltaTime;
		planets.angle+=planets.angle_speed*fDeltaTime;
	}
	glRotatef(planets.rotation,0.0f,1.0f,0.0f);
	glTranslatef(planets.distance,0.0f,0.0f);

	plan_pos.ind[0] = planets.distance;
	plan_pos.ind[1] = 0.0f;
	plan_pos.ind[2] = 0.0f;
	matrix_init(mat);
	matrix_y_rotate(RAD(planets.rotation),mat);
	matrix_mult_vector(mat,&plan_pos);
	plan_pos.ind[0] -= camera[ind(3,0)];
	plan_pos.ind[1] -= camera[ind(3,1)];
	plan_pos.ind[2] -= camera[ind(3,2)];
	distance = (float)sqrt(SQ(plan_pos.ind[0])+SQ(plan_pos.ind[1])+SQ(plan_pos.ind[2]));
	if(distance-planets.radius<1000000.0f && distance>planets.radius)
	{
		plan_pos.ind[0] /= distance;
		plan_pos.ind[1] /= distance;
		plan_pos.ind[2] /= distance;
		angle_to_cam = dot_product(&view_dir,&plan_pos);
		if(angle_to_cam<-0.5f)
			draw_geosphere(&planets,distance*(1.1f-((-angle_to_cam-0.5f)*2.0f)));// adjust distance so objects near center of screen get more detail
	}
	glPopMatrix();
}