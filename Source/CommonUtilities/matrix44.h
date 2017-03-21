#pragma once
#define _USE_MATH_DEFINES

#include <math.h>
#include <cmath>
#include <intrin.h>

#include "vector4.h"
#include "vector3.h"
#include "StaticArray.h"

#include "InvertMatrix.h"
#include "matrix33.h"

namespace CU
{
	enum class Axees
	{
		X,
		Y,
		Z
	};

	template<typename TYPE>
	class Matrix33;

	template<typename TYPE>
	class Matrix44
	{
	public:
		static const unsigned int NumRows = 4;
		static const unsigned int NumColumns = 4;
		static const unsigned int NumElements = NumRows * NumColumns;

		Matrix44()
		{
			m11 = static_cast<TYPE>(1); m12 = static_cast<TYPE>(0); m13 = static_cast<TYPE>(0); m14 = static_cast<TYPE>(0);
			m21 = static_cast<TYPE>(0); m22 = static_cast<TYPE>(1); m23 = static_cast<TYPE>(0); m24 = static_cast<TYPE>(0);
			m31 = static_cast<TYPE>(0); m32 = static_cast<TYPE>(0); m33 = static_cast<TYPE>(1); m34 = static_cast<TYPE>(0);
			m41 = static_cast<TYPE>(0); m42 = static_cast<TYPE>(0); m43 = static_cast<TYPE>(0); m44 = static_cast<TYPE>(1);
		}

		Matrix44(const Matrix44& aOther)
		{
			m11 = aOther.m11; m12 = aOther.m12; m13 = aOther.m13; m14 = aOther.m14;
			m21 = aOther.m21; m22 = aOther.m22; m23 = aOther.m23; m24 = aOther.m24;
			m31 = aOther.m31; m32 = aOther.m32; m33 = aOther.m33; m34 = aOther.m34;
			m41 = aOther.m41; m42 = aOther.m42; m43 = aOther.m43; m44 = aOther.m44;
		}

		Matrix44(const Matrix33<TYPE>& aOther)
		{
			m11 = aOther.m11; m12 = aOther.m12; m13 = aOther.m13; m14 = static_cast<TYPE>(0);
			m21 = aOther.m21; m22 = aOther.m22; m23 = aOther.m23; m24 = static_cast<TYPE>(0);
			m31 = aOther.m31; m32 = aOther.m32; m33 = aOther.m33; m34 = static_cast<TYPE>(0);
			m41 = static_cast<TYPE>(0); m42 = static_cast<TYPE>(0); m43 = static_cast<TYPE>(0); m44 = static_cast<TYPE>(1);
		}

		template<typename U>
		Matrix44(const Matrix44<U>& aOther)
		{
			m11 = static_cast<TYPE>(aOther.m11); m12 = static_cast<TYPE>(aOther.m12); m13 = static_cast<TYPE>(aOther.m13); m14 = static_cast<TYPE>(aOther.m14);
			m21 = static_cast<TYPE>(aOther.m21); m22 = static_cast<TYPE>(aOther.m22); m23 = static_cast<TYPE>(aOther.m23); m24 = static_cast<TYPE>(aOther.m24);
			m31 = static_cast<TYPE>(aOther.m31); m32 = static_cast<TYPE>(aOther.m32); m33 = static_cast<TYPE>(aOther.m33); m34 = static_cast<TYPE>(aOther.m34);
			m41 = static_cast<TYPE>(aOther.m41); m42 = static_cast<TYPE>(aOther.m42); m43 = static_cast<TYPE>(aOther.m43); m44 = static_cast<TYPE>(aOther.m44);
		}

		Matrix44(const TYPE(&anArray)[NumElements])
		{
			m11 = anArray[0]; m12 = anArray[1]; m13 = anArray[2]; m14 = anArray[3];
			m21 = anArray[4]; m22 = anArray[5]; m23 = anArray[6]; m24 = anArray[7];
			m31 = anArray[8]; m32 = anArray[9]; m33 = anArray[10]; m34 = anArray[11];
			m41 = anArray[12]; m42 = anArray[13]; m43 = anArray[14]; m44 = anArray[15];
		}

