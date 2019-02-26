#include "BWElement.h"
#include "BWPrimitive.h"
#include "RTMath.h"
BWEle3DD BWEle3DD::ZERO;
BWEle3DD BWEle3DD::UNIT_SCALE(1.0, 1.0, 1.0);
BWEle3DD BWEle3DD::NEGATIVE_UNIT_Z(0.0, 0.0,-1.0);
BWEle3DD BWEle3DD::UNIT_X(1.0, 0.0, 0.0);
BWEle3DD BWEle3DD::UNIT_Y(0.0, 1.0, 0.0);
BWEle3DD BWEle3DD::UNIT_Z(0.0, 0.0, 1.0);
const BWQuaternion BWQuaternion::ZERO(0.0,0.0, 0.0,0.0);
const BWQuaternion BWQuaternion::IDENTITY(1.0, 0.0, 0.0, 0.0);
BWEle3DD::BWEle3DD(const BWEle4DD& ele4)
{

	x = ele4.x;
	y = ele4.y;
	z = ele4.z;
}

const BWEle3DD BWEle3DD::operator/(float k) const
{
	return BWEle3DD(x / k, y / k, z / k);
}
const BWEle3DD BWEle3DD::operator/(float k)
{
	return BWEle3DD(x / k, y / k, z / k);
}
const BWEle3DD BWEle3DD::operator/(const BWEle3DD & vector) const
{
	return BWEle3DD(x / vector.x, y / vector.y, z / vector.z);
}
const BWEle3DD BWEle3DD::operator*(float k) const
{
	return BWEle3DD(x * k, y * k, z * k);
}
const BWEle3DD BWEle3DD::operator*(float k)
{
	return BWEle3DD(x * k, y * k, z * k);
}
const BWEle3DD BWEle3DD::operator*(const BWEle3DD& vector) const
{
	return BWEle3DD(x*vector.x, y*vector.y, z*vector.z);
}
const BWEle3DD BWEle3DD::operator*(BWEle3DD &vector) const
{
	return BWEle3DD(x*vector.x, y *vector.y, z*vector.z);
}

const BWEle3DD BWEle3DD::operator+(const BWEle3DD & ele)
{
	return BWEle3DD(x + ele.x, y + ele.y, z + ele.z);
}

const BWEle3DD BWEle3DD::operator-(const BWEle3DD & ele)
{
	return BWEle3DD(x - ele.x, y - ele.y, z - ele.z);
}
const BWEle3DD BWEle3DD::operator+(const BWEle3DD & ele) const
{
	return BWEle3DD(x + ele.x, y + ele.y, z + ele.z);
}

const BWEle3DD BWEle3DD::operator-(const BWEle3DD & ele) const
{
	return BWEle3DD(x - ele.x, y - ele.y, z - ele.z);
}
const BWEle3DD& BWEle3DD::operator+=(const BWEle3DD &ele)
{
	x += ele.x;
	y += ele.y;
	z += ele.z;
	return *this;
}


void BWQuaternion::fromAngleAxis(const Radian& rfAngle, const BWEle3DD& rkAxis)
{
	// assert:  axis[] is unit length
	//
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

	Radian fHalfAngle(0.5*rfAngle.mRad);
	float fSin = sin(fHalfAngle.mRad);
	w = cos(fHalfAngle.mRad);
	x = fSin*rkAxis.x;
	y = fSin*rkAxis.y;
	z = fSin*rkAxis.z;
}

