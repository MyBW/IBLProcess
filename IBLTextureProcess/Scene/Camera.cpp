#include "Camera.h"

void Camera::SetFixedYawAxis(bool InIsFixed, const BWVector3D& InFiexAxis /*= BWVector3D::UNIT_Y*/)
{
	IsYawFixed = InIsFixed;
	YawFixedAxis = InFiexAxis;
}
void Camera::Yaw(const Radian& Angle)
{
	BWVector3 yAxis;
	if (IsYawFixed) // 如果想使用第一人称摄像机 那么就要是固定的Y轴 Y轴朝向为单位Y轴
	{
		// Rotate around fixed yaw axis
		yAxis = YawFixedAxis;
	}
	else
	{
		// Rotate around local Y axis
		yAxis = CameraOrientation * BWVector3::UNIT_Y;
	}

	RotateCamera(yAxis, Angle);
}
void Camera::Pitch(const Radian& Angle)
{
	// Rotate around local X axis
	BWVector3 xAxis = CameraOrientation * BWVector3::UNIT_X;
	RotateCamera(xAxis, Angle);
}
void Camera::RotateCamera(const BWVector3D& Axis, const Radian& Angle)
{
	BWQuaternion Q;
	Q.fromAngleAxis(Angle, Axis);
	RotateCamera(Q);
}
void Camera::RotateCamera(const BWQuaternion &Q)
{
	BWQuaternion Qnorm = Q;
	Qnorm.normalize();
	CameraOrientation = Qnorm * CameraOrientation;
	UpdateViewMatrix();
}
void Camera::UpdateViewMatrix()
{
	ViewMatrix = BWMatrix4::makeViewMatrix(CameraPosition, CameraOrientation, 0);
}
BWMatrix4& Camera::GetViewMatrix()
{
	return ViewMatrix;
}
BWMatrix4& Camera::GetProjectMatrix()
{
	return ProjectMatrix;
}

BWVector3D& Camera::GetPosition()
{
	return CameraPosition;
}

void Camera::Init(int Width, int Height)
{
	ViewMatrix = BWMatrix4::IDENTITY;
	ScreenWidth = Width;
	ScreenHeight = Height;
	InitProjectMatrix();
}

void Camera::InitProjectMatrix()
{
	mFOVy.mRad = (3.1415 / 2);
	mAspect = ScreenWidth / ScreenHeight;
	NearDist = 1.0;
	FarDist = 1000;
	UpateProjectMatrix();
}

BWVector3D Camera::GetViewportPositionInCameraSpace(float X, float Y)
{
	float Left, Right, Bottom, Top;
	CalcProjectionParameters(Left, Right, Bottom, Top);
	return BWVector3D((Right - Left) * X + Left , (Top - Bottom) * Y + Bottom, -NearDist);
}

BWVector3D Camera::GetViewportPositionInWorldSpace(float X, float Y)
{
	float Left, Right, Bottom, Top;
	CalcProjectionParameters(Left, Right, Bottom, Top);
	return GetViewMatrix().inverse() * BWVector3D((Right - Left) * X + Left, (Top - Bottom) * Y + Bottom, -NearDist);
}

void Camera::UpateProjectMatrix()
{
	float Left, Right, Bottom, Top;
	CalcProjectionParameters(Left, Right, Bottom, Top);

	float inv_w = 1.0 / (Right - Left);
	float inv_h = 1.0 / (Top - Bottom);
	float inv_d = 1.0 / (FarDist - NearDist);

	float A = 2 * NearDist * inv_w;
	float B = 2 * NearDist * inv_h;
	float C = (Right + Left) * inv_w;
	float D = (Top + Bottom) * inv_h;
	float q, qn;
	if (FarDist == 0)
	{
		q = INFINITE_FAR_PLANE_ADJUST - 1;
		qn = NearDist *(INFINITE_FAR_PLANE_ADJUST - 2);
	}
	else
	{
		q = -(FarDist + NearDist) * inv_d;
		qn = -2 * (FarDist * NearDist) * inv_d;
	}
	// NB: This creates 'uniform' perspective projection matrix,
	// which depth range [-1,1], right-handed rules
	//
	// [ A   0   C   0  ]
	// [ 0   B   D   0  ]
	// [ 0   0   q   qn ]
	// [ 0   0   -1  0  ]
	//
	// A = 2 * near / (right - left)
	// B = 2 * near / (top - bottom)
	// C = (right + left) / (right - left)
	// D = (top + bottom) / (top - bottom)
	// q = - (far + near) / (far - near)
	// qn = - 2 * (far * near) / (far - near)

	ProjectMatrix.SetZero();
	ProjectMatrix[0][0] = A;
	ProjectMatrix[0][2] = C;
	ProjectMatrix[1][1] = B;
	ProjectMatrix[1][2] = D;
	ProjectMatrix[2][2] = q;
	ProjectMatrix[2][3] = qn;
	ProjectMatrix[3][2] = -1;
}

void Camera::CalcProjectionParameters(float &left, float &right, float &bottom, float &top)
{
	Radian thetaY(mFOVy.mRad * 0.5f);
	float tanThetaY = tan(thetaY.mRad);
	float tanThetaX = tanThetaY * mAspect;
	float half_w = tanThetaX * NearDist;
	float half_h = tanThetaY * NearDist;
	left = -half_w;
	right = half_w;
	bottom = -half_h;
	top = half_h;
}

void Camera::Move(BWVector3D &Dir)
{
	CameraPosition += Dir;
	UpdateViewMatrix();
}
BWVector3D Camera::GetDirection() const
{
	return CameraOrientation * BWVector3D::NEGATIVE_UNIT_Z;
}
BWVector3D Camera::GetUp() const
{
	return CameraOrientation * BWVector3D::UNIT_Y;
}
BWVector3D Camera::GetRight() const
{
	return CameraOrientation * BWVector3D::UNIT_X;
}
