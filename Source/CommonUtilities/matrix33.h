#pragma once
#include <math.h>

#include "vector3.h"
#include "vector2.h"
#include "StaticArray.h"

#define self (*this)

namespace CU
{
	template<typename TYPE>
	class Matrix44;

	template<typename TYPE>
	class Matrix33
	{
	public:
		static const unsigned int NumRows = 3;
		static const unsigned int NumColumns = 3;
		static const unsigned int NumElements = NumRows * NumColumns;

		Matrix33()
		{
			m11 = static_cast<TYPE>(1); m12 = static_cast<TYPE>(0); m13 = static_cast<TYPE>(0);
			m21 = static_cast<TYPE>(0); m22 = static_cast<TYPE>(1); m23 = static_cast<TYPE>(0);
			m31 = static_cast<TYPE>(0); m32 = static_cast<TYPE>(0); m33 = static_cast<TYPE>(1);
		}

		Matrix33(const Matrix33& aRight)
		{
			m11 = aRight.m11; m12 = aRight.m12; m13 = aRight.m13;
			m21 = aRight.m21; m22 = aRight.m22; m23 = aRight.m23;
			m31 = aRight.m31; m32 = aRight.m32; m33 = aRight.m33;
		}

		Matrix33(const Matrix44<TYPE>& aRight)
		{
			m11 = aRight.m11; m12 = aRight.m12; m13 = aRight.m13;
			m21 = aRight.m21; m22 = aRight.m22; m23 = aRight.m23;
			m31 = aRight.m31; m32 = aRight.m32; m33 = aRight.m33;
		}

		template<typename U>
		Matrix33(const Matrix33<U>& aRight)
		{
			m11 = static_cast<TYPE>(aRight.m11); m12 = static_cast<TYPE>(aRight.m12); m13 = static_cast<TYPE>(aRight.m13);
			m21 = static_cast<TYPE>(aRight.m21); m22 = static_cast<TYPE>(aRight.m22); m23 = static_cast<TYPE>(aRight.m23);
			m31 = static_cast<TYPE>(aRight.m31); m32 = static_cast<TYPE>(aRight.m32); m33 = static_cast<TYPE>(aRight.m33);
		}

		Matrix33(const TYPE(&aArray)[NumElements])
		{
			m11 = aArray[0]; m12 = aArray[1]; m13 = aArray[2];
			m21 = aArray[3]; m22 = aArray[4]; m23 = aArray[5];
			m31 = aArray[6]; m32 = aArray[7]; m33 = aArray[8];
		}

		Matrix33(
			const TYPE a11, const TYPE a12, const TYPE a13,
			const TYPE a21, const TYPE a22, const TYPE a23,
			const TYPE a31, const TYPE a32, const TYPE a33)
		{
			m11 = a11; m12 = a12; m13 = a13;
			m21 = a21; m22 = a22; m23 = a23;
			m31 = a31; m32 = a32; m33 = a33;
		}

		~Matrix33() {}

		Matrix33& operator+=(const Matrix33& aRight)
		{
			m11 += aRight.m11; m12 += aRight.m12; m13 += aRight.m13;
			m21 += aRight.m21; m22 += aRight.m22; m23 += aRight.m23;
			m31 += aRight.m31; m32 += aRight.m32; m33 += aRight.m33;

			return self;
		}

		Matrix33 &operator-=(const Matrix33& aRight)
		{
			m11 -= aRight.m11; m12 -= aRight.m12; m13 -= aRight.m13;
			m21 -= aRight.m21; m22 -= aRight.m22; m23 -= aRight.m23;
			m31 -= aRight.m31; m32 -= aRight.m32; m33 -= aRight.m33;

			return self;
		}

