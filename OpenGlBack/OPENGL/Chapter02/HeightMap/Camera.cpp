
#pragma comment(lib, "winmm.lib")

#include "main.h"
#include "Camera.h"


#define kSpeed	50.0f									

float g_FrameInterval = 0.0f;


//  ����ĺ����Ĺ����Ǽ���֡��
void CalculateFrameRate()
{
	static float framesPerSecond    = 0.0f;		// ����֡��
    static float lastTime			= 0.0f;		// ���֡��ʾ��ʱ��
	static char strFrameRate[50] = {0};			// ���ڱ����ַ���

	static float frameTime = 0.0f;	

	// ��õ�ǰϵͳʱ��
    float currentTime = timeGetTime() * 0.001f;				

	//  ���ʱ����
 	g_FrameInterval = currentTime - frameTime;
	frameTime = currentTime;

	// ֡����������
    ++framesPerSecond;

	//  ʱ���Ƿ񳬹�1��
    if( currentTime - lastTime > 1.0f )
    {
		// ���õ�ǰʱ��
	    lastTime = currentTime;
		
		// ��ӡ����
		sprintf(strFrameRate, "����ģ�� ��ǰ֡��:ÿ�� %d ֡", int(framesPerSecond));

		// ���ô��ڱ���
		SetWindowText(g_hWnd, strFrameRate);

		// ֡��������λ
        framesPerSecond = 0;
    }
}


//  ����ĺ����Ĺ����Ǽ����ʸ���Ĳ��������������ʸ������ֱ��ʸ��
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vNormal;	

	// ���㴹ֱʸ��
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	// ���ؽ��
	return vNormal;										 
}


//  ����ĺ����Ĺ�������ʸ���ĳ���
float Magnitude(CVector3 vNormal)
{
	return (float)sqrt( (vNormal.x * vNormal.x) + 
						(vNormal.y * vNormal.y) + 
						(vNormal.z * vNormal.z) );
}


//  ����ĺ����Ĺ����ǽ�ʸ����λ��
CVector3 Normalize(CVector3 vVector)
{
	// ���ʸ���ĳ���
	float magnitude = Magnitude(vVector);				

	vVector = vVector / magnitude;		
	
	return vVector;										
}


//  ����ĺ�������CCamera�Ĺ��캯��
CCamera::CCamera()
{
	CVector3 vZero = CVector3(0.0, 0.0, 0.0);		// ��ʼ�������λ��
	CVector3 vView = CVector3(0.0, 1.0, 0.5);		// ��ʼ����������� 
	CVector3 vUp   = CVector3(0.0, 0.0, 1.0);		// ��ʼ������������Ϸ���

	m_vPosition	= vZero;
	m_vView		= vView;
	m_vUpVector	= vUp;	
}


//  �����������λ�á�����
void CCamera::PositionCamera(float positionX, float positionY, float positionZ,
				  		     float viewX,     float viewY,     float viewZ,
							 float upVectorX, float upVectorY, float upVectorZ)
{
	CVector3 vPosition	= CVector3(positionX, positionY, positionZ);
	CVector3 vView		= CVector3(viewX, viewY, viewZ);
	CVector3 vUpVector	= CVector3(upVectorX, upVectorY, upVectorZ);

	m_vPosition = vPosition;
	m_vView     = vView;
	m_vUpVector = vUpVector;
}


//  ����ĺ����Ĺ�����ͨ����������ӵ�
void CCamera::SetViewByMouse()
{
	POINT mousePos;	
	int middleX = SCREEN_WIDTH  >> 1;
	int middleY = SCREEN_HEIGHT >> 1;
	float angleY = 0.0f;
	float angleZ = 0.0f;
	static float currentRotX = 0.0f;
	
	// ������ĵ�ǰλ��
	GetCursorPos(&mousePos);						
	
	// ������λ�ڴ��ڵ������룬�򷵻�
	if( (mousePos.x == middleX) && (mousePos.y == middleY) ) return;

	// ��������λ��Ϊ����������
	SetCursorPos(middleX, middleY);							

	// ����Ƕ�
	angleY = (float)( (middleX - mousePos.x) ) / 500.0f;		
	angleZ = (float)( (middleY - mousePos.y) ) / 500.0f;		

	currentRotX -= angleZ;  

	if(currentRotX > 1.0f)
		currentRotX = 1.0f;
	else if(currentRotX < -1.0f)
		currentRotX = -1.0f;
	// ��ת�۲췽��
	else
	{
		CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
		vAxis = Normalize(vAxis);

		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
		RotateView(angleY, 0, 1, 0);
	}
}


