
#ifndef MILKSHAPEMODEL_H
#define MILKSHAPEMODEL_H

#include "Model.h"

class MilkshapeModel : public Model
{
	public:
		// ���캯��
		MilkshapeModel();

		// ��������
		virtual ~MilkshapeModel();

		//  װ��ģ�����ݵ�˽�б�����
		virtual bool loadModelData( const char *filename );
};

#endif 