		Matrix44(
			const TYPE a11, const TYPE a12, const TYPE a13, const TYPE a14,
			const TYPE a21, const TYPE a22, const TYPE a23, const TYPE a24,
			const TYPE a31, const TYPE a32, const TYPE a33, const TYPE a34,
			const TYPE a41, const TYPE a42, const TYPE a43, const TYPE a44)
		{
			m11 = a11; m12 = a12; m13 = a13; m14 = a14;
			m21 = a21; m22 = a22; m23 = a23; m24 = a24;
			m31 = a31; m32 = a32; m33 = a33; m34 = a34;
			m41 = a41; m42 = a42; m43 = a43; m44 = a44;
		}

		~Matrix44() {}

		Matrix44& operator+=(const Matrix44& aRight)
		{
			m11 += aRight.m11; m12 += aRight.m12; m13 += aRight.m13; m14 += aRight.m14;
			m21 += aRight.m21; m22 += aRight.m22; m23 += aRight.m23; m24 += aRight.m24;
			m31 += aRight.m31; m32 += aRight.m32; m33 += aRight.m33; m34 += aRight.m34;
			m41 += aRight.m41; m42 += aRight.m42; m43 += aRight.m43; m44 += aRight.m44;
			return *this;
		}

		Matrix44& operator-=(const Matrix44& aRight)
		{
			m11 -= aRight.m11; m12 -= aRight.m12; m13 -= aRight.m13; m14 -= aRight.m14;
			m21 -= aRight.m21; m22 -= aRight.m22; m23 -= aRight.m23; m24 -= aRight.m24;
			m31 -= aRight.m31; m32 -= aRight.m32; m33 -= aRight.m33; m34 -= aRight.m34;
			m41 -= aRight.m41; m42 -= aRight.m42; m43 -= aRight.m43; m44 -= aRight.m44;
			return *this;
		}

		Matrix44& operator*=(const TYPE aScalar)
		{
			for (unsigned int i = 0; i < NumElements; ++i)
			{
				myMatrix[i] *= aScalar;
			}

			return *this;
		}

		Matrix44& operator*=(const Matrix44& aRight)
		{
			Matrix44 temp = *this;
			const __m128 X = aRight.m1;
			const __m128 Y = aRight.m2;
			const __m128 Z = aRight.m3;
			const __m128 W = aRight.m4;

			__m128 temp1, temp2;

			temp1 = _mm_set1_ps(temp.m11);
			temp2 = _mm_mul_ps(X, temp1);
			temp1 = _mm_set1_ps(temp.m12);
			temp2 = _mm_add_ps(_mm_mul_ps(Y, temp1), temp2);
			temp1 = _mm_set1_ps(temp.m13);
			temp2 = _mm_add_ps(_mm_mul_ps(Z, temp1), temp2);
			temp1 = _mm_set1_ps(temp.m14);
			temp2 = _mm_add_ps(_mm_mul_ps(W, temp1), temp2);

			_mm_store_ps(&temp.myMatrix[0], temp2);

			temp1 = _mm_set1_ps(temp.m21);
			temp2 = _mm_mul_ps(X, temp1);
			temp1 = _mm_set1_ps(temp.m22);
			temp2 = _mm_add_ps(_mm_mul_ps(Y, temp1), temp2);
			temp1 = _mm_set1_ps(temp.m23);
			temp2 = _mm_add_ps(_mm_mul_ps(Z, temp1), temp2);
			temp1 = _mm_set1_ps(temp.m24);
			temp2 = _mm_add_ps(_mm_mul_ps(W, temp1), temp2);

			_mm_store_ps(&temp.myMatrix[4], temp2);

			temp1 = _mm_set1_ps(temp.m31);
			temp2 = _mm_mul_ps(X, temp1);
			temp1 = _mm_set1_ps(temp.m32);
			temp2 = _mm_add_ps(_mm_mul_ps(Y, temp1), temp2);
			temp1 = _mm_set1_ps(temp.m33);
			temp2 = _mm_add_ps(_mm_mul_ps(Z, temp1), temp2);
			temp1 = _mm_set1_ps(temp.m34);
			temp2 = _mm_add_ps(_mm_mul_ps(W, temp1), temp2);

			_mm_store_ps(&temp.myMatrix[8], temp2);

			temp1 = _mm_set1_ps(temp.m41);
			temp2 = _mm_mul_ps(X, temp1);
			temp1 = _mm_set1_ps(temp.m42);
			temp2 = _mm_add_ps(_mm_mul_ps(Y, temp1), temp2);
			temp1 = _mm_set1_ps(temp.m43);
			temp2 = _mm_add_ps(_mm_mul_ps(Z, temp1), temp2);
			temp1 = _mm_set1_ps(temp.m44);
			temp2 = _mm_add_ps(_mm_mul_ps(W, temp1), temp2);

			_mm_store_ps(&temp.myMatrix[12], temp2);

			memcpy(this, &temp, sizeof(TYPE) * 16);
			return *this;
		}

