
#ifndef MODEL_H
#define MODEL_H

class Model
{
	public:
		//	网格
		struct Mesh
		{
			int m_materialIndex;
			int m_numTriangles;
			int *m_pTriangleIndices;
		};

		//	材质特性
		struct Material
		{
			float m_ambient[4], m_diffuse[4], m_specular[4], m_emissive[4];
			float m_shininess;
			GLuint m_texture;
			char *m_pTextureFilename;
		};

		//	保存有关三角形信息的结构体
		struct Triangle
		{
			float m_vertexNormals[3][3];
			float m_s[3], m_t[3];
			int m_vertexIndices[3];
		};

		//	保存顶点信息的结构体
		struct Vertex
		{
			char m_boneID;	
			float m_location[3];
		};

	public:
		// 构造函数
		Model();

		// 析构函数
		virtual ~Model();

		//  将模型数据装入到私有变量中	
		virtual bool loadModelData( const char *filename ) = 0;

		//  绘制和渲染模型
		void draw();

		//  重装纹理
		void reloadTextures();

	protected:
		//	使用的网格
		int m_numMeshes;
		Mesh *m_pMeshes;

		//	使用的材质
		int m_numMaterials;
		Material *m_pMaterials;

		//	使用的三角形
		int m_numTriangles;
		Triangle *m_pTriangles;

		//	使用的顶点
		int m_numVertices;
		Vertex *m_pVertices;
};

#endif 