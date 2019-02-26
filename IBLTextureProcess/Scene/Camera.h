#pragma once
#include "..\math\bwprimitive.h"
class Camera
{
public:
	void Init(int Width, int Height);
	void InitProjectMatrix();

	BWVector3D GetViewportPositionInCameraSpace(float X, float Y);
	BWVector3D GetViewportPositionInWorldSpace(float X, float Y) ;
	BWVector3D GetDirection() const;
	BWVector3D GetUp() const;
	BWVector3D GetRight() const;
	void Move(BWVector3D &Dir);
	void RotateCamera(const BWVector3D& Axis, const Radian& Angle);
	void RotateCamera(const BWQuaternion &Q);
	void SetFixedYawAxis(bool IsFixed, const BWVector3D& fiexAxis = BWVector3D::UNIT_Y);
	void Yaw(const Radian& Angle);
	void Pitch(const Radian& Angle);
	void UpdateViewMatrix();
	void UpateProjectMatrix();
	void CalcProjectionParameters(float &left, float &right, float &bottom, float &top);
	float GetScreenWidth() const { return ScreenWidth; }
	float GetScreenHeight() const { return ScreenHeight; }
	BWMatrix4& GetViewMatrix();
	BWMatrix4& GetProjectMatrix();
	BWVector3D& GetPosition();
private:
	BWMatrix4 ProjectMatrix;
	BWMatrix4 ViewMatrix;
	bool IsYawFixed;
	BWVector3D YawFixedAxis;
	BWQuaternion CameraOrientation;
	BWVector3D CameraPosition;

	float FarDist;
	float NearDist;
	float ScreenWidth;
	float ScreenHeight;
	float INFINITE_FAR_PLANE_ADJUST = 0.00001;
	Radian mFOVy;
	float mAspect; // ¿í¸ß±È
	float mOrhtoHeight;
	BWVector3D mFrustumOffset;

};
