#ifndef __MATRIX_H__
#define __MATRIX_H__

#define ind(x,y) ((y)+(x)*4)
#define MATRIX 16

#define RAD(x) ((x)/180*M_PI)
#define M_PI 3.141592653589f
#define SQ(x) ((x)*(x))
#define TH(x) ((x)*(x)*(x))
#define SQR(x,y) SQ(x-y)

enum {X=0,Y,Z,W};

typedef struct svertex
{
	float from;
	float norm[3];
	float ind[3];
	float texc[2];
}vertex;

typedef struct svector
{
	float ind[3];
}vector3;

typedef struct lod_geosphere
{
	vertex tri[3];
	struct lod_geosphere *last;
	struct lod_geosphere *next[4];
}lod_geo;

typedef struct geosphere_struct
{
	unsigned int texture;	//  使用的纹理ID
	float distance;			//  球体与视点的距离
	float rotation;			//  球体自旋角度
	float angle;			//  球体的公转角度
	float radius;			//  球体的半径
	float rotation_speed;	//  球体的自选角速度
	float angle_speed;		//  球体的公转角速度
	float one_o_radius;		
}geosphere;

void camera_position(void);
void camera_stop();
void camera_move(float x,float y,float z);

void matrix_z_rotate(float angle, float *m);
void matrix_y_rotate(float angle, float *m);
void matrix_x_rotate(float angle, float *m);
void matrix_mult(float *m2,const float *m1);
void matrix_init(float *m);
void matrix_mult_vector(const float *m,vector3 *v);
void cross_product(vector3 *v1,vector3 *v2, vector3 *norm);

extern vector3 camera_speed;
extern float camera[MATRIX];
extern vector3 view_dir;


#endif