		Matrix44& operator=(const Matrix44& aRight)
		{
			m11 = aRight.m11; m12 = aRight.m12; m13 = aRight.m13; m14 = aRight.m14;
			m21 = aRight.m21; m22 = aRight.m22; m23 = aRight.m23; m24 = aRight.m24;
			m31 = aRight.m31; m32 = aRight.m32; m33 = aRight.m33; m34 = aRight.m34;
			m41 = aRight.m41; m42 = aRight.m42; m43 = aRight.m43; m44 = aRight.m44;
			return *this;
		}

		Matrix44& operator=(const Matrix33<TYPE>& aRight)
		{
			m11 = aRight.m11; m12 = aRight.m12; m13 = aRight.m13; m14 = 0;
			m21 = aRight.m21; m22 = aRight.m22; m23 = aRight.m23; m24 = 0;
			m31 = aRight.m31; m32 = aRight.m32; m33 = aRight.m33; m34 = 0;
			m41 = 0; m42 = 0; m43 = 0; m44 = 1;
			return *this;
		}

		TYPE operator[](const unsigned int aIndex) const
		{
			return myMatrix[aIndex];
		}

		TYPE& operator[](const unsigned int aIndex)
		{
			return myMatrix[aIndex];
		}

		Matrix44& Transpose()
		{
			Matrix44 temp = *this;

			m12 = temp.m21;
			m13 = temp.m31;
			m14 = temp.m41;

			m21 = temp.m12;
			m23 = temp.m32;
			m24 = temp.m42;

			m31 = temp.m13;
			m32 = temp.m23;
			m34 = temp.m43;

			m41 = temp.m14;
			m42 = temp.m24;
			m43 = temp.m34;

			return *this;
		}

