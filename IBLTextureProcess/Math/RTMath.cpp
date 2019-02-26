#include "RTMath.h"
#include <random>
int Jacobi(double a[], int n, double v[], double eps, int jt)
{
	int i, j, p, q, u, w, t, s, l;
	double fm, cn, sn, omega, x, y, d;
	l = 1;
	for (i = 0; i <= n - 1; i++)
	{
		v[i*n + i] = 1.0;
		for (j = 0; j <= n - 1; j++)
		{
			if (i != j)
			{
				v[i*n + j] = 0.0;
			}
		}
	}
	while (1 == 1)
	{
		fm = 0.0;
		for (i = 0; i <= n - 1; i++)
		{
			for (j = 0; j <= n - 1; j++)
			{
				d = fabs(a[i*n + j]);
				if ((i != j) && (d > fm))
				{
					fm = d;
					p = i;
					q = j;
				}
			}
		}
		if (fm<eps)
		{
			return(1);
		}
		if (l>jt)
		{
			return(-1);
		}
		l = l + 1;
		u = p*n + q;
		w = p*n + p;
		t = q*n + p;
		s = q*n + q;
		x = -a[u];
		y = (a[s] - a[w]) / 2.0;
		omega = x / sqrt(x*x + y*y);
		if (y < 0.0)
		{
			omega = -omega;
		}
		sn = 1.0 + sqrt(1.0 - omega*omega);
		sn = omega / sqrt(2.0*sn);
		cn = sqrt(1.0 - sn*sn);
		fm = a[w];
		a[w] = fm*cn*cn + a[s] * sn*sn + a[u] * omega;
		a[s] = fm*sn*sn + a[s] * cn*cn - a[u] * omega;
		a[u] = 0.0;
		a[t] = 0.0;
		for (j = 0; j <= n - 1; j++)
		{
			if ((j != p) && (j != q))
			{
				u = p*n + j;
				w = q*n + j;
				fm = a[u];
				a[u] = fm*cn + a[w] * sn;
				a[w] = -fm*sn + a[w] * cn;
			}
		}
		for (i = 0; i <= n - 1; i++)
		{
			if ((i != p) && (i != q))
			{
				u = i*n + p;
				w = i*n + q;
				fm = a[u];
				a[u] = fm*cn + a[w] * sn;
				a[w] = -fm*sn + a[w] * cn;
			}
		}
		for (i = 0; i <= n - 1; i++)
		{
			u = i*n + p;
			w = i*n + q;
			fm = v[u];
			v[u] = fm*cn + v[w] * sn;
			v[w] = -fm*sn + v[w] * cn;
		}
	}
	return(1);
}

void Swap(float& f1, float& f2)
{
	float tmp = f1;
	f1 = f2;
	f2 = tmp;
}

void Swap(double& d1, double& d2)
{
	double tmp = d1;
	d1 = d2;
	d2 = tmp;
}

float Clamp(float Value, float Min, float Max)
{
	if (Value > Max) return Max;
	if (Value < Min) return Min;
	return Value;
}

float Lerp(float T, float V1, float V2)
{
	return (1.0 - T) * V1 + T * V2;
}

int Rand_int()
{
	return rand();
}
float Rand_float()
{
	return float(rand()) / RAND_MAX;
}
BWPoint4DD TriangleCentroid(const BWPoint4DD& p1, const BWPoint4DD& p2, const BWPoint4DD & p3)
{
	BWPoint4DD tmp = (p1 + p2 + p3) / 3;
	return (p1 + p2 + p3) / 3;
}
BWPoint4DD TriangleCentroid(const BWTriangle& triangle)
{
	BWPoint4DD p1, p2, p3;
	triangle.GetFacePoints(p1, p2, p3);
	return TriangleCentroid(p1, p2, p3);
}

double TriangleArea(const BWPoint4DD& p1, const BWPoint4DD& p2, const BWPoint4DD & p3)
{
	return Lenth(Cross(p1 - p2, p1 - p3))/2.0;
}
double TriangleArea(const BWTriangle& triangle)
{
	BWPoint4DD p1 , p2, p3;
	triangle.GetFacePoints(p1, p2, p3);
	return TriangleArea(p1, p2, p3);
}
BWVector3DD Cross(const BWVector3DD& vec1, const BWVector3DD& vec2)
{
	BWVector3DD tmp;
	tmp.x = vec1.y * vec2.z - vec2.y * vec1.z;
	tmp.y = vec1.z * vec2.x - vec2.z * vec1.x;
	tmp.z = vec1.x * vec2.y - vec2.x * vec1.y;
	return tmp;
}
double Lenth(const BWVector3DD& a)
{
	return sqrt(a.x * a.x + a.y*a.y + a.z*a.z);
}

