#include "stdafx.h"
#include "Quaternion.h"


CU::Quaternion::Quaternion() : s(1.f), v(CU::Vector3f::Zero)
{
}

CU::Quaternion::~Quaternion()
{
}

float CU::Quaternion::Magnitude() const
{
	return sqrt(Magnitude2());
}

float CU::Quaternion::Magnitude2() const
{
	Quaternion result = *this * *(*this);
	const float resultMagnitude = result.s;
	return resultMagnitude;
}

const CU::Quaternion& CU::Quaternion::Normalize()
{
	*this /= Magnitude();

	return *this;
}

const CU::Quaternion CU::Quaternion::GetNormalized() const
{
	Quaternion result = *this;
	result.Normalize();
	return result;
}

const CU::Quaternion& CU::Quaternion::Invert()
{
	return *(*this) / Magnitude2();
}

const CU::Quaternion CU::Quaternion::GetInverse() const
{
	Quaternion result = *this;
	return result.Invert();
}

const CU::Quaternion& CU::Quaternion::operator+=(const Quaternion& right)
{
	s += right.s;
	v += right.v;

	return *this;
}

const CU::Quaternion& CU::Quaternion::operator-=(const Quaternion& right)
{
	s -= right.s;
	v -= right.v;

	return *this;
}

const CU::Quaternion& CU::Quaternion::operator*=(const Quaternion& right)
{
	const float oldS = s;
	const CU::Vector3f oldV = v;
	/*s = (oldS * right.s - oldV.x * right.v.x - oldV.y * right.v.y - oldV.z * right.v.z);
	v.x = (oldS * right.v.x + right.s * oldV.x + oldV.y * right.v.z - right.v.y * oldV.z);
	v.y = (oldS * right.v.y + right.s * oldV.y + oldV.z * right.v.x - right.v.z * oldV.x);
	v.z = (oldS * right.v.z + right.s * oldV.z + oldV.x * right.v.y - right.v.x * oldV.y);*/

	s = oldS * right.s - oldV.Dot(right.v);
	v = oldS * right.v + right.s * oldV + oldV.Cross(right.v);
	
	return *this;
}

const CU::Quaternion& CU::Quaternion::operator*=(const float right)
{
	s += right;
	v *= right;

	return *this;
}

const CU::Quaternion& CU::Quaternion::operator/=(const float right)
{
	s /= right;
	v /= right;
	return *this;
}

const CU::Quaternion CU::Quaternion::operator*() const
{
	Quaternion quaternion = *this;
	-quaternion.v;

	return quaternion;
}

const std::string CU::Quaternion::ToString() const
{
	return std::to_string(s) + " + " + std::to_string(v.x) + "i + " + std::to_string(v.y) + "j + " + std::to_string(v.z) + "k";
}

const CU::Quaternion CU::operator+(Quaternion left, const Quaternion& right)
{
	left += right;
	return left;
}

const CU::Quaternion CU::operator-(Quaternion left, const Quaternion& right)
{
	left -= right;
	return left;
}

const CU::Quaternion CU::operator*(Quaternion left, const Quaternion& right)
{
	left *= right;
	return left;
}

const CU::Quaternion CU::operator*(Quaternion left, const float right)
{
	return left *= right;
}

const CU::Quaternion CU::operator*(const float left, const Quaternion& right)
{
	return right * left;
}

const CU::Quaternion CU::operator/(Quaternion left, const float right)
{
	return left /= right;
}
