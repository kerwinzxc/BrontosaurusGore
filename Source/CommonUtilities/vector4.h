#pragma once

#define self (*this)

namespace CU
{
	template<typename TYPE>
	class Vector2;

	template<typename TYPE>
	class Vector3;

	template<typename TYPE>
	class Vector4
	{
	public:
		Vector4();
		Vector4(const Vector4 &aOther);
		Vector4(const TYPE aX, const TYPE aY, const TYPE aZ, const TYPE aW);

		template<typename U> explicit Vector4(const Vector4<U> &aOther);
		template<typename U> explicit Vector4(const U aX, const U aY, const U aZ, const U aW);

		explicit Vector4(const Vector3<TYPE>& aOther, const TYPE aW = 1.f);
		explicit Vector4(const Vector2<TYPE>& aOther);
		explicit Vector4(const Vector2<TYPE>& aXY, const TYPE aZ, const TYPE aW);
		explicit Vector4(const Vector2<TYPE>& aXY, const Vector2<TYPE> &aZW);

		__forceinline Vector4& operator=(const Vector4& aRight);
		__forceinline Vector4& operator=(const Vector3<TYPE>& aRight);
		__forceinline Vector4& operator=(const Vector2<TYPE>& aRight);

		void Set(const TYPE aX, const TYPE aY, const TYPE aZ, const TYPE aW);

		__forceinline Vector4& operator+=(const Vector3<TYPE>& aRight);
		__forceinline Vector4& operator+=(const Vector4& aRight);
		__forceinline Vector4& operator-=(const Vector4& aRight);
		__forceinline Vector4& operator*=(const Vector4& aRight);
		__forceinline Vector4& operator/=(const Vector4& aRight);
		__forceinline Vector4& operator*=(const TYPE aRight);
		__forceinline Vector4& operator/=(const TYPE aRight);

		__forceinline Vector4 operator+(const Vector4& aRight) const;
		__forceinline Vector4 operator-(const Vector4& aRight) const;
		__forceinline Vector4 operator*(const Vector4& aRight) const;
		__forceinline Vector4 operator/(const Vector4& aRight) const;

		__forceinline Vector4 operator+(const Vector3<TYPE>& aRight) const;
		__forceinline Vector4 operator-(const Vector3<TYPE>& aRight) const;
		__forceinline Vector4 operator*(const TYPE aRight) const;
		__forceinline Vector4 operator/(const TYPE aRight) const;
		__forceinline Vector4 operator-() const;

		__forceinline bool operator==(const Vector4& aRight) const;
		__forceinline bool operator!=(const Vector4& aRight) const;

		TYPE Length() const;
		TYPE Length2() const;
		TYPE Dot(const Vector4& aRight) const;
		Vector4 GetNormalized() const;

		Vector4& Normalize();

		void Print() const;

		union
		{
#pragma warning(disable : 4201) // nonstandard extension used: nameless struct/union
			struct
			{
				TYPE x;
				TYPE y;
				TYPE z;
				TYPE w;
			};

			struct
			{
				TYPE r;
				TYPE g;
				TYPE b;
				TYPE a;
			};

#pragma warning(default : 4201)

			TYPE vector[4];
		};

		static const Vector4 Zero;
		static const Vector4 UnitX;
		static const Vector4 UnitY;
		static const Vector4 UnitZ;
		static const Vector4 UnitW;
		static const Vector4 One;
	};

	using Vector4i = Vector4<int>;
	using Vector4ui = Vector4<unsigned int>;
	using Vector4f = Vector4<float>;

	using Point4i = Vector4<int>;
	using Point4ui = Vector4<unsigned int>;
	using Point4f = Vector4<float>;


	template<typename TYPE> const Vector4<TYPE> Vector4<TYPE>::Zero(0, 0, 0, 0);
	template<typename TYPE> const Vector4<TYPE> Vector4<TYPE>::UnitX(1, 0, 0, 0);
	template<typename TYPE> const Vector4<TYPE> Vector4<TYPE>::UnitY(0, 1, 0, 0);
	template<typename TYPE> const Vector4<TYPE> Vector4<TYPE>::UnitZ(0, 0, 1, 0);
	template<typename TYPE> const Vector4<TYPE> Vector4<TYPE>::UnitW(0, 0, 0, 1);
	template<typename TYPE> const Vector4<TYPE> Vector4<TYPE>::One(1, 1, 1, 1);

