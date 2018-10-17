
#ifndef MODEL_H
#define MODEL_H

class Model
{
	public:
		//	����
		struct Mesh
		{
			int m_materialIndex;
			int m_numTriangles;
			int *m_pTriangleIndices;
		};

		//	��������
		struct Material
		{
			float m_ambient[4], m_diffuse[4], m_specular[4], m_emissive[4];
			float m_shininess;
			GLuint m_texture;
			char *m_pTextureFilename;
		};

		//	�����й���������Ϣ�Ľṹ��
		struct Triangle
		{
			float m_vertexNormals[3][3];
			float m_s[3], m_t[3];
			int m_vertexIndices[3];
		};

		//	���涥����Ϣ�Ľṹ��
		struct Vertex
		{
			char m_boneID;	
			float m_location[3];
		};

	public:
		// ���캯��
		Model();

		// ��������
		virtual ~Model();

		//  ��ģ������װ�뵽˽�б�����	
		virtual bool loadModelData( const char *filename ) = 0;

		//  ���ƺ���Ⱦģ��
		void draw();

		//  ��װ����
		void reloadTextures();

	protected:
		//	ʹ�õ�����
		int m_numMeshes;
		Mesh *m_pMeshes;

		//	ʹ�õĲ���
		int m_numMaterials;
		Material *m_pMaterials;

		//	ʹ�õ�������
		int m_numTriangles;
		Triangle *m_pTriangles;

		//	ʹ�õĶ���
		int m_numVertices;
		Vertex *m_pVertices;
};

#endif 