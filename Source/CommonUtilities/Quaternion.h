#pragma once
namespace CU
{
	class Quaternion
	{
	public:
		Quaternion();
		~Quaternion();

		float Magnitude() const;
		float Magnitude2() const;

		const CU::Quaternion& Normalize();
		const Quaternion GetNormalized() const;

		const CU::Quaternion& Invert();
		const Quaternion GetInverse() const;

		const Quaternion& operator+=(const Quaternion& right);
		const Quaternion& operator-=(const Quaternion& right);
		const Quaternion& operator*=(const Quaternion& right);
		const Quaternion& operator*=(const float right);
		const Quaternion& operator/=(const float right);
		const Quaternion operator*() const;

		const std::string ToString() const;
	protected:
		float s;
		CU::Vector3f v;
	};


	const Quaternion operator+(Quaternion left, const Quaternion& right);
	const Quaternion operator-(Quaternion left, const Quaternion& right);
	const Quaternion operator*(Quaternion left, const Quaternion& right);
	const Quaternion operator*(Quaternion left, const float right);
	const Quaternion operator*(const float left, const Quaternion& right);
	const Quaternion operator/(Quaternion left, const float right);
}

