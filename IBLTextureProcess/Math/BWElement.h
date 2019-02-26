#pragma once
#include <math.h>

struct BWEle4DD ;
struct Radian;
class BWMatrix4;
class BWQuaternion;
struct BWEle2DD
{
	float x;
	float y;
	BWEle2DD() : x(0), y(0)
	{

	}
	BWEle2DD(float _x, float _y) :x(_x), y(_y)
	{

	}
	BWEle2DD(const BWEle2DD& ele)
	{
		if (this == & ele)
		{
			return;
		}
		x = ele.x;
		y = ele.y;
	}
	const BWEle2DD& operator=(const BWEle2DD& ele)
	{
		if (this == &ele)
		{
			return *this;
		}
		x = ele.x;
		y = ele.y;
		return *this;
	}
	const BWEle2DD operator-(const BWEle2DD& ele)
	{
		BWEle2DD tmp;
		tmp.x = x - ele.x;
		tmp.y = y - ele.y;
		return tmp;
	}
};

struct BWEle2I
{
	union
	{
		struct 
		{
			int x;
			int y;
		};
		int M[2];
	};
	BWEle2I() :x(0), y(0)
	{

	}
	BWEle2I(const BWEle2I & ele)
	{
		if (this == &ele)
		{
			return;
		}
		x = ele.x;
		y = ele.y;
	}
	const BWEle2I& operator=(const BWEle2I& ele)
	{
		if (this == &ele)
		{
			return *this;
		}
		x = ele.x;
		y = ele.y;
		return *this;
	}
}  ;

struct BWEle3DD
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		struct
		{
			float r;
			float g;
			float b;
		};
		float M[3];
	};
	BWEle3DD() :x(0.0), y(0.0), z(0.0)
	{

	}
	BWEle3DD(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{

	}
	BWEle3DD(const BWEle4DD& ele4);
	BWEle3DD(const BWEle3DD& ele)
	{
		if (this == & ele)
		{
			return;
		}
		x = ele.x;
		y = ele.y;
		z = ele.z;
		return;
	}
	const BWEle3DD& operator= (const BWEle3DD & ele)
	{
		if (this == & ele)
		{
			return *this;
		}
		x = ele.x;
		y = ele.y;
		z = ele.z;
		return *this;
	}
	bool operator != (const BWEle3DD& ele) const
	{
		if (ele.x == x && ele.y == y  && ele.z == z)
		{
			return false;
		}
		return true;
	}
	
	const BWEle3DD operator/(float k) const;
	const BWEle3DD operator/(float k) ;
	const BWEle3DD operator/(const BWEle3DD& vect) const;

	const BWEle3DD operator+(const BWEle3DD& ele);
	const BWEle3DD operator+(const BWEle3DD& ele) const;
	const BWEle3DD operator-(const BWEle3DD& ele);
	const BWEle3DD operator-(const BWEle3DD& ele) const;
	const BWEle3DD operator*(float k) const;
	const BWEle3DD operator*(float k) ;
	const BWEle3DD operator*(BWEle3DD& vector) const ;
	const BWEle3DD operator*(const BWEle3DD& vector) const;
	const BWEle3DD& operator+=(const BWEle3DD& ele);
	bool operator==(const BWEle3DD& vector) const
	{
		return !(*this != vector);
	}
	float dotProduct(const BWEle3DD& vector) const
	{
		 return M[0] * vector.x + M[1] * vector.y + M[2] * vector.z;
	}
	const float operator[](int k) const
	{
		return M[k];
	}
	float& operator[](int k)
	{
		return M[k];
	}
	void makeCeil(const BWEle3DD &vector)
	{
		if (vector.x > x) x = vector.x;
		if (vector.y > y) y = vector.y;
		if (vector.z > z) z = vector.z;

	}
	void makeFloor(const BWEle3DD &vector)
	{
		if (vector.x < x) x = vector.x;
		if (vector.y < y) y = vector.y;
		if (vector.z < z) z = vector.z;
	}
	inline BWEle3DD crossProduct(const BWEle3DD& rkVector) const
	{
		return BWEle3DD(y * rkVector.z - z * rkVector.y,
			z * rkVector.x - x * rkVector.z,
			x * rkVector.y - y * rkVector.x
			);
	}
	float lenth() const;
	bool isZeroLength()
	{
		float sqlen = (x*x) + (y*y) + (z*z);
		return (sqlen < (1e-6 * 1e-6));
	}
	void normalize()
	{
		float len = lenth();
		x /= len; y /= len; z /= len;
	}
	BWQuaternion getRotationTo(const BWEle3DD& dest, const BWEle3DD& fallbackAxis = BWEle3DD::ZERO);

	static BWEle3DD ZERO;
	static BWEle3DD UNIT_SCALE;
	static BWEle3DD NEGATIVE_UNIT_Z;
	static BWEle3DD UNIT_Y;
	static BWEle3DD UNIT_X;
	static BWEle3DD UNIT_Z;
};