float BWQuaternion::normalize()
{
	float fNorm = Norm();
	float factor = 1.0f / sqrt(fNorm);
	*this = (*this) * factor;
	return fNorm;
}
void BWQuaternion::ToRotationMatrix(BWMatrix4& matrix) const
{
	float fTx = x + x;
	float fTy = y + y;
	float fTz = z + z;
	float fTwx = fTx*w;
	float fTwy = fTy*w;
	float fTwz = fTz*w;
	float fTxx = fTx*x;
	float fTxy = fTy*x;
	float fTxz = fTz*x;
	float fTyy = fTy*y;
	float fTyz = fTz*y;
	float fTzz = fTz*z;

	matrix[0][0] = 1.0f - (fTyy + fTzz);
	matrix[0][1] = fTxy - fTwz;
	matrix[0][2] = fTxz + fTwy;
	matrix[1][0] = fTxy + fTwz;
	matrix[1][1] = 1.0f - (fTxx + fTzz);
	matrix[1][2] = fTyz - fTwx;
	matrix[2][0] = fTxz - fTwy;
	matrix[2][1] = fTyz + fTwx;
	matrix[2][2] = 1.0f - (fTxx + fTyy);
}
void BWQuaternion::ToAxes(BWEle3DD* axes)
{
	BWMatrix4 tmp;
	ToRotationMatrix(tmp);
	for (int i = 0; i < 3; i++)
	{
		axes[i].x = tmp[0][i];
		axes[i].y = tmp[1][i];
		axes[i].z = tmp[2][i];
	}
}
void BWQuaternion::fromRotationMatrix(const BWMatrix4& kRot)
{

	// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	// article "Quaternion Calculus and Fast Animation".

	float fTrace = kRot[0][0] + kRot[1][1] + kRot[2][2];
	float fRoot;

	if (fTrace > 0.0)
	{
		// |w| > 1/2, may as well choose w > 1/2
		fRoot = sqrt(fTrace + 1.0f);  // 2w
		w = 0.5f*fRoot;
		fRoot = 0.5f / fRoot;  // 1/(4w)
		x = (kRot[2][1] - kRot[1][2])*fRoot;
		y = (kRot[0][2] - kRot[2][0])*fRoot;
		z = (kRot[1][0] - kRot[0][1])*fRoot;
	}
	else
	{
		// |w| <= 1/2
		static size_t s_iNext[3] = { 1, 2, 0 };
		size_t i = 0;
		if (kRot[1][1] > kRot[0][0])
			i = 1;
		if (kRot[2][2] > kRot[i][i])
			i = 2;
		size_t j = s_iNext[i];
		size_t k = s_iNext[j];

		fRoot = sqrt(kRot[i][i] - kRot[j][j] - kRot[k][k] + 1.0f);
		float* apkQuat[3] = { &x, &y, &z };
		*apkQuat[i] = 0.5f*fRoot;
		fRoot = 0.5f / fRoot;
		w = (kRot[k][j] - kRot[j][k])*fRoot;
		*apkQuat[j] = (kRot[j][i] + kRot[i][j])*fRoot;
		*apkQuat[k] = (kRot[k][i] + kRot[i][k])*fRoot;
	}
}
void BWQuaternion::fromAxes(const BWVector3D& x, const BWVector3D& y, const BWVector3D& z)
{
	BWVector3D axes[3];
	axes[0] = x; 
	axes[1] = y;
	axes[2] = z;
	fromAxes(axes);
}
void BWQuaternion::fromAxes(const BWVector3D* akAxis)
{
	BWMatrix4 kRot;

	for (size_t iCol = 0; iCol < 3; iCol++)
	{
		kRot[0][iCol] = akAxis[iCol].x;
		kRot[1][iCol] = akAxis[iCol].y;
		kRot[2][iCol] = akAxis[iCol].z;
	}
	fromRotationMatrix(kRot);
}

float BWEle3DD::lenth() const
{
	return sqrt(x*x + y*y + z*z);
}

BWQuaternion BWEle3DD::getRotationTo(const BWEle3DD& dest, const BWVector3D& fallbackAxis /* = BWVector3D::ZERO */)
{
	// Based on Stan Melax's article in Game Programming Gems
	BWQuaternion q;
	// Copy, since cannot modify local
	BWVector3D v0 = *this;
	BWVector3D v1 = dest;
	v1.normalize();
	v0.normalize();
	float d = v0.dotProduct(v1);
	// If dot == 1, vectors are the same
	if (d >= 1.0f)
	{
		return BWQuaternion::IDENTITY;
	}
	if (d < (1e-6f - 1.0f))
	{
		if (fallbackAxis != BWEle3DD::ZERO)
		{
			// rotate 180 degrees about the fallback axis
			q.fromAngleAxis(Radian(PI), fallbackAxis);
		}
		else
		{
			// Generate an axis
			BWVector3 axis = BWEle3DD::UNIT_X.crossProduct(*this);
			if (axis.isZeroLength()) // pick another if colinear
				axis = BWEle3DD::UNIT_Y.crossProduct(*this);
			axis.normalize();
			q.fromAngleAxis(Radian(PI), axis);
		}
	}
	else
	{
		float s = sqrt((1 + d) * 2);
		float invs = 1 / s;

		BWVector3 c = v0.crossProduct(v1);

		q.x = c.x * invs;
		q.y = c.y * invs;
		q.z = c.z * invs;
		q.w = s * 0.5f;
		q.normalize();
	}
	return q;
}