// MathDefs.cpp: implementation of the CMathDefs class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Billiard.h"
#include "MathDefs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMathDefs::CMathDefs()
{

}

CMathDefs::~CMathDefs()
{

}


//  ����ĺ����Ĺ����Ƕ���һ����λ����
void CMathDefs::IdentityMatrix(tMatrix *mat) 
{
	int loop;
	for (loop = 0; loop < 16; loop++)
		mat->m[loop] = 0.0f;
	mat->m[0] = mat->m[5] = mat->m[10] = mat->m[15] = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	MultVectorByMatrix
// Purpose:		Multiplies a vector by a 4x4 Matrix in OpenGL Format
// Arguments:	Matrix, Vector in, and result Vector
// Notes:		This routing is tweaked to handle OpenGLs column-major format
//				This is one obvious place for optimization perhaps asm code
///////////////////////////////////////////////////////////////////////////////
void CMathDefs::MultVectorByMatrix(tMatrix *mat, tVector *v,tVector *result)
{
	result->x = (mat->m[0] * v->x) + (mat->m[4] * v->y) + (mat->m[8] * v->z) +
			     mat->m[12];
	result->y = (mat->m[1] * v->x) + (mat->m[5] * v->y) + (mat->m[9] * v->z) +
			     mat->m[13];
	result->z = (mat->m[2] * v->x) + (mat->m[6] * v->y) + (mat->m[10] * v->z) +
			     mat->m[14];
}
//// MultVectorByMatrix //////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	MultVectorByRotMatrix
// Purpose:		Multiplies a vector by a 4x4 Rotation Matrix in OpenGL Format
// Arguments:	Matrix, Vector in, and result Vector
// Notes:		This routing is tweaked to handle OpenGLs column-major format
//				This is one obvious place for optimization perhaps asm code
///////////////////////////////////////////////////////////////////////////////
void CMathDefs::MultVectorByRotMatrix(tMatrix *mat, tVector *v,tVector *result)
{
	result->x = (mat->m[0] * v->x) + (mat->m[4] * v->y) + (mat->m[8] * v->z);
	result->y = (mat->m[1] * v->x) + (mat->m[5] * v->y) + (mat->m[9] * v->z);
	result->z = (mat->m[2] * v->x) + (mat->m[6] * v->y) + (mat->m[10] * v->z);
}
//// MultVectorByRotMatrix ///////////////////////////////////////////////////


//  ����ĺ����Ĺ�������ʸ��ģ��ƽ������ʸ�����ȵ�ƽ��  
double CMathDefs::VectorSquaredLength(tVector *v) 
{
	return((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

//  ����ĺ����Ĺ�������ʸ����ģ����ʸ���ĳ���
double CMathDefs::VectorLength(tVector *v) 
{
	return(sqrt(VectorSquaredLength(v)));
}

//  ����ĺ����Ĺ�������һ��ʸ���ĵ�λ��ʸ��
void CMathDefs::NormalizeVector(tVector *v) 
{
	float len = (float)VectorLength(v);
    if (len != 0.0) 
	{ 
		v->x /= len;  
		v->y /= len; 
		v->z /= len; 
	}
}

//  ����ĺ����Ĺ�����������ʸ���ĵ��
double CMathDefs::DotProduct(tVector *v1, tVector *v2)
{
	return ((v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z));
}

//  ����ĺ����Ĺ�����������ʸ���Ĳ��
void CMathDefs::CrossProduct(tVector *v1, tVector *v2, tVector *result)
{
	result->x = (v1->y * v2->z) - (v1->z * v2->y);
	result->y = (v1->z * v2->x) - (v1->x * v2->z);
	result->z = (v1->x * v2->y) - (v1->y * v2->x);
}

double CMathDefs::VectorSquaredDistance(tVector *v1, tVector *v2) 
{
	return(	((v1->x - v2->x) * (v1->x - v2->x)) + 
			((v1->y - v2->y) * (v1->y - v2->y)) + 	
			((v1->z - v2->z) * (v1->z - v2->z)) ); 	
}

//  ����ĺ����Ĺ����ǽ�ʸ������һ������
void CMathDefs::ScaleVector(tVector *v, float scale, tVector *result) 
{
	result->x = v->x * scale;
	result->y = v->y * scale;
	result->z = v->z * scale;
}
//  ����ĺ����Ĺ�����������ʸ���ĺ�
void CMathDefs::VectorSum(tVector *v1, tVector *v2, tVector *result) 
{
	result->x = v1->x + v2->x;
	result->y = v1->y + v2->y;
	result->z = v1->z + v2->z;
}
//  ����ĺ����Ĺ�����������ʸ���Ĳ�
void CMathDefs::VectorDifference(tVector *v1, tVector *v2, tVector *result) 
{
	result->x = v1->x - v2->x;
	result->y = v1->y - v2->y;
	result->z = v1->z - v2->z;
}
