
#pragma comment(lib, "winmm.lib")

#include "main.h"
#include "Camera.h"


#define kSpeed	50.0f									

float g_FrameInterval = 0.0f;


//  下面的函数的功能是计算帧率
void CalculateFrameRate()
{
	static float framesPerSecond    = 0.0f;		// 保存帧率
    static float lastTime			= 0.0f;		// 最好帧显示的时刻
	static char strFrameRate[50] = {0};			// 窗口标题字符串

	static float frameTime = 0.0f;	

	// 获得当前系统时间
    float currentTime = timeGetTime() * 0.001f;				

	//  获得时间间隔
 	g_FrameInterval = currentTime - frameTime;
	frameTime = currentTime;

	// 帧计数器递增
    ++framesPerSecond;

	//  时间是否超过1秒
    if( currentTime - lastTime > 1.0f )
    {
		// 设置当前时间
	    lastTime = currentTime;
		
		// 打印标题
		sprintf(strFrameRate, "地形模拟 当前帧率:每秒 %d 帧", int(framesPerSecond));

		// 设置窗口标题
		SetWindowText(g_hWnd, strFrameRate);

		// 帧计数器复位
        framesPerSecond = 0;
    }
}


//  下面的函数的功能是计算个矢量的叉积，即求与两个矢量都垂直的矢量
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vNormal;	

	// 计算垂直矢量
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	// 返回结果
	return vNormal;										 
}


//  下面的函数的功能是求矢量的长度
float Magnitude(CVector3 vNormal)
{
	return (float)sqrt( (vNormal.x * vNormal.x) + 
						(vNormal.y * vNormal.y) + 
						(vNormal.z * vNormal.z) );
}


//  下面的函数的功能是将矢量单位化
CVector3 Normalize(CVector3 vVector)
{
	// 获得矢量的长度
	float magnitude = Magnitude(vVector);				

	vVector = vVector / magnitude;		
	
	return vVector;										
}


//  下面的函数是类CCamera的构造函数
CCamera::CCamera()
{
	CVector3 vZero = CVector3(0.0, 0.0, 0.0);		// 初始化摄像机位置
	CVector3 vView = CVector3(0.0, 1.0, 0.5);		// 初始化摄像机方向 
	CVector3 vUp   = CVector3(0.0, 0.0, 1.0);		// 初始化摄像机的向上方向

	m_vPosition	= vZero;
	m_vView		= vView;
	m_vUpVector	= vUp;	
}


//  设置摄像机的位置、方向
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


//  下面的函数的功能是通过鼠标设置视点
void CCamera::SetViewByMouse()
{
	POINT mousePos;	
	int middleX = SCREEN_WIDTH  >> 1;
	int middleY = SCREEN_HEIGHT >> 1;
	float angleY = 0.0f;
	float angleZ = 0.0f;
	static float currentRotX = 0.0f;
	
	// 获得鼠标的当前位置
	GetCursorPos(&mousePos);						
	
	// 如果鼠标位于窗口的正中央，则返回
	if( (mousePos.x == middleX) && (mousePos.y == middleY) ) return;

	// 设置鼠标的位置为窗口正中央
	SetCursorPos(middleX, middleY);							

	// 计算角度
	angleY = (float)( (middleX - mousePos.x) ) / 500.0f;		
	angleZ = (float)( (middleY - mousePos.y) ) / 500.0f;		

	currentRotX -= angleZ;  

	if(currentRotX > 1.0f)
		currentRotX = 1.0f;
	else if(currentRotX < -1.0f)
		currentRotX = -1.0f;
	// 旋转观察方向
	else
	{
		CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
		vAxis = Normalize(vAxis);

		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
		RotateView(angleY, 0, 1, 0);
	}
}


//  下面的函数的功能是将摄像机的观察方向绕某个方向轴旋转一定的角度
void CCamera::RotateView(float angle, float x, float y, float z)
{
	CVector3 vNewView;

	// 获得观察方向矢量
	CVector3 vView = m_vView - m_vPosition;		

	// 计算角度的cos和sin值
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	// 计算新的观察点坐标X
	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* vView.z;

	// 计算新的观察点坐标Y
	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.z;

	// 计算新的观察点坐标Z
	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;
	m_vView = m_vPosition + vNewView;
}

//  下面的函数的功能是向左向右移动摄像机
void CCamera::StrafeCamera(float speed)
{	
	// Add the strafe vector to our position
	m_vPosition.x += m_vStrafe.x * speed;
	m_vPosition.z += m_vStrafe.z * speed;

	// Add the strafe vector to our view
	m_vView.x += m_vStrafe.x * speed;
	m_vView.z += m_vStrafe.z * speed;
}

//  下面的函数的功能是根据一定的速度前后移动摄像机
void CCamera::MoveCamera(float speed)
{
	// 获得当前摄像机方向
	CVector3 vVector = m_vView - m_vPosition;
	vVector = Normalize(vVector);

	m_vPosition.x += vVector.x * speed;		// 移动摄像机的位置坐标X
	m_vPosition.y += vVector.y * speed;		// 移动摄像机的位置坐标Y
	m_vPosition.z += vVector.z * speed;		// 移动摄像机的位置坐标Z
	m_vView.x += vVector.x * speed;			// 摄像机X方向移动
	m_vView.y += vVector.y * speed;			// 摄像机Y方向移动
	m_vView.z += vVector.z * speed;			// 摄像机Z方向移动
}


//  下面的函数的功能是根据不同的按键，移动摄像机
void CCamera::CheckForMovement()
{	
	// 获得当前帧率
	float speed = kSpeed * g_FrameInterval;

	// 是否按下UP箭头键或'W'键
	if(GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {				

		// 移动摄像机
		MoveCamera(speed);				
	}

	// 是否按下DOWN键或'S'键
	if(GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {			

		// 移动摄像机
		MoveCamera(-speed);				
	}

	// 是否按下LEFT箭头键或'A'键
	if(GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {			

		// 移动摄像机
		StrafeCamera(-speed);
	}

	// 是否按下RIGHT箭头键或'D'键
	if(GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {			

		// 移动摄像机
		StrafeCamera(speed);
	}	
}


//  下面的函数的功能是更新摄像机方向
void CCamera::Update() 
{
	// 初始化变量
	CVector3 vCross = Cross(m_vView - m_vPosition, m_vUpVector);

	// 规范化矢量
	m_vStrafe = Normalize(vCross);

	// 移动摄像机方向
	SetViewByMouse();

	// 判断是否有按键
	CheckForMovement();
	
	// 计算帧率
	CalculateFrameRate();
}


//  下面的函数的功能是设置投影变换
void CCamera::Look()
{
	// 根据摄像机的位置、方向和上下方向设置投影变换
	gluLookAt(m_vPosition.x, m_vPosition.y, m_vPosition.z,	
			  m_vView.x,	 m_vView.y,     m_vView.z,	
			  m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);
}