		Matrix33 &operator*=(const Matrix33& aRight)
		{
			Matrix33 temp = self;

			m11 = (temp.m11 * aRight.m11) + (temp.m12 * aRight.m21) + (temp.m13 * aRight.m31);
			m12 = (temp.m11 * aRight.m12) + (temp.m12 * aRight.m22) + (temp.m13 * aRight.m32);
			m13 = (temp.m11 * aRight.m13) + (temp.m12 * aRight.m23) + (temp.m13 * aRight.m33);


			m21 = (temp.m21 * aRight.m11) + (temp.m22 * aRight.m21) + (temp.m23 * aRight.m31);
			m22 = (temp.m21 * aRight.m12) + (temp.m22 * aRight.m22) + (temp.m23 * aRight.m32);
			m23 = (temp.m21 * aRight.m13) + (temp.m22 * aRight.m23) + (temp.m23 * aRight.m33);


			m31 = (temp.m31 * aRight.m11) + (temp.m32 * aRight.m21) + (temp.m33 * aRight.m31);
			m32 = (temp.m31 * aRight.m12) + (temp.m32 * aRight.m22) + (temp.m33 * aRight.m32);
			m33 = (temp.m31 * aRight.m13) + (temp.m32 * aRight.m23) + (temp.m33 * aRight.m33);

			return self;

		}

		Matrix33 &operator=(const Matrix33& aRight)
		{
			m11 = aRight.m11; m12 = aRight.m12; m13 = aRight.m13;
			m21 = aRight.m21; m22 = aRight.m22; m23 = aRight.m23;
			m31 = aRight.m31; m32 = aRight.m32; m33 = aRight.m33;

			return self;
		}

		Matrix33& Transpose()
		{
			Matrix33 temp = self;

			m12 = temp.m21;
			m13 = temp.m31;

			m21 = temp.m12;
			m23 = temp.m32;

			m31 = temp.m13;
			m32 = temp.m23;

			return self;
		}

		static Matrix33 CreateRotateAroundX(const TYPE anAngle)
		{
			Matrix33 rotationX;

			rotationX.m11 = static_cast<TYPE>(1);
			rotationX.m12 = static_cast<TYPE>(0);
			rotationX.m13 = static_cast<TYPE>(0);

			rotationX.m21 = static_cast<TYPE>(0);
			rotationX.m22 = static_cast<TYPE>(cos(static_cast<double>(anAngle)));
			rotationX.m23 = static_cast<TYPE>(sin(static_cast<double>(anAngle)));

			rotationX.m31 = static_cast<TYPE>(0);
			rotationX.m32 = static_cast<TYPE>(-sin(static_cast<double>(anAngle)));
			rotationX.m33 = static_cast<TYPE>(cos(static_cast<double>(anAngle)));

			return rotationX;
		}

		static Matrix33 CreateRotateAroundY(const TYPE anAngle)
		{
			Matrix33 rotationY;

			rotationY.m11 = static_cast<TYPE>(cos(static_cast<double>(anAngle)));
			rotationY.m12 = static_cast<TYPE>(0);
			rotationY.m13 = static_cast<TYPE>(-sin(static_cast<double>(anAngle)));

			rotationY.m21 = static_cast<TYPE>(0);
			rotationY.m22 = static_cast<TYPE>(1);
			rotationY.m23 = static_cast<TYPE>(0);

			rotationY.m31 = static_cast<TYPE>(sin(static_cast<double>(anAngle)));
			rotationY.m32 = static_cast<TYPE>(0);
			rotationY.m33 = static_cast<TYPE>(cos(static_cast<double>(anAngle)));

			return rotationY;
		}

		static Matrix33 CreateRotateAroundZ(const TYPE anAngle)
		{
			Matrix33 rotationZ;

			rotationZ.m11 = static_cast<TYPE>(cos(static_cast<double>(anAngle)));
			rotationZ.m12 = static_cast<TYPE>(sin(static_cast<double>(anAngle)));
			rotationZ.m13 = static_cast<TYPE>(0);

			rotationZ.m21 = static_cast<TYPE>(-sin(static_cast<double>(anAngle)));
			rotationZ.m22 = static_cast<TYPE>(cos(static_cast<double>(anAngle)));
			rotationZ.m23 = static_cast<TYPE>(0);

			rotationZ.m31 = static_cast<TYPE>(0);
			rotationZ.m32 = static_cast<TYPE>(0);
			rotationZ.m33 = static_cast<TYPE>(1);

			return rotationZ;
		}