		Matrix44 CreateProjectionMatrixLH(TYPE aNear, TYPE aFar, TYPE aWidth, TYPE aHeight, TYPE aFov)
		{
	/*		Matrix44 temp;
			TYPE    SinFov;
			TYPE    CosFov;
			TYPE    Height;
			TYPE    Width;

			float aAspectRatio = aHeight / aWidth;

			SinFov = sin(0.5f * aFov);
			CosFov = cos(0.5f * aFov);

			Height = CosFov / SinFov;
			Width = Height * aAspectRatio;
*/
			TYPE scaling = aFar / (aFar - aNear);

			/*		temp.myMatrix[0] = Width;
					temp.myMatrix[5] = Height;
					temp.myMatrix[10] = scaling;
					temp.myMatrix[11] = 1.f;
					temp.myMatrix[14] = -scaling * aNear;
					temp.myMatrix[15] = 0;

					return temp;*/

			Matrix44<TYPE> returnMatrix = Matrix44<TYPE>::Identity;

			//float AspectRatioX = aWidth / aHeight;
			float AspectRatioY = aHeight / aWidth;
			float FovX = aFov * (static_cast<float>(M_PI) / 180.0f);
			float TanFovX = tan(FovX / 2.0f);
			float FovY = 2.0f * atan(TanFovX * AspectRatioY);

			float TanFovY = tan(FovY / 2.0f);
			float FarMinusNear = aFar - aNear;
			float OneDivTanFov = 1.0f / TanFovY;
			float FovYDivFarMinusNear = FovY / FarMinusNear;

			returnMatrix.my2DArray[0][0] = AspectRatioY * OneDivTanFov;
			returnMatrix.my2DArray[1][1] = OneDivTanFov;
			returnMatrix.my2DArray[2][2] = FovYDivFarMinusNear;
			returnMatrix.my2DArray[2][2] = scaling;

			returnMatrix.my2DArray[2][3] = 1.f;
			returnMatrix.my2DArray[3][2] = -aNear * FovYDivFarMinusNear;
			returnMatrix.my2DArray[3][2] = -aNear * scaling;

			returnMatrix.my2DArray[3][3] = 0;

			return returnMatrix;

			//TYPE width = aWidth, height = aHeight;
			//Matrix44<TYPE> temp;
			//TYPE aspectRatio = width / height;
			//TYPE scaling = aFar / (aFar - aNear);
			//// it was std::tan, sin seems te give far better results then tan
			//// maybe makes GUI somewhat koko
			//TYPE yScale = 1.0f / std::sin(aFov / 2.0f);
			//TYPE xScale = yScale / aspectRatio;
			//temp.myMatrix[0] = xScale;
			//temp.myMatrix[5] = yScale;
			//temp.myMatrix[10] = scaling;
			//temp.myMatrix[14] = -scaling * aNear;
			//temp.myMatrix[11] = 1;
			//temp.myMatrix[15] = 0;
			//return temp;

			//Matrix44 temp = Matrix44::Identity;
			//float aAspectRatio = aWidth / aHeight;

			//TYPE sinFov;
			//TYPE cosFov;
			//TYPE height;
			//TYPE width;

			//sinFov = sin(0.5f * aFov);
			//cosFov = cos(0.5f * aFov);


			//height = cosFov / sinFov;
			//width = height / aAspectRatio;

			//TYPE scaling = aFar / (aFar - aNear);

			//temp.m11 = width;
			//temp.m12 = 0.0f;
			//temp.m13 = 0.0f;
			//temp.m14 = 0.0f;

			//temp.m21 = 0.0f;
			//temp.m22 = height;
			//temp.m23 = 0.0f;
			//temp.m24 = 0.0f;

			//temp.m31 = 0.0f;
			//temp.m32 = 0.0f;
			//temp.m33 = scaling;
			//temp.m34 = 1.1f;

			//temp.m41 = 0.0f;
			//temp.m42 = 0.0f;
			//temp.m43 = -scaling * aNear;
			//temp.m44 = 0.0f;

			//return temp;
		}


		Matrix44 CreateOrthogonalProjectionMatrixLH(TYPE aNear, TYPE aFar, TYPE aWidth, TYPE aHeight)
		{
			Matrix44f matrix;

			matrix.m11 = 2.f / aWidth;

			matrix.m22 = 2.f / aHeight;

			matrix.m33 = 1.f / (aFar - aNear);
			matrix.m43 = aNear / (aNear - aFar);

			matrix.m44 = 1.f;

			return matrix;
		}

		void SetEulerRotation(const Vector3f& aRotation);
		//Doesn't work, don't use
		const CU::Vector3f GetEulerRotation() const;