	template<typename TYPE>
	__forceinline Vector4<TYPE>& operator*=(const TYPE aLeft, Vector4<TYPE>& aRight)
	{
		return aRight *= aLeft;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE> operator*(const TYPE aLeft, Vector4<TYPE> aRight)
	{
		return aRight *= aLeft;
	}

	template<typename TYPE>
	Vector4<TYPE>::Vector4()
		: x(static_cast<TYPE>(0))
		, y(static_cast<TYPE>(0))
		, z(static_cast<TYPE>(0))
		, w(static_cast<TYPE>(0))
	{
	}

	template<typename TYPE>
	Vector4<TYPE>::Vector4(const Vector4& aOther)
		: x(aOther.x)
		, y(aOther.y)
		, z(aOther.z)
		, w(aOther.w)
	{
	}

	template<typename TYPE>
	Vector4<TYPE>::Vector4(const Vector3<TYPE>& aVector3, const TYPE aW)
		: x(aVector3.x)
		, y(aVector3.y)
		, z(aVector3.z)
		, w(/*static_cast<TYPE>(1)*/aW)
	{
	}

	template<typename TYPE>
	template<typename U>
	Vector4<TYPE>::Vector4(const Vector4<U>& aOther)
		: x(static_cast<TYPE>(aOther.x))
		, y(static_cast<TYPE>(aOther.y))
		, z(static_cast<TYPE>(aOther.z))
		, w(static_cast<TYPE>(aOther.w))
	{
	}

	template<typename TYPE>
	Vector4<TYPE>::Vector4(const TYPE aX, const TYPE aY, const TYPE aZ, const TYPE aW)
		: x(static_cast<TYPE>(aX))
		, y(static_cast<TYPE>(aY))
		, z(static_cast<TYPE>(aZ))
		, w(static_cast<TYPE>(aW))
	{
	}

	template<typename TYPE>
	template<typename U>
	Vector4<TYPE>::Vector4(const U aX, const U aY, const U aZ, const U aW)
		: x(static_cast<TYPE>(aX))
		, y(static_cast<TYPE>(aY))
		, z(static_cast<TYPE>(aZ))
		, w(static_cast<TYPE>(aW))
	{
	}



	template<typename TYPE>
	Vector4<TYPE>::Vector4(const Vector2<TYPE> &aXY)
		: x(aXY.x)
		, y(aXY.y)
		, z(static_cast<TYPE>(0))
		, w(static_cast<TYPE>(1) /* VENNE?! CARL? */)
	{
	}

	template<typename TYPE>
	Vector4<TYPE>::Vector4(const Vector2<TYPE> &aXY, const TYPE aZ, const TYPE aW)
		: x(aXY.x)
		, y(aXY.y)
		, z(aZ)
		, w(aW)
	{
	}

	template<typename TYPE>
	Vector4<TYPE>::Vector4(const Vector2<TYPE> &aXY, const Vector2<TYPE> &aZW)
		: x(aXY.x)
		, y(aXY.y)
		, z(aZW.x)
		, w(aZW.y)
	{
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE>& Vector4<TYPE>::operator=(const Vector4& aRight)
	{
		x = aRight.x;
		y = aRight.y;
		z = aRight.z;
		w = aRight.w;

		return self;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE> &Vector4<TYPE>::operator=(const Vector3<TYPE>& aRight)
	{
		x = aRight.x;
		y = aRight.y;
		z = aRight.z;
		w = static_cast<TYPE>(0);

		return self;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE>& Vector4<TYPE>::operator=(const Vector2<TYPE>& aRight)
	{
		x = aRight.x;
		y = aRight.y;
		z = static_cast<TYPE>(0);
		w = static_cast<TYPE>(0);

		return self;
	}

	template<typename TYPE>
	inline void Vector4<TYPE>::Set(const TYPE aX, const TYPE aY, const TYPE aZ, const TYPE aW)
	{
		x = aX;
		y = aY;
		z = aZ;
		w = aW;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE>& Vector4<TYPE>::operator+=(const Vector4& aRight)
	{
		x += aRight.x;
		y += aRight.y;
		z += aRight.z;
		w += aRight.w;

		return self;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE>& Vector4<TYPE>::operator+=(const Vector3<TYPE>& aRight)
	{
		x += aRight.x;
		y += aRight.y;
		z += aRight.z;
		
		return self;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE>& Vector4<TYPE>::operator-=(const Vector4 &aRight)
	{
		x -= aRight.x;
		y -= aRight.y;
		z -= aRight.z;
		w -= aRight.w;

		return self;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE>& Vector4<TYPE>::operator*=(const Vector4& aRight)
	{
		x *= aRight.x;
		y *= aRight.y;
		z *= aRight.z;
		w *= aRight.w;

		return self;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE>& Vector4<TYPE>::operator/=(const Vector4& aRight)
	{
		x /= aRight.x;
		y /= aRight.y;
		z /= aRight.z;
		w /= aRight.w;

		return self;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE>& Vector4<TYPE>::operator*=(const TYPE aRight)
	{
		x *= aRight;
		y *= aRight;
		z *= aRight;
		w *= aRight;

		return self;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE>& Vector4<TYPE>::operator/=(const TYPE aRight)
	{
		x /= aRight;
		y /= aRight;
		z /= aRight;
		w /= aRight;
		
		return self;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE> Vector4<TYPE>::operator+(const Vector4& aRight) const
	{
		Vector4 sum = self;
		return sum += aRight;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE> Vector4<TYPE>::operator-(const Vector4& aRight) const
	{
		Vector4 difference = self;
		return difference -= aRight;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE> Vector4<TYPE>::operator*(const Vector4& aRight) const
	{
		Vector4 product = self;
		return product *= aRight;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE> Vector4<TYPE>::operator/(const Vector4& aRight) const
	{
		Vector4 quote = self;
		return quote /= aRight;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE> Vector4<TYPE>::operator+(const Vector3<TYPE>& aRight) const
	{
		Vector4 sum = self;
		return sum += aRight;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE> Vector4<TYPE>::operator-(const Vector3<TYPE>& aRight) const
	{
		Vector4 difference = self;
		return difference -= aRight;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE> Vector4<TYPE>::operator*(const TYPE aRight) const
	{
		Vector4 product = self;
		return product *= aRight;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE> Vector4<TYPE>::operator/(const TYPE aRight) const
	{
		Vector4 quote = self;
		return quote /= aRight;
	}

	template<typename TYPE>
	__forceinline Vector4<TYPE> Vector4<TYPE>::operator-() const
	{
		return Vector4(-x, -y, -z, -w);
	}

	template<typename TYPE>
	__forceinline bool Vector4<TYPE>::operator==(const Vector4& aRight) const
	{
		return (x == aRight.x && y == aRight.y && z == aRight.z && w == aRight.w);
	}

	template<typename TYPE>
	__forceinline bool Vector4<TYPE>::operator!=(const Vector4& aRight) const
	{
		return !(self == aRight);
	}

	template<typename TYPE>
	TYPE Vector4<TYPE>::Length() const
	{
		return sqrt(Length2());
	}

	template<typename TYPE>
	TYPE Vector4<TYPE>::Length2() const
	{
		return ((x * x) + (y * y) + (z * z) + (w * w));
	}

	template<typename TYPE>
	TYPE Vector4<TYPE>::Dot(const Vector4& aRight) const
	{
		return ((x * aRight.x) + (y * aRight.y) + (z * aRight.z) + (w * aRight.w));
	}

	template<typename TYPE>
	Vector4<TYPE> Vector4<TYPE>::GetNormalized() const
	{
		Vector4<TYPE> normalized = *this;
		return normalized.Normalize();
	}

	template<typename TYPE>
	Vector4<TYPE>& Vector4<TYPE>::Normalize()
	{
		TYPE length = Length();

		if (length > 0)
		{
			x /= length;
			y /= length;
			z /= length;
			w /= length;
		}

		return self;
	}

	template<typename TYPE>
	inline void Vector4<TYPE>::Print() const
	{
#ifdef DL_PRINT
		DL_PRINT("(%f, %f, %f, %f)", x, y, z, w);
#endif // DL_PRINT
	}
}

#undef self