		const CU::Vector3f GetEulerRotation() const;

		Matrix33 Lerp(const Matrix33<TYPE>& aInterpolateToMatrix, const TYPE aInterpolatingSpeed)
		{
			m21 = m21 + aInterpolatingSpeed * (aInterpolateToMatrix.m21 - m21);
			m22 = m22 + aInterpolatingSpeed * (aInterpolateToMatrix.m22 - m22);
			m23 = m23 + aInterpolatingSpeed * (aInterpolateToMatrix.m23 - m23);

			m31 = m31 + aInterpolatingSpeed * (aInterpolateToMatrix.m31 - m31);
			m32 = m32 + aInterpolatingSpeed * (aInterpolateToMatrix.m32 - m32);
			m33 = m33 + aInterpolatingSpeed * (aInterpolateToMatrix.m33 - m33);

			myForwardVector.Normalize();
			myRightVector = myUpVector.Cross(myForwardVector).GetNormalized();
			myUpVector = myForwardVector.Cross(myRightVector).GetNormalized();

			return self;
		}

		Matrix33 LookAt(const CU::Vector3<TYPE>& aPositionToLookFrom, const CU::Vector3<TYPE>& aPositionToLookAt)
		{
			CU::Vector3f objectUpVector = CU::Vector3f(0.0f, 1.0f, 0.0f);

			float xSize = myRightVector.Length();
			float ySize = myUpVector.Length();
			float zSize = myForwardVector.Length();

			CU::Vector3f zAxis = aPositionToLookAt - aPositionToLookFrom;
			if (zSize <= 0)
			{
				DL_PRINT("FUCKED FORWARD call Marcus if things got wonkee %f %f %f", myForwardVector.x, myForwardVector.y, myForwardVector.z);
				zAxis = zAxis.GetNormalized();
			}
			else
			{
				zAxis = zAxis.GetNormalized() * zSize;
			}

			CU::Vector3f xAxis = objectUpVector.Cross(zAxis);
			if(xSize <= 0)
			{
				DL_PRINT("FUCKED RIGHT call Marcus if things got wonkee %f %f %f", myRightVector.x, myRightVector.y, myRightVector.z);
				xAxis = xAxis.GetNormalized();
			}
			else
			{
				xAxis = xAxis.GetNormalized() * xSize;
			}
			CU::Vector3f yAxis = zAxis.Cross(xAxis);
			if (ySize <= 0)
			{
				DL_PRINT("FUCKED UP call Marcus if things got wonkee %f %f %f", myUpVector.x, myUpVector.y, myUpVector.z);
				yAxis = yAxis.GetNormalized();
			}
			else
			{
				yAxis = yAxis.GetNormalized() * ySize;
			}

			m11 = xAxis.x;
			m12 = xAxis.y;
			m13 = xAxis.z;

			m21 = yAxis.x;
			m22 = yAxis.y;
			m23 = yAxis.z;

			m31 = zAxis.x;
			m32 = zAxis.y;
			m33 = zAxis.z;

			return self;
		}

		static Matrix33 Transpose(Matrix33 aMatrix)
		{
			Matrix33 temp = aMatrix;

			temp.m21 = aMatrix.m12;
			temp.m31 = aMatrix.m13;

			temp.m12 = aMatrix.m21;
			temp.m32 = aMatrix.m23;

			temp.m13 = aMatrix.m31;
			temp.m23 = aMatrix.m32;
			return temp;
		}

		void SetPosition(const Vector2<TYPE> &aPosition)
		{
			m31 = aPosition.x;
			m32 = aPosition.y;
		}

		Vector2<TYPE> GetPosition() const
		{
			return Vector2<TYPE>(m31, m32);
		}

		Matrix33& Rotate2D(const TYPE anAngle)
		{
			self = CreateRotateAroundZ(anAngle) * self;
			return self;
		}