		static Matrix44 CreateRotateAroundX(const TYPE aAngle)
		{
			Matrix44 rotationX;

			rotationX.m11 = static_cast<TYPE>(1);
			rotationX.m12 = static_cast<TYPE>(0);
			rotationX.m13 = static_cast<TYPE>(0);
			rotationX.m14 = static_cast<TYPE>(0);

			rotationX.m21 = static_cast<TYPE>(0);
			rotationX.m22 = static_cast<TYPE>(cos(static_cast<double>(aAngle)));
			rotationX.m23 = static_cast<TYPE>(sin(static_cast<double>(aAngle)));
			rotationX.m24 = static_cast<TYPE>(0);

			rotationX.m31 = static_cast<TYPE>(0);
			rotationX.m32 = static_cast<TYPE>(-sin(static_cast<double>(aAngle)));
			rotationX.m33 = static_cast<TYPE>(cos(static_cast<double>(aAngle)));
			rotationX.m34 = static_cast<TYPE>(0);

			rotationX.m41 = static_cast<TYPE>(0);
			rotationX.m42 = static_cast<TYPE>(0);
			rotationX.m43 = static_cast<TYPE>(0);
			rotationX.m44 = static_cast<TYPE>(1);

			return rotationX;
		}

		static Matrix44 CreateRotateAroundY(const TYPE aAngle)
		{
			Matrix44 rotationY;

			rotationY.m11 = static_cast<TYPE>(cos(static_cast<double>(aAngle)));
			rotationY.m12 = static_cast<TYPE>(0);
			rotationY.m13 = static_cast<TYPE>(-sin(static_cast<double>(aAngle)));
			rotationY.m14 = static_cast<TYPE>(0);

			rotationY.m21 = static_cast<TYPE>(0);
			rotationY.m22 = static_cast<TYPE>(1);
			rotationY.m23 = static_cast<TYPE>(0);
			rotationY.m24 = static_cast<TYPE>(0);

			rotationY.m31 = static_cast<TYPE>(sin(static_cast<double>(aAngle)));
			rotationY.m32 = static_cast<TYPE>(0);
			rotationY.m33 = static_cast<TYPE>(cos(static_cast<double>(aAngle)));
			rotationY.m34 = static_cast<TYPE>(0);

			rotationY.m41 = static_cast<TYPE>(0);
			rotationY.m42 = static_cast<TYPE>(0);
			rotationY.m43 = static_cast<TYPE>(0);
			rotationY.m44 = static_cast<TYPE>(1);

			return rotationY;
		}

		static Matrix44 CreateRotateAroundZ(const TYPE aAngle)
		{
			Matrix44 rotationZ;

			rotationZ.m11 = static_cast<TYPE>(cos(static_cast<double>(aAngle)));
			rotationZ.m12 = static_cast<TYPE>(sin(static_cast<double>(aAngle)));
			rotationZ.m13 = static_cast<TYPE>(0);
			rotationZ.m14 = static_cast<TYPE>(0);

			rotationZ.m21 = static_cast<TYPE>(-sin(static_cast<double>(aAngle)));
			rotationZ.m22 = static_cast<TYPE>(cos(static_cast<double>(aAngle)));
			rotationZ.m23 = static_cast<TYPE>(0);
			rotationZ.m24 = static_cast<TYPE>(0);

			rotationZ.m31 = static_cast<TYPE>(0);
			rotationZ.m32 = static_cast<TYPE>(0);
			rotationZ.m33 = static_cast<TYPE>(1);
			rotationZ.m44 = static_cast<TYPE>(0);

			rotationZ.m41 = static_cast<TYPE>(0);
			rotationZ.m42 = static_cast<TYPE>(0);
			rotationZ.m43 = static_cast<TYPE>(0);
			rotationZ.m44 = static_cast<TYPE>(1);

			return rotationZ;
		}

		static Matrix44 Transpose(Matrix44 aMatrix)
		{
			Matrix44 transposed = aMatrix;

			transposed.Transpose();

			return transposed;
		}

		void SetPosition(const TYPE aX, const TYPE aY, const TYPE aZ)
		{
			m41 = aX;
			m42 = aY;
			m43 = aZ;
		}

		void SetPosition(const Vector3<TYPE>& aPosition)
		{
			m41 = aPosition.x;
			m42 = aPosition.y;
			m43 = aPosition.z;
		}

		Vector3<TYPE> GetScale() const
		{
			return Vector3<TYPE>(myRightVector.Length(), myUpVector.Length(), myForwardVector.Length());
		}