typedef unsigned char BYTE;
struct BWEle3B
{
	union
	{
		struct
		{
			BYTE x;
			BYTE y;
			BYTE z;
		};
		struct
		{
			BYTE r;
			BYTE g;
			BYTE b;
		};
		BYTE M[3];
	};
	BWEle3B() :x(BYTE(0)), y(BYTE(0)), z(BYTE(0))
	{

	}
	BWEle3B(BYTE _x, BYTE _y, BYTE _z) :x(_x), y(_y), z(_z)
	{

	}
	BWEle3B(const BWEle3B& ele)
	{
		if (this == &ele)
		{
			return;
		}
		x = ele.x;
		y = ele.y;
		z = ele.z;
	}
	const BWEle3B& operator=(const BWEle3B & ele)
	{
		if (this == &ele)
		{
			return *this;
		}
		x = ele.x;
		y = ele.y;
		z = ele.z;
		return *this;
	}
	bool operator== (const BWEle3B& ele)
	{
		if (ele.x == x &&
			ele.y == y &&
			ele.z == z)
		{
			return true;
		}
		return false;
	}
	const BWEle3B operator+(const BWEle3B& ele)
	{
		BWEle3B tmp;
		tmp.x = x + ele.x;
		tmp.y = y + ele.y;
		tmp.z = z + ele.z;
		return tmp;
	}
	const BWEle3B operator+(const BWEle3B& ele) const
	{
		BWEle3B tmp;
		tmp.x = x + ele.x;
		tmp.y = y + ele.y;
		tmp.z = z + ele.z;
		return tmp;
	}
	BYTE & operator[](int indxe)
	{
		return M[indxe];
	}
	const BYTE& operator[](int indxe) const
	{
		return M[indxe];
	}
};

struct BWEle3I
{
	union
	{
		struct
		{
			int x;
			int y;
			int z;
		};
		struct
		{
			int r;
			int g;
			int b;
		};
		int M[3];
	};
	BWEle3I() :x(0), y(0), z(0)
	{

	}
	BWEle3I(int _x, int _y, int _z) :x(_x), y(_y), z(_z)
	{

	}
	BWEle3I(const BWEle3I& ele)
	{
		if (this == &ele)
		{
			return;
		}
		x = ele.x;
		y = ele.y;
		z = ele.z;
	}
	const BWEle3I& operator=(const BWEle3I & ele)
	{
		if (this == &ele)
		{
			return *this;
		}
		x = ele.x;
		y = ele.y;
		z = ele.z;
		return *this;
	}
	bool operator== (const BWEle3I& ele)
	{
		if (ele.x == x &&
			ele.y == y &&
			ele.z == z)
		{
			return true;
		}
		return false;
	}
	int& operator[](int index)
	{
		return M[index];
	}
};
struct BWEle4I
{
	union
	{
		struct
		{
			int x;
			int y;
			int z;
			int w;
		};
		struct
		{
			int r;
			int g;
			int b;
			int a;
		};
	};
};
struct BWEle4DD
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		struct
		{
			float r;
			float g;
			float b;
			float a;
		};
		float M[4];
	};

	BWEle4DD() :x(0.0), y(0.0), z(0.0), w(1.0)
	{

	}
	BWEle4DD(const BWEle3DD& vec3D) :x(vec3D.x), y(vec3D.y), z(vec3D.z), w(0.0)
	{
	
	}
	BWEle4DD(float _x, float _y, float _z, float _w) :x(_x), y(_y), z(_z), w(_w)
	{

	}
	BWEle4DD(const BWEle4DD & ele)
	{
		if (this == &ele)
		{
			return;
		}
		x = ele.x;
		y = ele.y;
		z = ele.z;
		w = ele.w;
	}
	const BWEle4DD operator*(float w) const
	{
		BWEle4DD tmp;
		tmp.x = w * x;
		tmp.y = w * y;
		tmp.z = w * z;
		return tmp;
	}
	 BWEle4DD operator*(const BWEle4DD& ele4)
	{
		BWEle4DD tmp;
		tmp.r = ele4.r * r;
		tmp.g = ele4.g * g;
		tmp.b = ele4.b * b;
		tmp.w = 1.0;
		return tmp;
	}
	 const BWEle4DD operator*(const BWEle4DD& ele4) const
	{
		BWEle4DD tmp;
		tmp.r = ele4.r * r;
		tmp.g = ele4.g * g;
		tmp.b = ele4.b * b;
		tmp.w = 1.0;
		return tmp;
	}
	BWEle4DD operator*(float w)
	{
		BWEle4DD tmp;
		tmp.x = w * x;
		tmp.y = w * y;
		tmp.z = w * z;
		return tmp;
	}
	const BWEle4DD operator/(float w)
	{
		BWEle4DD tmp;
		tmp.x = x / w;
		tmp.y = y / w;
		tmp.z = z / w;
		return tmp;
	}
	const BWEle4DD operator/(float w) const
	{
		BWEle4DD tmp;
		tmp.x = x / w;
		tmp.y = y / w;
		tmp.z = z / w;
		return tmp;
	}
  const BWEle4DD operator+(const BWEle4DD& ele) const
	{
		BWEle4DD tmp;
		tmp.x = x + ele.x;
		tmp.y = y + ele.y;
		tmp.z = z + ele.z;
		return tmp;
	}
  const BWEle4DD operator+(const BWEle4DD& ele)
  {
	  //这种方式引起无限递归
	  //return static_cast<BWEle4DD >(static_cast<const BWEle4DD>(*this).operator +(ele));
	  BWEle4DD tmp;
	  tmp.x = ele.x + x;
	  tmp.y = ele.y + y;
	  tmp.z = ele.z + z;
	  return tmp;
  }
  const BWEle4DD operator-(const BWEle4DD& ele) const
  {
	  BWEle4DD tmp;
	  tmp.x = x - ele.x;
	  tmp.y = y - ele.y;
	  tmp.z = z - ele.z;
	  return tmp;
  }
  BWEle4DD operator-(const BWEle4DD& ele)
  {
	  BWEle4DD tmp;
	  tmp.x = x - ele.x;
	  tmp.y = y - ele.y;
	  tmp.z = z - ele.z;
	  return tmp;
	  //下面会引起递归调用
	  //return static_cast<BWEle4DD>(static_cast<const BWEle4DD>(*this) -(ele));
  }
	BWEle4DD& operator+=(const BWEle4DD& ele)
	{
		x += ele.x;
		y += ele.y;
		z += ele.z;
		return *this;
	}

 const BWEle4DD& operator=(const BWEle4DD& ele)
	{
		if (this == &ele)
		{
			return*this;
		}
		x = ele.x;
		y = ele.y;
		z = ele.z;
		w = ele.w;
		return *this;
	}
 const BWEle4DD& operator=(const BWEle3DD& ele)
 {
	 x = ele.x;
	 y = ele.y;
	 z = ele.z;
	 w = 0.0;
	 return *this;
 }
 float operator[](int i) const
 {
	 return M[i];
 }
 float& operator[](int i)
 {
	 return M[i];
 }
};


