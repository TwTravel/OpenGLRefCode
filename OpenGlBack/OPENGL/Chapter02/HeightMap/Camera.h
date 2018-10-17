#ifndef _CAMERA_H
#define _CAMERA_H


// 摄像机类
class CCamera {

public:

	// 摄像机类的构造函数
	CCamera();	

	// 下面的函数是获取有关摄像机的数据
	CVector3 Position() {	return m_vPosition;		}
	CVector3 View()		{	return m_vView;			}
	CVector3 UpVector() {	return m_vUpVector;		}
	CVector3 Strafe()	{	return m_vStrafe;		}
	
	//  摄像机位置
	void PositionCamera(float positionX, float positionY, float positionZ,
			 		    float viewX,     float viewY,     float viewZ,
						float upVectorX, float upVectorY, float upVectorZ);

	// 旋转摄像机
	void RotateView(float angle, float X, float Y, float Z);

	// 移动视点
	void SetViewByMouse(); 

	// 绕一点旋转摄像机
	void RotateAroundPoint(CVector3 vCenter, float X, float Y, float Z);
	//  左右移动摄像机
	void StrafeCamera(float speed);
	//  移动摄像机
	void MoveCamera(float speed);
	// 键盘事件
	void CheckForMovement();
	void Update();
	void Look();

private:

	// 摄像机的位置
	CVector3 m_vPosition;					
	// 摄像机的视野
	CVector3 m_vView;						
	// 摄像机的向上的位置
	CVector3 m_vUpVector;		
	//  摄像机左右方向	
	CVector3 m_vStrafe;						
};

#endif