//  ����ĺ����Ĺ����ǽ�������Ĺ۲췽����ĳ����������תһ���ĽǶ�
void CCamera::RotateView(float angle, float x, float y, float z)
{
	CVector3 vNewView;

	// ��ù۲췽��ʸ��
	CVector3 vView = m_vView - m_vPosition;		

	// ����Ƕȵ�cos��sinֵ
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	// �����µĹ۲������X
	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* vView.z;

	// �����µĹ۲������Y
	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.z;

	// �����µĹ۲������Z
	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;
	m_vView = m_vPosition + vNewView;
}

//  ����ĺ����Ĺ��������������ƶ������
void CCamera::StrafeCamera(float speed)
{	
	// Add the strafe vector to our position
	m_vPosition.x += m_vStrafe.x * speed;
	m_vPosition.z += m_vStrafe.z * speed;

	// Add the strafe vector to our view
	m_vView.x += m_vStrafe.x * speed;
	m_vView.z += m_vStrafe.z * speed;
}

//  ����ĺ����Ĺ����Ǹ���һ�����ٶ�ǰ���ƶ������
void CCamera::MoveCamera(float speed)
{
	// ��õ�ǰ���������
	CVector3 vVector = m_vView - m_vPosition;
	vVector = Normalize(vVector);

	m_vPosition.x += vVector.x * speed;		// �ƶ��������λ������X
	m_vPosition.y += vVector.y * speed;		// �ƶ��������λ������Y
	m_vPosition.z += vVector.z * speed;		// �ƶ��������λ������Z
	m_vView.x += vVector.x * speed;			// �����X�����ƶ�
	m_vView.y += vVector.y * speed;			// �����Y�����ƶ�
	m_vView.z += vVector.z * speed;			// �����Z�����ƶ�
}


//  ����ĺ����Ĺ����Ǹ��ݲ�ͬ�İ������ƶ������
void CCamera::CheckForMovement()
{	
	// ��õ�ǰ֡��
	float speed = kSpeed * g_FrameInterval;

	// �Ƿ���UP��ͷ����'W'��
	if(GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {				

		// �ƶ������
		MoveCamera(speed);				
	}

	// �Ƿ���DOWN����'S'��
	if(GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {			

		// �ƶ������
		MoveCamera(-speed);				
	}

	// �Ƿ���LEFT��ͷ����'A'��
	if(GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {			

		// �ƶ������
		StrafeCamera(-speed);
	}

	// �Ƿ���RIGHT��ͷ����'D'��
	if(GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {			

		// �ƶ������
		StrafeCamera(speed);
	}	
}


//  ����ĺ����Ĺ����Ǹ������������
void CCamera::Update() 
{
	// ��ʼ������
	CVector3 vCross = Cross(m_vView - m_vPosition, m_vUpVector);

	// �淶��ʸ��
	m_vStrafe = Normalize(vCross);

	// �ƶ����������
	SetViewByMouse();

	// �ж��Ƿ��а���
	CheckForMovement();
	
	// ����֡��
	CalculateFrameRate();
}


//  ����ĺ����Ĺ���������ͶӰ�任
void CCamera::Look()
{
	// �����������λ�á���������·�������ͶӰ�任
	gluLookAt(m_vPosition.x, m_vPosition.y, m_vPosition.z,	
			  m_vView.x,	 m_vView.y,     m_vView.z,	
			  m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);
}