class BWQuaternion
{
public:
	inline BWQuaternion(float fw = 1.0, float fx = 0.0f, float fy = 0.0f, float fz = 0.0f) :
		w(fw), x(fx), y(fy), z(fz)
	{

	}
	inline BWQuaternion(const BWQuaternion& quaternion)
	{
		w = quaternion.w;
		x = quaternion.x;
		y = quaternion.y;
		z = quaternion.z;
	}
	inline BWEle3DD operator*(const BWEle3DD& vector) const
	{
		// 这里使用的并不是传统的四元数乘法公式  
		//是invidia 中使用的   是Rodrigues’ rotation formula 注意 这里的 x y z 是带有sin(A/2)*x 这种形式的  而不是旋转轴
		BWEle3DD uv, uuv;
		BWEle3DD qvec(x, y, z);
		uv = qvec.crossProduct(vector);
		uuv = qvec.crossProduct(uv);
		uv = uv * (w * 2.0f);
		uuv = uuv * 2.0f;
		return vector + uv + uuv;
	}
	inline BWQuaternion operator*(const BWQuaternion &quaternion) const
	{
		return BWQuaternion(
			w*quaternion.w - x*quaternion.x - y*quaternion.y - z*quaternion.z,
			w*quaternion.x + x*quaternion.w + y*quaternion.z - z*quaternion.y,
			w*quaternion.y + y*quaternion.w + z*quaternion.x - x*quaternion.z,
			w*quaternion.z + z*quaternion.w + x*quaternion.y - y*quaternion.x
			);
	}
	BWQuaternion operator*(float fscale) const
	{
		return BWQuaternion(fscale*w, fscale*x, fscale*y, fscale*z);
	}
	bool operator!=(const BWQuaternion& q) const
	{
		if (q.x == x && q.y == y && q.z == z  && q.w == w)
		{
			return false;
		}
		return true;
	}
	inline BWQuaternion Inverse() const
	{
		float fNorm = w*w + x*x + y*y + z*z;
		if (fNorm > 0.0f)
		{
			float fInvNorm = 1.0f / fNorm;
			return BWQuaternion(w*fInvNorm, -x*fInvNorm, -y*fInvNorm, -z*fInvNorm);
		}
		{
			return ZERO;
		}
	}
	inline float Norm() const
	{
		return w*w + x*x + y*y + z*z;
	}
	float normalize();
	
	void set(float fw, float fx, float fy, float fz)
	{
		w = fw; y = fy; z = fz; x = fx;
	}
	void ToRotationMatrix(BWMatrix4& matrix) const;
	void ToAxes(BWEle3DD* axes);

	void fromRotationMatrix(const BWMatrix4& matrix);
	void fromAngleAxis(const Radian& rfAngle, const BWEle3DD& rkAxis);
	void fromAxes(const BWEle3DD& x, const BWEle3DD& y, const BWEle3DD& z);
	void fromAxes(const BWEle3DD* axes);
	static const BWQuaternion ZERO;
	static const BWQuaternion IDENTITY;

	float w;
	float x; 
	float y;
	float z;
};

template<int Order>
struct TSHVector
{
	enum {MaxOrder = Order};
	enum {MaxBasis = Order * Order};
	float V[MaxBasis * 4]; // 输出的是RGBA  A是权重
};