		Matrix33 &Set2DRotation(const TYPE anAngle)
		{
			static_assert(false, "this function doesn't do what it's supposed to do, mvh carl");
			Matrix33<TYPE> tempMatrix = self;
			tempMatrix.CreateRotateAroundZ(anAngle);
			tempMatrix.m31 = m31;
			tempMatrix.m32 = m32;
			return tempMatrix;
		}

#pragma warning( disable : 4201 ) // remove warning namless struct
		union
		{
			struct
			{
				TYPE	m11, m12, m13,
						m21, m22, m23,
						m31, m32, m33;
			};
			struct
			{
				Vector3<TYPE> myRightVector;
				Vector3<TYPE> myUpVector;
				Vector3<TYPE> myForwardVector;
			};
#pragma warning( default : 4201 )

			CU::StaticArray<TYPE, NumElements> myMatrix;
		};

		static const Matrix33 Identity;
		static const Matrix33 Zero;
	};

	template <typename TYPE>
	const CU::Vector3f Matrix33<TYPE>::GetEulerRotation() const
	{
		const float aX = atan2(m33, m23);
		const float aY = sqrt(m11 * m11 + m21 * m21);
		const float aZ = atan2(m21, m11);
		return CU::Vector3f(aX, aY, aZ);
	}

	using Matrix33f = Matrix33<float>;

	template<typename TYPE>
	Matrix33<TYPE> operator+(Matrix33<TYPE> aLeft, const Matrix33<TYPE>& aRight)
	{
		return aLeft += aRight;
	}

	template<typename TYPE>
	Matrix33<TYPE> operator-(Matrix33<TYPE> aLeft, const Matrix33<TYPE>& aRight)
	{
		return aLeft -= aRight;
	}

	template<typename TYPE>
	Matrix33<TYPE> operator *(Matrix33<TYPE> aLeftValue, const Matrix33<TYPE>& aRightValue)
	{
		return aLeftValue *= aRightValue;
	}

	template<typename TYPE>
	Vector2<TYPE> operator*(Vector2<TYPE> aLeftValue, const Matrix33<TYPE>& aRightValue)
	{
		Vector3<TYPE> tempVec(aLeftValue.x, aLeftValue.y, 1);
		Vector3<TYPE> returnVec;
		returnVec.x = aRightValue.m11 * tempVec.x
			+ aRightValue.m21 * tempVec.y
			+ aRightValue.m31 * tempVec.z;
		returnVec.y = aRightValue.m12 * tempVec.x
			+ aRightValue.m22 * tempVec.y
			+ aRightValue.m32 * tempVec.z;

		return Vector2<TYPE>(returnVec.x, returnVec.y);
	}

	template<typename TYPE>
	Vector3<TYPE> operator*(Vector3<TYPE> aLeft, const Matrix33<TYPE>& aRight)
	{
		Vector3<TYPE> temp;

		temp.x = (aLeft.x * aRight.m11) + (aLeft.y * aRight.m21) + (aLeft.z * aRight.m31);
		temp.y = (aLeft.x * aRight.m12) + (aLeft.y * aRight.m22) + (aLeft.z * aRight.m32);
		temp.z = (aLeft.x * aRight.m13) + (aLeft.y * aRight.m23) + (aLeft.z * aRight.m33);

		return temp;
	}

	template<typename TYPE>
	bool operator ==(const Matrix33<TYPE> &aLeft, const Matrix33<TYPE> &aRight)
	{
		if (aLeft.m11 == aRight.m11 && aLeft.m12 == aRight.m12 && aLeft.m13 == aRight.m13 &&
			aLeft.m21 == aRight.m21 && aLeft.m22 == aRight.m22 && aLeft.m23 == aRight.m23 &&
			aLeft.m31 == aRight.m31 && aLeft.m32 == aRight.m32 && aLeft.m33 == aRight.m33)
		{
			return true;
		}

		return false;
	}

	template<typename TYPE>
	bool operator !=(const Matrix33<TYPE> &aLeft, const Matrix33<TYPE> &aRight)
	{
		return (!(aLeft == aRight));
	}

	template<typename TYPE> const Matrix33<TYPE> Matrix33<TYPE>::Zero = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	template<typename TYPE> const Matrix33<TYPE> Matrix33<TYPE>::Identity = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
}

#undef self