		Vector3<TYPE> GetPosition() const
		{
			return Vector3<TYPE>(m41, m42, m43);
		}

		Vector3<TYPE>& GetPosition()
		{
			return myPosition;
		}

		Matrix44<TYPE> GetRotation() const
		{
			return Matrix44(m11, m12, m13, 0,
							m21, m22, m23, 0,
							m31, m32, m33, 0,
							0, 0, 0, 1);
		}
		
		Matrix44<TYPE>& SetRotation(const Matrix33<TYPE>& aMatrix)
		{
			m11 = aMatrix.m11;
			m12 = aMatrix.m12;
			m13 = aMatrix.m13;

			m21 = aMatrix.m21;
			m22 = aMatrix.m22;
			m23 = aMatrix.m23;

			m31 = aMatrix.m31;
			m32 = aMatrix.m32;
			m33 = aMatrix.m33;

			return *this;
		}

		Matrix44<TYPE>& Rotate(float anAngle, Axees anAxis)
		{
			Matrix44<TYPE> temp;

			switch (anAxis)
			{
			case Axees::X:
				temp = CreateRotateAroundX(anAngle);
				break;

			case Axees::Y:
				temp = CreateRotateAroundY(anAngle);
				break;

			case Axees::Z:
				temp = CreateRotateAroundZ(anAngle);
				break;
			}

			Vector4<TYPE> tempLoc = Vector4<TYPE>(GetPosition().x, GetPosition().y, GetPosition().z, 1);
			m41 = 0;
			m42 = 0;
			m43 = 0;

			*this = temp * *this;
			SetPosition(Vector3<TYPE>(tempLoc.x, tempLoc.y, tempLoc.z));

			return *this;
		}
		Matrix44<TYPE>& RotateAroundAxis(float anAngle, Axees anAxis)
		{
			Matrix44<TYPE> temp;

			switch (anAxis)
			{
			case Axees::X:
				temp = CreateRotateAroundX(anAngle);
				break;

			case Axees::Y:
				temp = CreateRotateAroundY(anAngle);
				break;

			case Axees::Z:
				temp = CreateRotateAroundZ(anAngle);
				break;
			}

			Vector4<TYPE> tempLoc = Vector4<TYPE>(GetPosition().x, GetPosition().y, GetPosition().z, 1);
			m41 = 0;
			m42 = 0;
			m43 = 0;

			*this = *this * temp;
			SetPosition(Vector3<TYPE>(tempLoc.x, tempLoc.y, tempLoc.z));

			return *this;
		}

		/* rotates x,y then z*/
		Matrix44<TYPE>& Rotate(const float x, const float y, const float z)
		{
			Rotate(x, Axees::X);
			Rotate(y, Axees::Y);
			Rotate(z, Axees::Z);

			return *this;
		}

		/* rotates x,y then z*/
		Matrix44<TYPE>& RotateAroundAxes(const float x, const float y, const float z)
		{
			RotateAroundAxis(x, Axees::X);
			Rotate(y, Axees::Y);
			RotateAroundAxis(z, Axees::Z);

			return *this;
		}

		Matrix44<TYPE>& Scale(const Vector3<TYPE>& aScaleVector)
		{
			myRightVector *= aScaleVector.x;
			myUpVector *= aScaleVector.y;
			myForwardVector *= aScaleVector.z;

			return *this;
		}
		Matrix44<TYPE>& SetScale(const Vector3<TYPE>& aScaleVector)
		{
			myRightVector.Normalize();
			myRightVector *= aScaleVector.x;

			myUpVector.Normalize();
			myUpVector *= aScaleVector.y;

			myForwardVector.Normalize();
			myForwardVector *= aScaleVector.z;

			return *this;
		}


		void Move(const Vector3<TYPE>& aDisplacement)
		{
			SetPosition(aDisplacement * GetRotation() + GetPosition());
		}

