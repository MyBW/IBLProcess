#include "BWPrimitive.h"
#include <vector>
BWRay::BWRay()
{
}

BWRay::~BWRay()
{

}


BWTriangle::BWTriangle()
{

}
BWTriangle::~BWTriangle()
{

}

void BWTriangle::GetFacePoints(BWPoint4DD & p1, BWPoint4DD &p2, BWPoint4DD&p3)
{
	p1 = _vertex[0]; p2 = _vertex[1]; p3 = _vertex[2];
}

void BWTriangle::GetFacePoints(BWPoint4DD & p1, BWPoint4DD &p2, BWPoint4DD&p3) const
{
	p1 = _vertex[0]; p2 = _vertex[1]; p3 = _vertex[2];
}


BWMatrix4 BWMatrix4::IDENTITY ( 1.0f, 0.0f, 0.0f, 0.0f,
0.0, 1.0, 0.0, 0.0,
0.0, 0.0, 1.0, 0.0,
0.0, 0.0, 0.0, 1.0 );
BWMatrix4 BWMatrix4::CLIPSPACE2DTOIMAGESPACE(
	0.5, 0.0, 0.0, 0.5,
	0.0, -0.5, 0.0, 0.5,
	0.0, 0.0, 1, 0.0,
	0.0, 0.0, 0.0, 1
);


//c++11 以后可以这么用 但是vs2008不支持c++11
//BWMatrix4 BWMatrix4::IDENTITY = { 1.0f, 0.0f, 0.0f, 0.0f,
//0.0, 1.0, 0.0, 0.0,
//0.0, 0.0, 1.0, 0.0,
//0.0, 0.0, 0.0, 1.0 };
//BWMatrix4 BWMatrix4::CLIPSPACE2DTOIMAGESPACE =
//{
//	0.5, 0.0, 0.0, 0.5,
//	0.0, -0.5, 0.0, 0.5,
//	0.0, 0.0, 1, 0.0,
//	0.0, 0.0, 0.0, 1,
//};
BWBOX::BWBOX() :_center(), _halfLenth()
{
	_axi[0].x = 1.0;
	_axi[1].y = 1.0;
	_axi[2].z = 1.0;
}

BWBOX::~BWBOX()
{
}
BWBOX::BWBOX(const BWBOX& BWBOX)
{
	if (this == &BWBOX)
	{
		return;
	}
	_center = BWBOX._center;
	_halfLenth = BWBOX._halfLenth;
	_axi[0] = BWBOX._axi[0];
	_axi[1] = BWBOX._axi[1];
	_axi[2] = BWBOX._axi[2];
}

void BWBOX::SetCenter(const BWPoint4DD & center)
{
	_center = center;
}
const BWPoint4DD& BWBOX::GetCenter() const
{
	return _center;
}

const BWPoint4DD& BWBOX::GetCenter()
{
	return _center;
}
void BWBOX::SetAxi(BWPoint3DD& axi1, BWPoint3DD& axi2, BWPoint3DD& axi3)
{
	_axi[0] = axi1;
	_axi[1] = axi2;
	_axi[2] = axi3;
}

void BWBOX::GetAxi(BWPoint3DD& axi1, BWPoint3DD& axi2, BWPoint3DD& axi3) const
{
	axi1 = _axi[0];
	axi2 = _axi[1];
	axi3 = _axi[2];
}

void BWBOX::GetAxi(BWPoint3DD& axi1, BWPoint3DD& axi2, BWPoint3DD& axi3)
{
	axi1 = _axi[0];
	axi2 = _axi[1];
	axi3 = _axi[2];
}
const BWPoint3DD* BWBOX::GetAxi() const
{
	return _axi;
}
 BWPoint3DD* BWBOX::GetAxi()
{
	return _axi;
}
void BWBOX::SetHalfLenth(float x, float y, float z)
{
	_halfLenth.x = x;
	_halfLenth.y = y;
	_halfLenth.z = z;
}
void BWBOX::GetHalfLenth(float & x, float & y, float &z) const
{
	x = _halfLenth.x;
	y = _halfLenth.y;
	z = _halfLenth.z;
}

