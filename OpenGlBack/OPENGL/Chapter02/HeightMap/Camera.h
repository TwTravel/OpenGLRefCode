#ifndef _CAMERA_H
#define _CAMERA_H


// �������
class CCamera {

public:

	// �������Ĺ��캯��
	CCamera();	

	// ����ĺ����ǻ�ȡ�й������������
	CVector3 Position() {	return m_vPosition;		}
	CVector3 View()		{	return m_vView;			}
	CVector3 UpVector() {	return m_vUpVector;		}
	CVector3 Strafe()	{	return m_vStrafe;		}
	
	//  �����λ��
	void PositionCamera(float positionX, float positionY, float positionZ,
			 		    float viewX,     float viewY,     float viewZ,
						float upVectorX, float upVectorY, float upVectorZ);

	// ��ת�����
	void RotateView(float angle, float X, float Y, float Z);

	// �ƶ��ӵ�
	void SetViewByMouse(); 

	// ��һ����ת�����
	void RotateAroundPoint(CVector3 vCenter, float X, float Y, float Z);
	//  �����ƶ������
	void StrafeCamera(float speed);
	//  �ƶ������
	void MoveCamera(float speed);
	// �����¼�
	void CheckForMovement();
	void Update();
	void Look();

private:

	// �������λ��
	CVector3 m_vPosition;					
	// ���������Ұ
	CVector3 m_vView;						
	// ����������ϵ�λ��
	CVector3 m_vUpVector;		
	//  ��������ҷ���	
	CVector3 m_vStrafe;						
};

#endif

