
#ifndef MILKSHAPEMODEL_H
#define MILKSHAPEMODEL_H

#include "Model.h"

class MilkshapeModel : public Model
{
	public:
		// 构造函数
		MilkshapeModel();

		// 析构函数
		virtual ~MilkshapeModel();

		//  装入模型数据到私有变量中
		virtual bool loadModelData( const char *filename );
};

#endif 
