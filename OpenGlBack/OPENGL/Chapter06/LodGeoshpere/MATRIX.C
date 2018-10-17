#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include <math.h>
#include <memory.h>
#include "matrix.h"

vector3 camera_speed={0.0f,0.0f,0.0f};	// 摄像机速度方向矢量
float camera[MATRIX];					// 摄像机矩阵
vector3 view_dir={0.0f,0.0f,-1.0f};		// 观看方向


void cross_product(vector3 *v1,vector3 *v2, vector3 *norm)
{
	norm->ind[X] = v1->ind[Y] * v2->ind[Z] - v1->ind[Z] * v2->ind[Y];
  norm->ind[Y] = v1->ind[Z] * v2->ind[X] - v1->ind[X] * v2->ind[Z];
  norm->ind[Z] = v1->ind[X] * v2->ind[Y] - v1->ind[Y] * v2->ind[X];
}


void camera_stop()
{
	camera_speed.ind[0] = 0.0f;
	camera_speed.ind[1] = 0.0f;
	camera_speed.ind[2] = 0.0f;
}

void camera_move(float x,float y,float z)
{
	vector3 vert={x,y,z};
	matrix_mult_vector(camera,&vert);
	camera_speed.ind[0] += vert.ind[0];
	camera_speed.ind[1] += vert.ind[1];
	camera_speed.ind[2] += vert.ind[2];
}

void camera_position(void)
{
	vector3 up_dir={0.0f,1.0f,0.0f};
	view_dir.ind[0] = 0.0f;
	view_dir.ind[1] = 0.0f;
	view_dir.ind[2] = -1.0f;

	matrix_mult_vector(camera,&view_dir);
	matrix_mult_vector(camera,&up_dir);

	gluLookAt(view_dir.ind[0]+camera[ind(3,0)],view_dir.ind[1]+camera[ind(3,1)],view_dir.ind[2]+camera[ind(3,2)],
						camera[ind(3,0)],camera[ind(3,1)],camera[ind(3,2)],
						up_dir.ind[0],up_dir.ind[1],up_dir.ind[2]);
}


void matrix_x_rotate(float angle, float *m)
{
	float r[MATRIX];
	float c = (float)cos(angle), s = (float)sin(angle);

	matrix_init(r);
	r[ind(1,1)] =  c;
	r[ind(1,2)] =  s;
	r[ind(2,1)] = -s;
	r[ind(2,2)] =  c;
	matrix_mult(m, r);
}

void matrix_y_rotate(float angle, float *m)
{
	float r[MATRIX];
	float c = (float)cos(angle), s = (float)sin(angle);

	matrix_init(r);
	r[ind(0,0)] =  c;
	r[ind(0,2)] = -s;
	r[ind(2,0)] =  s;
	r[ind(2,2)] =  c;
	matrix_mult(m, r);
}

void matrix_z_rotate(float angle, float *m)
{
	float r[MATRIX];
	float c = (float)cos(angle), s = (float)sin(angle);

	matrix_init(r);
	r[ind(0,0)] = c;
	r[ind(0,1)] = s;
	r[ind(1,0)] = -s;
	r[ind(1,1)] = c;
	matrix_mult(m, r);
}

void matrix_init(float *m)
{
	memset(m,0,sizeof(float)*MATRIX);
	m[ind(0,0)] = m[ind(1,1)] =m[ind(2,2)] = m[ind(3,3)] = 1.0f;
}

void matrix_mult_vector(const float *m,vector3 *v)
{
	int i,j;
	vector3 t;

	for(i=0; i<3; i++)
	{
		t.ind[i] = 0.0f;
		for(j=0;j<3;j++)
			t.ind[i] += m[ind(j,i)] * v->ind[j];
	}
	*v = t;
}

void matrix_mult(float *m2,const float *m1)
{
	int i,j,k;
	float m[MATRIX];

	matrix_init(m);
	m[ind(3,0)] = m2[ind(3,0)];
	m[ind(3,1)] = m2[ind(3,1)];
	m[ind(3,2)] = m2[ind(3,2)];
	for(i=0; i<3; i++)
	{
	   for(j=0; j<3; j++)
	   {
		   m[ind(i,j)] = 0.0f;
		   for(k=0; k<3; k++)
			   m[ind(i,j)] += m1[ind(i,k)] * m2[ind(k,j)];
		}
	}
	memcpy(m2,m,sizeof(float)*MATRIX);
}
