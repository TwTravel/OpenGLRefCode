// MathDefs.h: interface for the CMathDefs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATHDEFS_H__AACDDD61_CE32_485C_BD07_07442B0568B7__INCLUDED_)
#define AFX_MATHDEFS_H__AACDDD61_CE32_485C_BD07_07442B0568B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define M_PI        3.14159265358979323846f
#define HALF_PI	    1.57079632679489661923f

//  �йؽǶȻ���ĺ궨��
#define DEGTORAD(A)	((A * M_PI) / 180.0f)
#define RADTODEG(A)	((A * 180.0f) / M_PI)

#define MAKEVECTOR(a,vx,vy,vz)	a.x = vx; a.y = vy; a.z = vz;

typedef unsigned char uchar;

typedef int		BOOL;
typedef unsigned int	uint;
typedef unsigned short  ushort;
typedef unsigned char	byte;

typedef struct
{
	float u,v;
} t2DCoord;

typedef struct
{
	union 
	{
		float x;
		float u;
		float r;
	};
	union 
	{
		float y;
		float v;
		float g;
	};
	union 
	{
		float z;
		float w;
		float b;
	};
} tVector;

// ����������ݽṹ
typedef struct
{
	float m[16];
} tMatrix;

// �������ݽṹ(������������Ͷ�����ɫ)
typedef struct
{
	float r,g,b;
	float x,y,z;
} tColoredVertex;

typedef struct
{
	float u,v;
	float x,y,z;
} tTexturedVertex;

typedef struct
{
	float u,v;
	float r,g,b;
	float x,y,z;
} tTexturedColoredVertex;

//  ��Ԫ���ṹ����
typedef struct
{
	float x,y,z,w;
} tQuaternion;

class CMathDefs  
{
public:
	CMathDefs();
	virtual ~CMathDefs();

	void	MultVectorByMatrix(tMatrix *mat, tVector *v,tVector *result);
	void	MultVectorByRotMatrix(tMatrix *mat, tVector *v,tVector *result);
	double	VectorSquaredLength(tVector *v); 
	double	VectorLength(tVector *v); 
	void	NormalizeVector(tVector *v); 
	double	DotProduct(tVector *v1, tVector *v2);
	void	CrossProduct(tVector *v1, tVector *v2, tVector *result);
	double	VectorSquaredDistance(tVector *v1, tVector *v2);
	void	ScaleVector(tVector *v, float scale, tVector *result);
	void	VectorSum(tVector *v1, tVector *v2, tVector *result);
	void	VectorDifference(tVector *v1, tVector *v2, tVector *result);
	void	IdentityMatrix(tMatrix *mat);

};

#endif // !defined(AFX_MATHDEFS_H__AACDDD61_CE32_485C_BD07_07442B0568B7__INCLUDED_)