void BWBOX::GetHalfLenth(float & x, float & y, float &z)
{
	x = _halfLenth.x;
	y = _halfLenth.y;
	z = _halfLenth.z;
}
void BWBOX::GetHalfLenth(double & x, double & y, double &z)
{
	x = _halfLenth.x;
	y = _halfLenth.y;
	z = _halfLenth.z;
}
const BWBOX&  BWBOX::operator=(const BWBOX & BWBOX)
{
	if (this == &BWBOX)
	{
		return *this;
	}
	_center = BWBOX._center;
	_halfLenth = BWBOX._halfLenth;
	_axi[0] = BWBOX._axi[0];
	_axi[1] = BWBOX._axi[1];
	_axi[2] = BWBOX._axi[2];
}

void BWBOX::Draw()
{
	/*std::vector<BWObject::Vertex> pointList;
	BWObject::Vertex tmp1;
	tmp1.normal = BWVector3DD(0.0, 0.0, 1.0);
	tmp1.point = BWVector4DD(1.0, 1.0, 1.0, 0.0);
	pointList.push_back(tmp1);
	BWObject::Vertex tmp2;
	tmp2.normal = BWVector3DD(0.0, 0.0, 1.0);
	tmp2.point = BWVector4DD(1.0, 1.0, -1.0, 0.0);
	pointList.push_back(tmp2);
	BWObject::Vertex tmp3;
	tmp3.normal = BWVector3DD(0.0, 0.0, 1.0);
	tmp3.point = BWVector4DD(-1.0, 1.0, -1.0, 0.0);
	pointList.push_back(tmp3);
	BWObject::Vertex tmp4;
	tmp4.normal = BWVector3DD(0.0, 0.0, 1.0);
	tmp4.point = BWVector4DD(-1.0, 1.0, 1.0, 0.0);
	pointList.push_back(tmp4);
	BWObject::Vertex tmp5;
	tmp5.normal = BWVector3DD(0.0, 0.0, 1.0);
	tmp5.point = BWVector4DD(1.0, -1.0, 1.0, 0.0);
	pointList.push_back(tmp5);
	BWObject::Vertex tmp6;
	tmp6.normal = BWVector3DD(0.0, 0.0, 1.0);
	tmp6.point = BWVector4DD(1.0, -1.0, -1.0, 0.0);
	pointList.push_back(tmp6);
	BWObject::Vertex tmp7;
	tmp7.normal = BWVector3DD(0.0, 0.0, 1.0);
	tmp7.point = BWVector4DD(-1.0, -1.0, -1.0, 0.0);
	pointList.push_back(tmp7);
	BWObject::Vertex tmp8;
	tmp8.normal = BWVector3DD(0.0, 0.0, 1.0);
	tmp8.point = BWVector4DD(-1.0, -1.0, 1.0, 0.0);
	pointList.push_back(tmp8);
*/
	BWPoint4DD p1 = _axi[0] * _halfLenth.x;
	BWPoint4DD p2 = _axi[1] * _halfLenth.y;
	BWPoint4DD p3 = _axi[2] * _halfLenth.z;
	BWPoint4DD p8[8];
	p8[0] = p1 + p2 + p3 + _center;
	p8[1] = p1 + p2 - p3 + _center;
	p8[2] = p2 - p1 - p3 + _center;
	p8[3] = p2 - p1 + p3 + _center;

	p8[4] = p1 - p2 + p3 + _center;
	p8[5] = p1 - p2 - p3 + _center;
	p8[6] = BWPoint4DD(0.0, 0.0, 0.0, 0.0) - p2 - p1 - p3 + _center;
	p8[7] = BWPoint4DD(0.0, 0.0, 0.0, 0.0) - p2 - p1 + p3 + _center;
	/*GLuint face1[] =
	{
		0, 1, 2, 3, 0,
		4, 5, 6, 7, 4,
		5, 1, 2, 6, 7, 3
	};
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_DOUBLE, sizeof(BWPoint4DD), p8);
	glColorPointer(3, GL_DOUBLE, sizeof(BWObject::Vertex), &pointList[0]._color);
	glNormalPointer(GL_DOUBLE, sizeof(BWObject::Vertex), &pointList[0].normal);
	glDrawElements(GL_LINE_STRIP, 16, GL_UNSIGNED_INT, face1);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);*/
}