BWVector3D GetDirection(const BWVector3D &From, const BWVector3D &To)
{
	BWVector3D Dir = To - From;
	Dir.normalize();
	return Dir;
}

double Dot(const BWVector3DD& a, const BWVector3DD& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
void Normalize( BWPoint3DD& vec)
{
	vec = vec / Lenth(vec);
}

 const BWVector3DD GetRotatedZ(const BWVector3DD vec, double angle)
 {
	 if (angle == 0.0)
		 return vec;

	 float sinAngle = (float)sin(PI*angle / 180);
	 float cosAngle = (float)cos(PI*angle / 180);

	 return BWVector3DD(vec.x*cosAngle - vec.y*sinAngle,
		 vec.x*sinAngle + vec.y*cosAngle,
		 vec.z);
 }
 const BWVector3DD GetRotatedAxis(const BWVector3DD vec, double angle, const BWVector3DD & axis)
 {
	 if (angle == 0.0)
		 return vec;
	 BWVector3DD u = axis;
	 Normalize(u);


	 BWVector3DD rotMatrixRow0, rotMatrixRow1, rotMatrixRow2;

	 float sinAngle = (float)sin(PI*angle / 180);
	 float cosAngle = (float)cos(PI*angle / 180);
	 float oneMinusCosAngle = 1.0f - cosAngle;

	 rotMatrixRow0.x = (u.x)*(u.x) + cosAngle*(1 - (u.x)*(u.x));
	 rotMatrixRow0.y = (u.x)*(u.y)*(oneMinusCosAngle)-sinAngle*u.z;
	 rotMatrixRow0.z = (u.x)*(u.z)*(oneMinusCosAngle)+sinAngle*u.y;

	 rotMatrixRow1.x = (u.x)*(u.y)*(oneMinusCosAngle)+sinAngle*u.z;
	 rotMatrixRow1.y = (u.y)*(u.y) + cosAngle*(1 - (u.y)*(u.y));
	 rotMatrixRow1.z = (u.y)*(u.z)*(oneMinusCosAngle)-sinAngle*u.x;

	 rotMatrixRow2.x = (u.x)*(u.z)*(oneMinusCosAngle)-sinAngle*u.y;
	 rotMatrixRow2.y = (u.y)*(u.z)*(oneMinusCosAngle)+sinAngle*u.x;
	 rotMatrixRow2.z = (u.z)*(u.z) + cosAngle*(1 - (u.z)*(u.z));

	 return BWVector3DD(Dot(rotMatrixRow0, vec),
		 Dot(rotMatrixRow1, vec),
		 Dot(rotMatrixRow2, vec));
 }
 BWPoint4DD PointMultMatrix(const BWPoint4DD& point, const float* matrix)
 {
	 BWPoint4DD reslut;
	 reslut[0] = point[0] * matrix[0] + point[1] * matrix[4] + point[2] * matrix[8] + point[3] * matrix[12];
	 reslut[1] = point[0] * matrix[1] + point[1] * matrix[5] + point[2] * matrix[9] + point[3] * matrix[13];
	 reslut[2] = point[0] * matrix[2] + point[1] * matrix[6] + point[2] * matrix[10] + point[3] * matrix[14];
	 reslut[3] = point[0] * matrix[3] + point[1] * matrix[7] + point[2] * matrix[11] + point[3] * matrix[15];
	 return reslut;
 }
 BWPoint4DD MatrixMultPoint(const double* matrix, const BWPoint4DD& point)
 {
	 BWPoint4DD reslut;
	 reslut[0] = point[0] * matrix[0] + point[1] * matrix[1] + point[2] * matrix[2] + point[3] * matrix[3];
	 reslut[1] = point[0] * matrix[4] + point[1] * matrix[5] + point[2] * matrix[6] + point[3] * matrix[7];
	 reslut[2] = point[0] * matrix[8] + point[1] * matrix[9] + point[2] * matrix[10] + point[3] * matrix[11];
	 reslut[3] = point[0] * matrix[12] + point[1] * matrix[13] + point[2] * matrix[14] + point[3] * matrix[15];
	 return reslut;
 }

 void MatrixTranspose(const double* matrix, double* trans)
 {
	 for (int i = 0; i < 4; i++)
	 {
		 for (int j = 0; j < 4; j++)
		 {
			 trans[i * 4 + j] = matrix[j * 4 + i];
		 }
	 }
 }
 void MatrixTranspose(const float * matrix, float * trans)
 {
	 for (int i = 0; i < 4; i++)
	 {
		 for (int j = 0; j < 4; j++)
		 {
			 trans[i * 4 + j] = matrix[j * 4 + i];
		 }
	 }
 }

 void MatrixMultMatrix(const double *matrix1, const double* matrix2, double* reslut)
 {
	 for (int i = 0; i < 4; i++)
	 {
		 for (int k = 0; k < 4; k++)
		 {
			 double tmp = 0;
			 for (int j = 0; j < 4; j++)
			 {
				 tmp += matrix1[i * 4 + j] * matrix2[j * 4 + k];
			 }
			 reslut[i * 4 + k] = tmp;
		 }
	 }
 }
 void MatrixMultMatrix(const double *source, double* result)
 {
	 double matrix[16];
	 for (int i = 0; i < 4; i++)
	 {
		 for (int k = 0; k < 4; k++)
		 {
			 double tmp = 0;
			 for (int j = 0; j < 4; j++)
			 {
				 tmp += source[i * 4 + j] * result[j * 4 + k];
			 }
			 matrix[i * 4 + k] = tmp;
		 }
	 }
	 for (int i = 0; i < 16; i++)
	 {
		 result[i] = matrix[i];
	 }
 }

 //bool CollisionDetecte(const Cuboid& cuboid1, const Cuboid & cuboid2)
 //{
	// BWVector3DD AToB;
	// AToB = cuboid2._center - cuboid1._center;
	// // 得到十五个轴 都是单位长度 且和AToB保持一致
	// BWVector3DD Axle[15];
	// for (int i = 0; i < 3; i++)
	// {
	//	 if (Dot(cuboid1._axi[i], AToB) < 0)
	//	 {
	//		 Axle[i].x = -cuboid1._axi[i].x;
	//		 Axle[i].y = -cuboid1._axi[i].y;
	//		 Axle[i].z = -cuboid1._axi[i].z;
	//	 }
	//	 else
	//	 {
	//		 Axle[i] = cuboid1._axi[i];

	//	 }

	//	 if (Dot(cuboid2._axi[i], AToB) < 0)
	//	 {
	//		 Axle[i + 3].x = -cuboid2._axi[i].x;
	//		 Axle[i + 3].y = -cuboid2._axi[i].y;
	//		 Axle[i + 3].z = -cuboid2._axi[i].z;
	//	 }
	//	 else
	//	 {
	//		 Axle[i + 3] = cuboid2._axi[i];

	//	 }
	// }
	// BWVector3DD tmp1;
	// for (int i = 0; i < 3; i++)
	// {

	//	 for (int k = 0; k < 3; k++)
	//	 {
	//		 tmp1 = Cross(cuboid1._axi[i], cuboid1._axi[k]);
	//		 if (Dot(tmp1, AToB) < 0)
	//		 {
	//			 Axle[6 + i * 3 + k].x = -tmp1.x;
	//			 Axle[6 + i * 3 + k].y = -tmp1.y;
	//			 Axle[6 + i * 3 + k].z = -tmp1.z;
	//		 }
	//		 else
	//		 {
	//			 Axle[6 + i * 3 + k] = tmp1;

	//		 }
	//	 }
	// }


	// //得到A 和 B  的各八个长方体向量

	// BWVector3DD PointVec[16];
	// float tmp2[3];

	// for (int i = 0; i < 8; i++)
	// {

	//	 tmp2[0] = tmp2[1] = tmp2[2] = 0;



	//	 tmp2[0] += ((i & 1) ? (-1) : 1) * cuboid1._halfLenth.x;
	//	 tmp2[1] += ((i & 2) ? (-1) : 1) * cuboid1._halfLenth.y;
	//	 tmp2[2] += ((i & 4) ? (-1) : 1) * cuboid1._halfLenth.z;



	//	 PointVec[i].x = tmp2[0] * cuboid1._axi[0].x + tmp2[1] * cuboid1._axi[1].x + tmp2[2] * cuboid1._axi[2].x;
	//	 PointVec[i].y = tmp2[0] * cuboid1._axi[0].y + tmp2[1] * cuboid1._axi[1].y + tmp2[2] * cuboid1._axi[2].y;
	//	 PointVec[i].z = tmp2[0] * cuboid1._axi[0].z + tmp2[1] * cuboid1._axi[1].z + tmp2[2] * cuboid1._axi[2].z;

	//	 /* PointVec[i].x += A.center.x ;
	//	 PointVec[i].y += A.center.y ;
	//	 PointVec[i].z += A.center.z ;*/


	//	 tmp2[0] = tmp2[1] = tmp2[2] = 0;



	//	 tmp2[0] += ((i & 1) ? (-1) : 1) *  cuboid2._halfLenth.x;
	//	 tmp2[1] += ((i & 2) ? (-1) : 1) *  cuboid2._halfLenth.y;
	//	 tmp2[2] += ((i & 4) ? (-1) : 1) *  cuboid2._halfLenth.z;


	//	 PointVec[i + 8].x = tmp2[0] * cuboid2._axi[0].x + tmp2[1] * cuboid2._axi[1].x + tmp2[2] * cuboid2._axi[2].x;
	//	 PointVec[i + 8].y = tmp2[0] * cuboid2._axi[0].y + tmp2[1] * cuboid2._axi[1].y + tmp2[2] * cuboid2._axi[2].y;
	//	 PointVec[i + 8].z = tmp2[0] * cuboid2._axi[0].z + tmp2[1] * cuboid2._axi[1].z + tmp2[2] * cuboid2._axi[2].z;


	// }

	// //int   n = 8 ;
	// // glColor3f(1.0 , 0.0 , 0.0) ;
	// // glBegin(GL_QUADS) ;
	// // glVertex3f(PointVec[1 + n].x ,PointVec[1+ n].y ,PointVec[1+ n].z) ;
	// // glVertex3f(PointVec[0+ n].x ,PointVec[0+ n].y ,PointVec[0+ n].z) ;
	// // glVertex3f(PointVec[2+ n].x ,PointVec[2+ n].y ,PointVec[2+ n].z) ;
	// // glVertex3f(PointVec[3+ n].x ,PointVec[3+ n].y ,PointVec[3+ n].z) ;
	// // glEnd() ;

	// // glColor3f(1.0 , 1.0 , 0.0) ;
	// // glBegin(GL_QUADS) ;
	// // glVertex3f(PointVec[1+ n].x ,PointVec[1+ n].y ,PointVec[1+ n].z) ;
	// // glVertex3f(PointVec[5+ n].x ,PointVec[5+ n].y ,PointVec[5+ n].z) ;
	// // glVertex3f(PointVec[4+ n].x ,PointVec[4+ n].y ,PointVec[4+ n].z) ;
	// // glVertex3f(PointVec[0+ n].x ,PointVec[0+ n].y ,PointVec[0+ n].z) ;
	// // glEnd() ;

	// // glColor3f(0.0 , 0.0 , 0.0) ;
	// // glBegin(GL_QUADS) ;
	// // glVertex3f(PointVec[0+ n].x ,PointVec[0+ n].y ,PointVec[0+ n].z) ;
	// // glVertex3f(PointVec[4+ n].x ,PointVec[4+ n].y ,PointVec[4+ n].z) ;
	// // glVertex3f(PointVec[6+ n].x ,PointVec[6+ n].y ,PointVec[6+ n].z) ;
	// // glVertex3f(PointVec[2+ n].x ,PointVec[2+ n].y ,PointVec[2+ n].z) ;
	// // glEnd() ;

	// // glColor3f(0.0 , 1.0 , 0.0) ;
	// // glBegin(GL_QUADS) ;
	// // glVertex3f(PointVec[2+ n].x ,PointVec[2+ n].y ,PointVec[2+ n].z) ;
	// // glVertex3f(PointVec[3+ n].x ,PointVec[3+ n].y ,PointVec[3+ n].z) ;
	// // glVertex3f(PointVec[7+ n].x ,PointVec[7+ n].y ,PointVec[7+ n].z) ;
	// // glVertex3f(PointVec[6+ n].x ,PointVec[6+ n].y ,PointVec[6+ n].z) ;

	// // glEnd() ;

	// // glColor3f(1.0 , 0.0 , 1.0) ;
	// // glBegin(GL_QUADS) ;
	// // glVertex3f(PointVec[3+ n].x ,PointVec[3+ n].y ,PointVec[3+ n].z) ;
	// // glVertex3f(PointVec[7+ n].x ,PointVec[7+ n].y ,PointVec[7+ n].z) ;
	// // glVertex3f(PointVec[5+ n].x ,PointVec[5+ n].y ,PointVec[5+ n].z) ;
	// // glVertex3f(PointVec[1+ n].x ,PointVec[1+ n].y ,PointVec[1+ n].z) ;
	// // glEnd() ;

	// // glColor3f(0.0 , 1.0 , 1.0) ;
	// // glBegin(GL_QUADS) ;
	// // glVertex3f(PointVec[4+ n].x ,PointVec[4+ n].y ,PointVec[4+ n].z) ;
	// // glVertex3f(PointVec[5+ n].x ,PointVec[5+ n].y ,PointVec[5+ n].z) ;
	// // glVertex3f(PointVec[7+ n].x ,PointVec[7+ n].y ,PointVec[7+ n].z) ;
	// // glVertex3f(PointVec[6+ n].x ,PointVec[6+ n].y ,PointVec[6+ n].z) ;
	// // glEnd() ;


	// for (int i = 0; i< 15; i++)
	// {
	//	 BWVector3DD tmp;
	//	 tmp = Axle[i];
	//	 if (Lenth(tmp) < 0.001)
	//	 {
	//		 continue;
	//	 }
	//	 float aMax = 0;
	//	 float bMax = 0;
	//	 float AdotB;
	//	 float ABProj = Dot(AToB, Axle[i]);
	//	 for (int k = 0; k < 8; k++)
	//	 {
	//		 AdotB = Dot(tmp, PointVec[k]);
	//		 if (AdotB > aMax)
	//		 {
	//			 aMax = AdotB;
	//		 }

	//		 AdotB = Dot(tmp, PointVec[k + 8]);
	//		 if (AdotB < bMax)
	//		 {
	//			 bMax = AdotB;
	//		 }
	//	 }
	//	 if ((aMax - bMax) < ABProj)  //只要有一个 轴上的投影之和小于obb盒的中心在轴上的投影  则没有碰撞 就是说存在一个分割面
	//	 {
	//		 return false;
	//	 }
	// }
	// return  true;
 //}
 bool ReflcteRay(const BWVector3DD& normal, const BWRay& ray, BWPoint3DD& reflect)
 {
	 if (Dot(normal , ray._vector) >0)
	 {
		 return false;
	 }
	 reflect  = ray._vector - normal * 2.0f * Dot(normal, ray._vector);
	 return true;
 }
 void GetTranslateMatrix(double x, double y, double z, double * reslut)
 {
	 for (int i = 0; i < 16; i++)
	 {
		 reslut[i] = 0.0;
	 }
	 reslut[0] = 1.0;  reslut[5] = 1.0; reslut[10] = 1.0; reslut[15] = 1.0;
	 reslut[3] = x;    reslut[7] = y;   reslut[11] = z;
 }
 void GetScaleMatrix(float x, float y, float z, double * reslut)
 {
	 for (int i = 0; i < 16; i++)
	 {
		 reslut[i] = 0.0;
	 }
	 reslut[0] = x; reslut[5] = y; reslut[10] = z; reslut[15] = 1.0;
 }
 void GetRotateMatrix(double theta, float x, float y, float z, double* reslut)
 {
	 if (x == 0 && y == 0 && z == 0)
	 {
		 return;
	 }
	 double mod = sqrt(x*x + y*y + z*z);
	 x = x / mod;
	 y = y / mod;
	 z = z / mod;
	 double x2 = x * x;
	 double y2 = y * y;
	 double z2 = z * z;
	 theta = (theta*PI) / 180.0;
	 double c = cosf(theta);
	 double s = sinf(theta);

	 reslut[0] = x2 + (1 - x2)*c;   reslut[1] = x*y*(1 - c) - z * s;   reslut[2] = x*z*(1 - c) + y*s;    reslut[3] = 0;
	 reslut[4] = x*y*(1 - c) + z*s; reslut[5] = y2 + (1 - y2)*c;       reslut[6] = y*z*(1 - c) - x *s;   reslut[7] = 0;
	 reslut[8] = x*z*(1 - c) - y*s; reslut[9] = y*z*(1 - c) + x*s;     reslut[10] = z2 + (1 - z2)*c;     reslut[11] = 0;
	 reslut[12] = 0;                reslut[13] = 0;                     reslut[14] = 0;                    reslut[15] = 1;

	/* reslut[0] = x2 + (1 - x2)*c;   reslut[4] = x*y*(1 - c) - z * s;   reslut[8] = x*z*(1 - c) + y*s;    reslut[12] = 0;
	 reslut[1] = x*y*(1 - c) + z*s; reslut[5] = y2 + (1 - y2)*c;       reslut[9] = y*z*(1 - c) - x *s;   reslut[13] = 0;
	 reslut[2] = x*z*(1 - c) - y*s; reslut[6] = y*z*(1 - c) + x*s;     reslut[10] = z2 + (1 - z2)*c;     reslut[14] = 0;
	 reslut[3] = 0;                reslut[7] = 0;                     reslut[11] = 0;                    reslut[15] = 1;*/
 }

 //bool RayIntersectOBB(const BWRay& ray, const Cuboid& OBB , float&t)
 //{
	// float tmin = FLT_MIN_;
	// float tmax = FLT_MAX_;
	// float e, f;
	// BWPoint3DD p = OBB.GetCenter() - ray._start;
	// for (int i = 0; i < 3; i++)
	// {
	//	 e = Dot(OBB._axi[i], p);
	//	 f = Dot(OBB._axi[i], ray._vector);
	//	 if ( abs(f) > ESP)
	//	 {
	//		 float tmpf = 1.0 / f;
	//		 float t1 = (e + OBB._halfLenth[i]) * tmpf;
	//		 float t2 = (e - OBB._halfLenth[i]) * tmpf;
	//		 if (t1 > t2)
	//		 {
	//			 Swap(t1, t2);
	//		 }
	//		 if (t1 > tmin)
	//		 {
	//			 tmin = t1;
	//		 }
	//		 if (t2 < tmax)
	//		 {
	//			 tmax = t2;
	//		 }
	//		 if (tmin > tmax ||  tmax < 0)
	//		 {
	//			 t = 0;
	//			 return false;
	//		 }
	//	 }
	//	 else
	//	 {
	//		 if ((-e - OBB._halfLenth[i]) > 0 || (-e + OBB._halfLenth[i]) < 0)
	//		 {
	//			 t = 0;
	//			 return false;
	//		 }
	//	 }
	// }
	// if (tmin > 0 )
	// {
	//	 t = tmin;
	//	 return true;
	// }
	// t = tmax;
	// return true;
 //}

 //bool RayIntersectAABB(const BWRay& ray, const Cuboid& AABB, float &t)
 //{
	// float tmin = FLT_MIN_;
	// float tmax = FLT_MAX_;
	// float e, f;
	// BWPoint3DD p = AABB.GetCenter() - ray._start;
	// for (int i = 0; i < 3; i++)
	// {
	//	 e = p[i]; //Dot(AABB._axi[i], p);
	//	 f = ray._vector[i];//Dot(AABB._axi[i], ray._vector);
	//	 if (abs(f) > ESP)
	//	 {
	//		 float tmpf = 1.0 / f;
	//		 float t1 = (e + AABB._halfLenth[i]) * tmpf;
	//		 float t2 = (e - AABB._halfLenth[i]) * tmpf;
	//		 if (t1 > t2)
	//		 {
	//			 Swap(t1, t2);
	//		 }
	//		 if (t1 > tmin)
	//		 {
	//			 tmin = t1;
	//		 }
	//		 if (t2 < tmax)
	//		 {
	//			 tmax = t2;
	//		 }
	//		 if (tmin > tmax || tmax < 0)
	//		 {
	//			 t = 0;
	//			 return false;
	//		 }
	//	 }
	//	 else
	//	 {
	//		 if ((-e - AABB._halfLenth[i]) > 0 || (-e + AABB._halfLenth[i]) < 0)
	//		 {
	//			 t = 0;
	//			 return false;
	//		 }
	//	 }
	// }
	// if (tmin > 0)
	// {
	//	 t = tmin;
	//	 return true;
	// }
	// t = tmax;
	// return true;
 //}

 bool RayIntersectTriangle(const BWRay& ray, const BWPoint4DD& p1, const BWPoint4DD &p2, const BWPoint4DD& p3, float &t , float &u ,float &v)
 {
	 BWVector3DD D = ray._vector;
	 BWVector3DD E1 = p2 - p1, E2 = p3 - p1, T = ray._start - p1;
	 BWVector3DD P, Q;
	 P = Cross(D, E2);
	 double w = Dot(P, E1);
	 if (w < 0)
	 {
		 T = p1 - ray._start;
		 w = -w;
	 }
	 if (w < ESP)
	 {
		 return false;
	 }
	  u = Dot(P, T);
	 if (u < 0.0 || u > w)
	 {
		 return false;
	 }
	 Q = Cross(T, E1);
	 v = Dot(Q, D);
	 if (v < 0.0 ||  u + v > w)
	 {
		 return false;
	 }
	 t = Dot(Q, E2);
	 if (t < 0 )
	 {
		 t = 0;
		 return false;
	 }
	 w = 1.0 / w;
	 t *= w;
	 v *= w;
	 u *= w;
	 return true;
 }


 BWPoint4DD SphereCoordToDescartesCoord(float theta, float phi, float radius)
 {
	 return BWVector4DD(float(radius*sin(theta)*cos(phi)),
		 float(radius*sin(theta)*sin(phi)),
		 float(radius*cos(theta)), 1.0);
 }

 BWVector3DD LinearInterpolation(const BWVector3DD& start, const BWVector3DD& end, double k)
 {
	 return (end - start) * k + start;
 }

 bool AABBIntersctRay(const BWRay& Ray, const BWVector3D &Min, const BWVector3D &Max)
 {
	 double ox = Ray._start.x; double oy = Ray._start.y; double oz = Ray._start.z;
	 double dx = Ray._vector.x; double dy = Ray._vector.y; double dz = Ray._vector.z;
	 double tx_min, ty_min, tz_min;
	 double tx_max, ty_max, tz_max;

	 //x0,y0,z0为包围体的最小顶点
	 //x1,y1,z1为包围体的最大顶点
	 if (abs(dx) < 0.000001f)
	 {
		 //若射线方向矢量的x轴分量为0且原点不在盒体内
		 if (ox < Max.x || ox > Min.x)
			 return false;
	 }
	 else
	 {
		 if (dx >= 0)
		 {
			 tx_min = (Min.x - ox) / dx;
			 tx_max = (Max.x - ox) / dx;
		 }
		 else
		 {
			 tx_min = (Max.x - ox) / dx;
			 tx_max = (Min.x - ox) / dx;
		 }

	 }


	 if (abs(dy) < 0.000001f)
	 {
		 //若射线方向矢量的x轴分量为0且原点不在盒体内
		 if (oy < Max.y || oy > Min.y)
			 return false;
	 }
	 else
	 {
		 if (dy >= 0)
		 {
			 ty_min = (Min.y - oy) / dy;
			 ty_max = (Max.y - oy) / dy;
		 }
		 else
		 {
			 ty_min = (Max.y - oy) / dy;
			 ty_max = (Min.y - oy) / dy;
		 }

	 }


	 if (abs(dz) < 0.000001f)
	 {
		 //若射线方向矢量的x轴分量为0且原点不在盒体内
		 if (oz < Max.z || oz > Min.z)
			 return false;
	 }
	 else
	 {
		 if (dz >= 0)
		 {
			 tz_min = (Min.z - oz) / dz;
			 tz_max = (Max.z - oz) / dz;
		 }
		 else
		 {
			 tz_min = (Max.z - oz) / dz;
			 tz_max = (Min.z - oz) / dz;
		 }

	 }

	 double t0, t1;

	 //光线进入平面处（最靠近的平面）的最大t值 
	 t0 = TMax(tz_min, TMax(tx_min, ty_min));

	 //光线离开平面处（最远离的平面）的最小t值
	 t1 = TMin(tz_max, TMin(tx_max, ty_max));

	 return t0 < t1;
 }
