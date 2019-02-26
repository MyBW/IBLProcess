#pragma once
#include <memory.h>
#include <assert.h>
#include "BWElement.h"



typedef BWEle2DD BWPoint2DD;
typedef BWPoint2DD * BWPPoint2DD;

typedef BWEle3B BWColor3B;
typedef BWColor3B* BWPColor3B;

typedef BWEle3DD BWPoint3DD;
typedef BWPoint3DD *BWPPoint3DD;

typedef BWEle3DD BWVector3DD;
typedef BWVector3DD* BWPVector3DD;
typedef BWEle3DD BWVector3D; 

typedef BWEle3DD BWColor3DD;
typedef BWColor3DD* BWPColor3DD;
typedef BWEle3DD BWVector3;

typedef BWEle4DD BWPoint4DD;
typedef BWPoint4DD *BWPPoint4DD;

typedef BWEle4DD BWVector4DD;
typedef BWVector4DD* BWPVector4DD;

typedef BWEle4DD BWVector4;

// 本程序中使用的矩阵都是是 a = A*b；的形式,如果传入OpenGL或者GLSL 必须转置矩阵
// [m[0][0]  m[0][1]  m[0][2]  m[0][3]]    {x}
// |m[1][0]  m[1][1]  m[1][2]  m[1][3]| *  {y}
// |m[2][0]  m[2][1]  m[2][2]  m[2][3]|    {z}
// [m[3][0]  m[3][1]  m[3][2]  m[3][3]]    {1}
struct BWMatrix4
{
	union
	{
		float M[16];
		struct
		{
			float M00, M01, M02, M03;
			float M10, M11, M12, M13;
			float M20, M21, M22, M23;
			float M30, M31, M32, M33;
		};
		float _M[4][4];
	};
	static BWMatrix4 IDENTITY;
	static BWMatrix4 CLIPSPACE2DTOIMAGESPACE;
	BWMatrix4()
	{
		memset(M, 0, sizeof(BWMatrix4));
	}
	BWMatrix4(BWMatrix4& mat)
	{
		memcpy(M, &mat, sizeof(BWMatrix4));
	}
	BWMatrix4(const BWMatrix4 &mat) 
	{
		memcpy(M, &mat, sizeof(BWMatrix4));
	}
	BWMatrix4(const float* mat)
	{
		memcpy(M, mat, sizeof(BWMatrix4));
	}
	BWMatrix4& operator=(const BWMatrix4& const mat)
	{
		if (this != &mat)
		{
			memcpy(M, &mat, sizeof(BWMatrix4));
		}

		return *this;
	}
	const BWMatrix4 operator*(const BWMatrix4& mat) const 
	{
		return concatenate(mat);
	}
	const BWEle3DD operator*(const BWEle3DD &vector) const
	{
		BWEle3DD r;
		float invw = 1.0f / (M30*vector.x + M31*vector.y + M32*vector.z + M33);
		r.x = (M00 * vector.x + M01 * vector.y + M02 * vector.z + M03) * invw;
		r.y = (M10 * vector.x + M11 * vector.y + M12 * vector.z + M13) * invw;
		r.z = (M20 * vector.x + M21 * vector.y + M22 * vector.z + M23) * invw;
		return r;
	}
	BWMatrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		_M[0][0] = m00;
		_M[0][1] = m01;
		_M[0][2] = m02;
		_M[0][3] = m03;
		_M[1][0] = m10;
		_M[1][1] = m11;
		_M[1][2] = m12;
		_M[1][3] = m13;
		_M[2][0] = m20;
		_M[2][1] = m21;
		_M[2][2] = m22;
		_M[2][3] = m23;
		_M[3][0] = m30;
		_M[3][1] = m31;
		_M[3][2] = m32;
		_M[3][3] = m33;
	}
	
	inline	BWMatrix4& operator=(const float* mat)
	{
		memcpy(M, mat, sizeof(BWMatrix4));
		return *this;
	}
	inline  const float* const operator[](int n1) const //operator[][]
	{
		return (M + n1 * 4);
	}
	inline float* operator[](int n1)
	{
		return M + n1 * 4;
	}
	void SetZero()
	{
		memset(M, 0, sizeof(BWMatrix4));
	}
	void setTrans(const BWVector3& trans)
	{
		M03 = trans.x;
		M13 = trans.y;
		M23 = trans.z;
	}
	BWVector3 getTrans()
	{
		return BWVector3(M03, M13, M23);
	}
	//矩阵乘法
	const BWMatrix4 concatenate(const BWMatrix4  &m2) const
	{
		BWMatrix4 r;
		r._M[0][0] = _M[0][0] * m2._M[0][0] + _M[0][1] * m2._M[1][0] + _M[0][2] * m2._M[2][0] + _M[0][3] * m2._M[3][0];
		r._M[0][1] = _M[0][0] * m2._M[0][1] + _M[0][1] * m2._M[1][1] + _M[0][2] * m2._M[2][1] + _M[0][3] * m2._M[3][1];
		r._M[0][2] = _M[0][0] * m2._M[0][2] + _M[0][1] * m2._M[1][2] + _M[0][2] * m2._M[2][2] + _M[0][3] * m2._M[3][2];
		r._M[0][3] = _M[0][0] * m2._M[0][3] + _M[0][1] * m2._M[1][3] + _M[0][2] * m2._M[2][3] + _M[0][3] * m2._M[3][3];
		
		r._M[1][0] = _M[1][0] * m2._M[0][0] + _M[1][1] * m2._M[1][0] + _M[1][2] * m2._M[2][0] + _M[1][3] * m2._M[3][0];
		r._M[1][1] = _M[1][0] * m2._M[0][1] + _M[1][1] * m2._M[1][1] + _M[1][2] * m2._M[2][1] + _M[1][3] * m2._M[3][1];
		r._M[1][2] = _M[1][0] * m2._M[0][2] + _M[1][1] * m2._M[1][2] + _M[1][2] * m2._M[2][2] + _M[1][3] * m2._M[3][2];
		r._M[1][3] = _M[1][0] * m2._M[0][3] + _M[1][1] * m2._M[1][3] + _M[1][2] * m2._M[2][3] + _M[1][3] * m2._M[3][3];
		
		r._M[2][0] = _M[2][0] * m2._M[0][0] + _M[2][1] * m2._M[1][0] + _M[2][2] * m2._M[2][0] + _M[2][3] * m2._M[3][0];
		r._M[2][1] = _M[2][0] * m2._M[0][1] + _M[2][1] * m2._M[1][1] + _M[2][2] * m2._M[2][1] + _M[2][3] * m2._M[3][1];
		r._M[2][2] = _M[2][0] * m2._M[0][2] + _M[2][1] * m2._M[1][2] + _M[2][2] * m2._M[2][2] + _M[2][3] * m2._M[3][2];
		r._M[2][3] = _M[2][0] * m2._M[0][3] + _M[2][1] * m2._M[1][3] + _M[2][2] * m2._M[2][3] + _M[2][3] * m2._M[3][3];

		r._M[3][0] = _M[3][0] * m2._M[0][0] + _M[3][1] * m2._M[1][0] + _M[3][2] * m2._M[2][0] + _M[3][3] * m2._M[3][0];
		r._M[3][1] = _M[3][0] * m2._M[0][1] + _M[3][1] * m2._M[1][1] + _M[3][2] * m2._M[2][1] + _M[3][3] * m2._M[3][1];
		r._M[3][2] = _M[3][0] * m2._M[0][2] + _M[3][1] * m2._M[1][2] + _M[3][2] * m2._M[2][2] + _M[3][3] * m2._M[3][2];
		r._M[3][3] = _M[3][0] * m2._M[0][3] + _M[3][1] * m2._M[1][3] + _M[3][2] * m2._M[2][3] + _M[3][3] * m2._M[3][3];

		return r;
	}
	//仿射变换乘法
	const BWMatrix4 concatenateAffine(const BWMatrix4 & m2) const
	{
		assert(isAffine() && m2.isAffine());

		return BWMatrix4(
			_M[0][0] * m2._M[0][0] + _M[0][1] * m2._M[1][0] + _M[0][2] * m2._M[2][0],
			_M[0][0] * m2._M[0][1] + _M[0][1] * m2._M[1][1] + _M[0][2] * m2._M[2][1],
			_M[0][0] * m2._M[0][2] + _M[0][1] * m2._M[1][2] + _M[0][2] * m2._M[2][2],
			_M[0][0] * m2._M[0][3] + _M[0][1] * m2._M[1][3] + _M[0][2] * m2._M[2][3] + _M[0][3],
			
			_M[1][0] * m2._M[0][0] + _M[1][1] * m2._M[1][0] + _M[1][2] * m2._M[2][0],  
			_M[1][0] * m2._M[0][1] + _M[1][1] * m2._M[1][1] + _M[1][2] * m2._M[2][1],  
			_M[1][0] * m2._M[0][2] + _M[1][1] * m2._M[1][2] + _M[1][2] * m2._M[2][2],  
			_M[1][0] * m2._M[0][3] + _M[1][1] * m2._M[1][3] + _M[1][2] * m2._M[2][3] + _M[1][3],

			_M[2][0] * m2._M[0][0] + _M[2][1] * m2._M[1][0] + _M[2][2] * m2._M[2][0],  
			_M[2][0] * m2._M[0][1] + _M[2][1] * m2._M[1][1] + _M[2][2] * m2._M[2][1],  
			_M[2][0] * m2._M[0][2] + _M[2][1] * m2._M[1][2] + _M[2][2] * m2._M[2][2],  
			_M[2][0] * m2._M[0][3] + _M[2][1] * m2._M[1][3] + _M[2][2] * m2._M[2][3] + _M[2][3],

			0, 0, 0, 1);
	}
	const BWMatrix4 inverseAffine() const
	{
		assert(isAffine());

		float m10 = _M[1][0], m11 = _M[1][1], m12 = _M[1][2];
		float m20 = _M[2][0], m21 = _M[2][1], m22 = _M[2][2];

		float t00 = m22 * m11 - m21 * m12;
		float t10 = m20 * m12 - m22 * m10;
		float t20 = m21 * m10 - m20 * m11;

		float m00 = _M[0][0], m01 = _M[0][1], m02 = _M[0][2];

		float invDet = 1 / (m00 * t00 + m01 * t10 + m02 * t20);

		t00 *= invDet; t10 *= invDet; t20 *= invDet;

		m00 *= invDet; m01 *= invDet; m02 *= invDet;

		float r00 = t00;
		float r01 = m02 * m21 - m01 * m22;
		float r02 = m01 * m12 - m02 * m11;

		float r10 = t10;
		float r11 = m00 * m22 - m02 * m20;
		float r12 = m02 * m10 - m00 * m12;

		float r20 = t20;
		float r21 = m01 * m20 - m00 * m21;
		float r22 = m00 * m11 - m01 * m10;

		float m03 = _M[0][3], m13 = _M[1][3], m23 = _M[2][3];

		float r03 = -(r00 * m03 + r01 * m13 + r02 * m23);
		float r13 = -(r10 * m03 + r11 * m13 + r12 * m23);
		float r23 = -(r20 * m03 + r21 * m13 + r22 * m23);

		return BWMatrix4(
			r00, r01, r02, r03,
			r10, r11, r12, r13,
			r20, r21, r22, r23,
			0, 0, 0, 1);
	}
	// 转置矩阵
	const BWMatrix4 transpose() const
	{
		return BWMatrix4(
			M00, M10, M20, M30,
			M01, M11, M21, M31,
			M02, M12, M22, M32,
			M03, M13, M23, M33);
	}
	//变换向量
	const BWVector3 transformAffine(BWVector3 vector) const
	{
		assert(isAffine()); 
		return BWVector3(
			M00 * vector.x + M01 * vector.y + M02 * vector.z ,
			M10 * vector.x + M11 * vector.y + M12 * vector.z ,
			M20 * vector.x + M21 * vector.y + M22 * vector.z);
	}
	//const BWMatrix4 inverse() const
	//{
	//	float Coef00 = _M[2][2] * _M[3][3] - _M[3][2] * _M[2][3];
	//	float Coef02 = _M[1][2] * _M[3][3] - _M[3][2] * _M[1][3];
	//	float Coef03 = _M[1][2] * _M[2][3] - _M[2][2] * _M[1][3];

	//	float Coef04 = _M[2][1] * _M[3][3] - _M[3][1] * _M[2][3];
	//	float Coef06 = _M[1][1] * _M[3][3] - _M[3][1] * _M[1][3];
	//	float Coef07 = _M[1][1] * _M[2][3] - _M[2][1] * _M[1][3];

	//	float Coef08 = _M[2][1] * _M[3][2] - _M[3][1] * _M[2][2];
	//	float Coef10 = _M[1][1] * _M[3][2] - _M[3][1] * _M[1][2];
	//	float Coef11 = _M[1][1] * _M[2][2] - _M[2][1] * _M[1][2];

	//	float Coef12 = _M[2][0] * _M[3][3] - _M[3][0] * _M[2][3];
	//	float Coef14 = _M[1][0] * _M[3][3] - _M[3][0] * _M[1][3];
	//	float Coef15 = _M[1][0] * _M[2][3] - _M[2][0] * _M[1][3];

	//	float Coef16 = _M[2][0] * _M[3][2] - _M[3][0] * _M[2][2];
	//	float Coef18 = _M[1][0] * _M[3][2] - _M[3][0] * _M[1][2];
	//	float Coef19 = _M[1][0] * _M[2][2] - _M[2][0] * _M[1][2];

	//	float Coef20 = _M[2][0] * _M[3][1] - _M[3][0] * _M[2][1];
	//	float Coef22 = _M[1][0] * _M[3][1] - _M[3][0] * _M[1][1];
	//	float Coef23 = _M[1][0] * _M[2][1] - _M[2][0] * _M[1][1];

	//	const BWVector4 SignA = BWVector4(1.0, -1.0, 1.0, -1.0);
	//	const BWVector4 SignB = BWVector4(-1.0, 1.0, -1.0, 1.0);

	//	BWVector4 Fac0 = BWVector4(Coef00, Coef00, Coef02, Coef03);
	//	BWVector4 Fac1 = BWVector4(Coef04, Coef04, Coef06, Coef07);
	//	BWVector4 Fac2 = BWVector4(Coef08, Coef08, Coef10, Coef11);
	//	BWVector4 Fac3 = BWVector4(Coef12, Coef12, Coef14, Coef15);
	//	BWVector4 Fac4 = BWVector4(Coef16, Coef16, Coef18, Coef19);
	//	BWVector4 Fac5 = BWVector4(Coef20, Coef20, Coef22, Coef23);
	//	
	//	BWVector4 Vec0 = BWVector4(_M[1][0], _M[0][0], _M[0][0], _M[0][0]);
	//	BWVector4 Vec1 = BWVector4(_M[1][1], _M[0][1], _M[0][1], _M[0][1]);
	//	BWVector4 Vec2 = BWVector4(_M[1][2], _M[0][2], _M[0][2], _M[0][2]);
	//	BWVector4 Vec3 = BWVector4(_M[1][3], _M[0][3], _M[0][3], _M[0][3]);
	//	
	//	BWVector4 Inv0 = SignA * (Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
	//	BWVector4 Inv1 = SignB * (Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
	//	BWVector4 Inv2 = SignA * (Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
	//	BWVector4 Inv3 = SignB * (Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

	//	BWVector4 Inverse = BWVector4(Inv0, Inv1, Inv2, Inv3);

	//	BWVector4 Row0 = BWVector4(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

	//	float Determinant = dot(_M[0], Row0);

	//	Inverse /= Determinant;

	//	return Inverse;
	//}
	//求逆矩阵
	const BWMatrix4 inverse() const
	{
		// 并非所有的矩阵都有逆矩阵。然而所有的仿射矩阵都有逆矩阵，可用高斯消去法或LU分解求之。
		float m00 = _M[0][0], m01 = _M[0][1], m02 = _M[0][2], m03 = _M[0][3];
		float m10 = _M[1][0], m11 = _M[1][1], m12 = _M[1][2], m13 = _M[1][3];
		float m20 = _M[2][0], m21 = _M[2][1], m22 = _M[2][2], m23 = _M[2][3];
		float m30 = _M[3][0], m31 = _M[3][1], m32 = _M[3][2], m33 = _M[3][3];

		float v0 = m20 * m31 - m21 * m30;
		float v1 = m20 * m32 - m22 * m30;
		float v2 = m20 * m33 - m23 * m30;
		float v3 = m21 * m32 - m22 * m31;
		float v4 = m21 * m33 - m23 * m31;
		float v5 = m22 * m33 - m23 * m32;

		float t00 = +(v5 * m11 - v4 * m12 + v3 * m13);
		float t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
		float t20 = +(v4 * m10 - v2 * m11 + v0 * m13);
		float t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

		float invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

		float d00 = t00 * invDet;
		float d10 = t10 * invDet;
		float d20 = t20 * invDet;
		float d30 = t30 * invDet;

		float d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		float d11 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		float d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		float d31 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		v0 = m10 * m31 - m11 * m30;
		v1 = m10 * m32 - m12 * m30;
		v2 = m10 * m33 - m13 * m30;
		v3 = m11 * m32 - m12 * m31;
		v4 = m11 * m33 - m13 * m31;
		v5 = m12 * m33 - m13 * m32;

		float d02 = +(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		float d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		float d22 = +(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		float d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		v0 = m21 * m10 - m20 * m11;
		v1 = m22 * m10 - m20 * m12;
		v2 = m23 * m10 - m20 * m13;
		v3 = m22 * m11 - m21 * m12;
		v4 = m23 * m11 - m21 * m13;
		v5 = m23 * m12 - m22 * m13;

		float d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		float d13 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		float d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		float d33 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		return BWMatrix4(
			d00, d01, d02, d03,
			d10, d11, d12, d13,
			d20, d21, d22, d23,
			d30, d31, d32, d33);
	}
	static  BWMatrix4 makeViewMatrix(const BWVector3D& position, const BWQuaternion& orientation, const BWMatrix4* reflectMatrix)
	{
		BWMatrix4 viewMatrix;
		// View matrix is:
		//
		//  [ Lx  Uy  Dz  Tx  ]
		//  [ Lx  Uy  Dz  Ty  ]
		//  [ Lx  Uy  Dz  Tz  ]
		//  [ 0   0   0   1   ]
		//
		// Where T = -(Transposed(Rot) * Pos)
		BWMatrix4 rot;
		orientation.ToRotationMatrix(rot);
		rot.M33 = 1.0;
		// Make the translation relative to new axes
		BWMatrix4 rotT = rot.transpose();
		BWVector3D trans = (rotT * position) * (-1);

		// Make final matrix
		viewMatrix = BWMatrix4::IDENTITY;
		viewMatrix = rotT; // fills upper 3x3
		viewMatrix[0][3] = trans.x;
		viewMatrix[1][3] = trans.y;
		viewMatrix[2][3] = trans.z;

		// Deal with reflections
		if (reflectMatrix)
		{
			viewMatrix = viewMatrix * (*reflectMatrix);
		}

		return viewMatrix;
	}
	bool isAffine() const
	{
		return M30 == 0 && M31 == 0 && M32 == 0 && M33 == 1;
	}
	/** Determines if this matrix involves a scaling. */
	inline bool hasScale() const
	{
		// check magnitude of column vectors (==local axes)
		float t = _M[0][0] * _M[0][0] + _M[1][0] * _M[1][0] + _M[2][0] * _M[2][0];
		if (!(fabs(t - 1.0) < ((float)1e-04)))
		{
			return true;
		}
		t = _M[0][1] * _M[0][1] + _M[1][1] * _M[1][1] + _M[2][1] * _M[2][1];
		if (!(fabs(t - 1.0) < ((float)1e-04)))
		{
			return true;
		}
		t = _M[0][2] * _M[0][2] + _M[1][2] * _M[1][2] + _M[2][2] * _M[2][2];
		if (!(fabs(t - 1.0) < ((float)1e-04)))
		{
			return true;
		}
		return false;
	}
	inline bool hasNegativeScale() const
	{
		return determinant() < 0;
	}

	inline static float
		MINOR(const BWMatrix4& m, const size_t r0, const size_t r1, const size_t r2,
		const size_t c0, const size_t c1, const size_t c2)
	{
			return 
				m[r0][c0] * (m[r1][c1] * m[r2][c2] - m[r2][c1] * m[r1][c2]) -
				m[r0][c1] * (m[r1][c0] * m[r2][c2] - m[r2][c0] * m[r1][c2]) +
				m[r0][c2] * (m[r1][c0] * m[r2][c1] - m[r2][c0] * m[r1][c1]);
	}
	// 求解行列式的值
	float determinant() const
	{
		return 
			_M[0][0] * MINOR(*this, 1, 2, 3, 1, 2, 3) -
			_M[0][1] * MINOR(*this, 1, 2, 3, 0, 2, 3) +
			_M[0][2] * MINOR(*this, 1, 2, 3, 0, 1, 3) -
			_M[0][3] * MINOR(*this, 1, 2, 3, 0, 1, 2);
	}
	void transposeSelf()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = i; j < 4; j++)
			{
				float a = _M[i][j];
				_M[i][j] = _M[j][i];
				_M[j][i] = a;
			}
		}
	}/**/
	void makeTransform(const BWVector3D& position, const BWVector3D &scale, const BWQuaternion& orientation)
	{
		// Ordering:
		//    1. Scale
		//    2. Rotate
		//    3. Translate
		BWMatrix4 matrix;
		orientation.ToRotationMatrix(matrix);
		
		_M[0][0] = scale.x * matrix[0][0]; _M[0][1] = scale.y * matrix[0][1]; _M[0][2] = scale.z * matrix[0][2]; _M[0][3] = position.x;
		_M[1][0] = scale.x * matrix[1][0]; _M[1][1] = scale.y * matrix[1][1]; _M[1][2] = scale.z * matrix[1][2]; _M[1][3] = position.y;
		_M[2][0] = scale.x * matrix[2][0]; _M[2][1] = scale.y * matrix[2][1]; _M[2][2] = scale.z * matrix[2][2]; _M[2][3] = position.z;

		// No projection term
		_M[3][0] = 0; _M[3][1] = 0; _M[3][2] = 0; _M[3][3] = 1;
	}
};
struct Radian
{
	float mRad;
	Radian(float rad) :mRad(rad){}
	Radian() :mRad(0){}
	bool operator!=(const Radian& r) const
	{
		return r.mRad != mRad;
	}
	bool operator!=(const Radian& r) 
	{
		return r.mRad != mRad;
	}
	float valueRadians() const
	{
		return mRad;
	}
};