		Matrix44& LookAt(const Vector3<TYPE>& aLookFrom, const Vector3<TYPE>& aLookTo)
		{
			if(aLookFrom == aLookTo)
			{
				return *this;
			}
			static const CU::Vector3f objectUpVector(0.0f, 1.0f, 0.0f);

			float xSize = myRightVector.Length();
			float ySize = myUpVector.Length();
			float zSize = myForwardVector.Length();

			CU::Vector3f zAxis = aLookTo - aLookFrom;
			zAxis = zAxis.GetNormalized() * zSize;
			CU::Vector3f xAxis = objectUpVector.Cross(zAxis);
			xAxis = xAxis.GetNormalized() * xSize;
			CU::Vector3f yAxis = zAxis.Cross(xAxis);
			yAxis = yAxis.GetNormalized() * ySize;

			m11 = xAxis.x;
			m12 = xAxis.y;
			m13 = xAxis.z;

			m21 = yAxis.x;
			m22 = yAxis.y;
			m23 = yAxis.z;

			m31 = zAxis.x;
			m32 = zAxis.y;
			m33 = zAxis.z;

			return *this;
		}

		Matrix44& LookAt(const Vector3<TYPE>& aLookTo)
		{
			return LookAt(myPosition, aLookTo);
		}

		Matrix44 CreateLookAt(const Vector3<TYPE>& aLookTo)
		{
			Matrix44<TYPE> matrixToReturn = *this;
			matrixToReturn.LookAt(aLookTo);
			return matrixToReturn;
		}

		Matrix44 Lerp(const Matrix44<TYPE>& aInterpolateToMatrix, const TYPE aInterpolatingSpeed) const
		{
			Matrix44 copy = *this;
			copy.Lerp(aInterpolateToMatrix, aInterpolatingSpeed);
			return copy;
		}

		Matrix44& Lerp(const Matrix44<TYPE>& aInterpolateToMatrix, const TYPE aInterpolatingSpeed)
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

			return *this;
		}

		Matrix44& LerpPosition(const Vector3<TYPE>& aInterpolateToPosition, const TYPE aInterpolatingSpeed)
		{
			myPosition.Lerp(aInterpolateToPosition, aInterpolatingSpeed);

			return *this;
		}

		Matrix44 SlerpRotation(const Matrix44& aSlerpTo, const TYPE aSlerpValue) const
		{
			Matrix44 copy = *this;
			copy.SlerpRotation(aSlerpTo, aSlerpValue);
			return copy;
		}

		Matrix44& SlerpRotation(const Matrix44& aSlerpTo, const TYPE aSlerpValue)
		{
			myRightVector.Slerp(aSlerpTo.myRightVector, aSlerpValue);
			myUpVector.Slerp(aSlerpTo.myUpVector, aSlerpValue);
			myForwardVector.Slerp(aSlerpTo.myForwardVector, aSlerpValue);

			return *this;
		}

		void InvertMe()
		{
			InvertMatrix(&m11);
		}

		Matrix44<TYPE> GetInverted() const
		{
			Matrix44<TYPE> matrix44 = *this;
			matrix44.InvertMe();
			return matrix44;
		}

#pragma warning( disable : 4201 ) // nonstandard extension used: nameless struct/union
		union
		{
			struct
			{
				TYPE    m11, m12, m13, m14,
						m21, m22, m23, m24,
						m31, m32, m33, m34,
						m41, m42, m43, m44;
			};

			struct
			{
				__m128 m1, m2, m3, m4;
			};

			struct
			{
				Vector3<TYPE> myRightVector;	TYPE ms14;
				Vector3<TYPE> myUpVector;		TYPE ms24;
				Vector3<TYPE> myForwardVector;	TYPE ms34;
				Vector3<TYPE> myPosition;		TYPE ms44;
			};

			CU::StaticArray<CU::StaticArray<TYPE, NumColumns>, NumRows> my2DArray;

			CU::StaticArray<TYPE, NumElements> myMatrix;
		};
#pragma warning( default : 4201 )