struct  BWRay
{
	BWRay();
	~BWRay();
	BWRay operator-()
	{
		BWRay Result;
		Result._start = _start;
		Result._vector = _vector * -1;
		return Result;
	}
	BWPoint4DD _start;
	BWPoint3DD _vector;
};

class  BWTriangle
{
public:
	BWTriangle();
	~BWTriangle();
	void GetFacePoints(BWPoint4DD & p1, BWPoint4DD &p2, BWPoint4DD&p3);
	void GetFacePoints(BWPoint4DD & p1, BWPoint4DD &p2, BWPoint4DD&p3) const;
private:
	BWPoint4DD _vertex[3];
	BWPoint3DD _normal;
	BWPoint4DD _centroid;
};

class BWBox
{
public:
	size_t left, right, top, bottom, front, back;
	BWBox() :left(0), right(1), top(0), bottom(1), front(0), back(1)
	{

	}
	BWBox(size_t l, size_t t, size_t ff, size_t r, size_t b, size_t bb)
		:left(l), right(r), top(t), bottom(b), front(ff), back(bb)
	{

	}
	BWBox(size_t l, size_t t, size_t r, size_t b) :left(l), right(r), top(t), bottom(b), front(0), back(1)
	{
		//assert(right >= left && bottom >= top && back >= front);
	}
	bool contains(const BWBox& box) const
	{
		return (left <= box.left && right >= box.right && top >= box.top
			&&  bottom <= box.bottom && front >= box.front && back <= box.back
			);
	}
	size_t getWidth() const { return right - left; }
	size_t getHeight() const  { return top - bottom; }
	size_t getDepth() const { return front - back; }
protected:
private:
};
//class BWPixelBox : public BWBox
//{
//public:
//	BWPixelBox(size_t width, size_t height, size_t depth, PixelFormat pixelFormat, void *pixelData = 0) :
//		BWBox(0, height, 0, width, 0, depth)
//		, mPixelFormat(pixelFormat), mData(pixelData)
//	{
//		setConsecutive();
//	}
//	BWPixelBox() :BWBox(0, 0, 0, 0, 0, 0), mPixelFormat(PixelFormat::PF_UNKNOWN), mData(NULL)
//	{
//
//	}
//	~BWPixelBox()
//	{
//	}
//	
//	PixelFormat mPixelFormat;
//	void *mData;  // pixelbox  的主要作用是标记位置 并不保存数据 所以mData最后也不用销毁
//	size_t mRowPitch;
//	size_t mSlicePitch;
//	void setConsecutive()
//	{
//		mRowPitch = getWidth();
//		mSlicePitch = getWidth() * getHeight();
//	}
//	size_t getRowSkip() const  { return mRowPitch - getWidth(); }
//	size_t getSliceSkip() const { return mSlicePitch - getHeight(); }
//	bool isConsecutive() const 
//	{
//		return ( mRowPitch == getWidth()) && (mSlicePitch == getWidth() * getHeight());
//	}
//	size_t getConsecutiveSize() const 
//	{
//		return PixelUtil::getMemorySize(getWidth(), getHeight(), getDepth(), mPixelFormat);
//	}
//	BWPixelBox getSubVolume(const BWBox &def) const
//	{
//		if (PixelUtil::isCompressed(mPixelFormat))
//		{
//			if (def.left == left && def.right == right && def.top == top && def.bottom == bottom && def.front == front  && def.back == back)
//			{
//				return *this;
//			}
//			assert(0); //压缩格式的只可以整块的拷贝
//		}
//		if (!contains(def))
//		{
//			assert(0);
//		}
//		const size_t eleSize = PixelUtil::getNumElemBytes(mPixelFormat);
//		BWPixelBox rval(def.getWidth(), def.getHeight(), def.getDepth(), mPixelFormat,
//			((unsigned char *)mData) + (def.left - left) * eleSize + (top - def.top) * mRowPitch * eleSize + (front - def.front) *mSlicePitch * mRowPitch * eleSize
//			);
//		rval.mRowPitch = mRowPitch;
//		rval.mSlicePitch = mSlicePitch;
//		rval.mPixelFormat = mPixelFormat;
//		return rval;
//	}
//};
class BWBOX
{
public:
	friend bool CollisionDetecte(const BWBOX&, const BWBOX &);
	BWBOX();
	BWBOX(const BWBOX& BWBOX);
	~BWBOX();
	void SetCenter(const BWPoint4DD & center);
	const BWPoint4DD & GetCenter() const;
	const BWPoint4DD & GetCenter();
	void SetAxi(BWPoint3DD& axi1, BWPoint3DD& axi2, BWPoint3DD& axi3);
	void GetAxi(BWPoint3DD& axi1, BWPoint3DD& axi2, BWPoint3DD& axi3) const;
	void GetAxi(BWPoint3DD& axi1, BWPoint3DD& axi2, BWPoint3DD& axi3) ;
	const BWPoint3DD* GetAxi() const;
    BWPoint3DD* GetAxi();
	void SetHalfLenth(float x, float y, float z);
	void GetHalfLenth(float & x, float & y, float &z) const;
	void GetHalfLenth(float & x, float & y, float &z) ;
	void GetHalfLenth(double& x, double & y, double &z);
	void Draw();
	const BWBOX& operator=(const BWBOX& BWBOX);

	BWPoint4DD _center;
	BWPoint3DD _axi[3];
	BWPoint3DD _halfLenth;
};

struct Degree
{
	float mDeg;
	explicit Degree(float d) :mDeg(d){}
};