		static const Matrix44 Identity;
		static const Matrix44 Zero;
	};

	template <typename TYPE>
	void Matrix44<TYPE>::SetEulerRotation(const Vector3f& aRotation)
	{
		const CU::Matrix33f mRotationY = CU::Matrix33f::CreateRotateAroundY(aRotation.y);
		const CU::Matrix33f mRotationX = CU::Matrix33f::CreateRotateAroundX(-aRotation.x);
		const CU::Matrix33f mRotationZ = CU::Matrix33f::CreateRotateAroundZ(aRotation.z);
		SetRotation(mRotationZ * mRotationX * mRotationY);
	}

	template <typename TYPE>
	const CU::Vector3f Matrix44<TYPE>::GetEulerRotation() const
	{
		const CU::Matrix33<TYPE> rotation = GetRotation();
		
		return rotation.GetEulerRotation();
	}

	using Matrix44f = Matrix44<float>;

	template<typename TYPE>
	Matrix44<TYPE> operator+(Matrix44<TYPE> aLeft, const Matrix44<TYPE>& aRight)
	{
		return aLeft += aRight;
	}

	template<typename TYPE>
	Matrix44<TYPE> operator-(Matrix44<TYPE> aLeft, const Matrix44<TYPE>& aRight)
	{
		return aLeft -= aRight;
	}

	template<typename TYPE>
	Matrix44<TYPE> operator*(const Matrix44<TYPE>& aLeft, const Matrix44<TYPE>& aRight)
	{
		Matrix44<TYPE> temp; // så att allignment med simd funkar.
		temp = aLeft;
		return temp *= aRight;
	}

	template<typename TYPE>
	Vector4<TYPE> operator*(Vector4<TYPE> aLeft, const Matrix44<TYPE> &aRight)
	{
		Vector4<TYPE> temp;

		temp.x = (aLeft.x * aRight.m11) + (aLeft.y * aRight.m21) + (aLeft.z * aRight.m31) + (aLeft.w * aRight.m41);
		temp.y = (aLeft.x * aRight.m12) + (aLeft.y * aRight.m22) + (aLeft.z * aRight.m32) + (aLeft.w * aRight.m42);
		temp.z = (aLeft.x * aRight.m13) + (aLeft.y * aRight.m23) + (aLeft.z * aRight.m33) + (aLeft.w * aRight.m43);
		temp.w = (aLeft.x * aRight.m14) + (aLeft.y * aRight.m24) + (aLeft.z * aRight.m34) + (aLeft.w * aRight.m44);

		return temp;
	}

	template<typename TYPE>
	Vector3<TYPE> operator*(Vector3<TYPE> aLeftValue, const Matrix44<TYPE>& aRightValue)
	{
		Vector4<TYPE> tempVec(aLeftValue);
		tempVec = tempVec * aRightValue;
		return Vector3<TYPE>(tempVec.x, tempVec.y, tempVec.z);
	}

	template<typename TYPE>
	bool operator ==(const Matrix44<TYPE> &aLeft, const Matrix44<TYPE>& aRight)
	{
		if (aLeft.m11 == aRight.m11 && aLeft.m12 == aRight.m12 && aLeft.m13 == aRight.m13 && aLeft.m14 == aRight.m14 &&
			aLeft.m21 == aRight.m21 && aLeft.m22 == aRight.m22 && aLeft.m23 == aRight.m23 && aLeft.m24 == aRight.m24 &&
			aLeft.m31 == aRight.m31 && aLeft.m32 == aRight.m32 && aLeft.m33 == aRight.m33 && aLeft.m34 == aRight.m34 &&
			aLeft.m41 == aRight.m41 && aLeft.m42 == aRight.m42 && aLeft.m43 == aRight.m43 && aLeft.m44 == aRight.m44)
		{
			return true;
		}
		return false;
	}

	template<typename TYPE>
	bool operator !=(const Matrix44<TYPE>& aLeft, const Matrix44<TYPE>& aRight)
	{
		return (!(aLeft == aRight));
	}

	template<typename TYPE> const Matrix44<TYPE> Matrix44<TYPE>::Zero(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	template<typename TYPE> const Matrix44<TYPE> Matrix44<TYPE>::Identity(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}
