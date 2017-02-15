#pragma once

#define self (*this)

namespace CU
{
	template <typename TYPE>
	class Vector3;

	template<typename TYPE>
	class Vector2
	{
	public:
		Vector2();
		Vector2(const Vector2& aVector2);
		Vector2(const TYPE aX, const TYPE aY);
		inline Vector2(const Vector3<TYPE>& aVector3);
		template<typename U> explicit Vector2(const Vector2<U>& aVector2);
		template<typename U> explicit Vector2(const U aX, const U aY);

		inline Vector2& operator=(const Vector2& aRight);
		template<typename U> inline Vector2& operator=(const Vector2<U>& aRight);

		inline void Set(const TYPE aX, const TYPE aY);
		template<typename U> inline void Set(const U aX, const U aY);

		__forceinline Vector2& operator+=(const Vector2& aRight);
		__forceinline Vector2& operator-=(const Vector2& aRight);
		__forceinline Vector2& operator*=(const Vector2& aRight);
		__forceinline Vector2& operator/=(const Vector2& aRight);

		__forceinline Vector2 operator+(const Vector2& aRight) const;
		__forceinline Vector2 operator-(const Vector2& aRight) const;
		__forceinline Vector2 operator*(const Vector2& aRight) const;
		__forceinline Vector2 operator/(const Vector2& aRight) const;

		__forceinline Vector2 operator-() const;

		__forceinline Vector2& operator*=(const TYPE aRight);
		__forceinline Vector2& operator/=(const TYPE aRight);

		__forceinline Vector2 operator*(const TYPE aRight) const;
		__forceinline Vector2 operator/(const TYPE aRight) const;

		__forceinline bool operator==(const Vector2& aRight) const;
		__forceinline bool operator!=(const Vector2& aRight) const;
		
		inline Vector2 Abs() const;
		inline TYPE Length() const;
		inline TYPE Length2() const;
		inline TYPE Dot(const Vector2& aRight) const;
		 
		inline TYPE Cross(Vector2<TYPE> aVector) const;
		inline Vector2 GetNormalized() const;
		inline Vector2& Normalize();
		inline Vector2& Saturate();
		inline Vector2& Clamp(const Vector2& aLowest, const Vector2& aHighest);

		inline void Print() const;

		union
		{
#pragma warning(disable : 4201) // nonstandard extension used: nameless struct/union
			struct 
			{
				TYPE x;
				TYPE y;
			};
#pragma warning(default : 4201)

			TYPE vector[2];
		};

		static const Vector2 Zero;
		static const Vector2 UnitX;
		static const Vector2 UnitY;
		static const Vector2 One;
	};

	using Vector2c = Vector2<char>;
	using Vector2i = Vector2<int>;
	using Vector2ui = Vector2<unsigned int>;
	using Vector2f = Vector2<float>;
	using Vector2d = Vector2<double>;

	using Point2c = Vector2<char>;
	using Point2i = Vector2<int>;
	using Point2ui = Vector2<unsigned int>;
	using Point2f = Vector2<float>;
	using Point2d = Vector2<double>;

	template<typename TYPE> using Point2 = Vector2<TYPE>;

	template<typename TYPE> const Vector2<TYPE> Vector2<TYPE>::Zero(0, 0);
	template<typename TYPE> const Vector2<TYPE> Vector2<TYPE>::UnitX(1, 0);
	template<typename TYPE> const Vector2<TYPE> Vector2<TYPE>::UnitY(0, 1);
	template<typename TYPE> const Vector2<TYPE> Vector2<TYPE>::One(1, 1);

	template<typename TYPE>
	__forceinline Vector2<TYPE>& operator*=(const TYPE aLeft, Vector2<TYPE>& aRight)
	{
		return aRight *= aLeft;
	}

	template<typename TYPE>
	__forceinline Vector2<TYPE> operator*(const TYPE aLeft, Vector2<TYPE> aRight)
	{
		return aRight *= aLeft;
	}

	template<typename TYPE>
	Vector2<TYPE>::Vector2() : x(static_cast<TYPE>(0)), y(static_cast<TYPE>(0))
	{
	}

	template<typename TYPE>
	Vector2<TYPE>::Vector2(const Vector2& aVector2) : x(aVector2.x), y(aVector2.y)
	{
	}

	template<typename TYPE>
	Vector2<TYPE>::Vector2(const TYPE aX, const TYPE aY) : x(aX), y(aY)
	{
	}

	template<typename TYPE>
	Vector2<TYPE>::Vector2(const Vector3<TYPE>& aVector3) : x(aVector3.x), y(aVector3.y)
	{
	}

	template<typename TYPE>
	template<typename U>
	Vector2<TYPE>::Vector2(const Vector2<U>& aVector) : x(static_cast<TYPE>(aVector.x)), y(static_cast<TYPE>(aVector.y))
	{
	}

	template<typename TYPE>
	template<typename U>
	Vector2<TYPE>::Vector2(const U aX, const U aY) : x(static_cast<TYPE>(aX)), y(static_cast<TYPE>(aY))
	{
	}

	template<typename TYPE>
	__forceinline Vector2<TYPE>& Vector2<TYPE>::operator=(const Vector2& aRight)
	{
		x = aRight.x;
		y = aRight.y;

		return self;
	}

	template<typename TYPE>
	template<typename U>
	__forceinline Vector2<TYPE>& Vector2<TYPE>::operator=(const Vector2<U>& aRight)
	{
		x = static_cast<TYPE>(aRight.x);
		y = static_cast<TYPE>(aRight.y);

		return self;
	}

	template<typename TYPE>
	template<typename U>
	inline void Vector2<TYPE>::Set(const U aX, const U aY)
	{
		x = static_cast<TYPE>(aX);
		y = static_cast<TYPE>(aY);
	}

	template<typename TYPE>
	inline void Vector2<TYPE>::Set(const TYPE aX, const TYPE aY)
	{
		x = aX;
		y = aY;
	}

	template<typename TYPE>
	__forceinline Vector2<TYPE>& Vector2<TYPE>::operator+=(const Vector2& aRight)
	{
		x += aRight.x;
		y += aRight.y;

		return self;
	}

	template<typename TYPE>
	__forceinline Vector2<TYPE>& Vector2<TYPE>::operator-=(const Vector2& aRight)
	{
		x -= aRight.x;
		y -= aRight.y;

		return self;
	}


	template<typename TYPE>
	__forceinline Vector2<TYPE>& Vector2<TYPE>::operator*=(const Vector2& aRight)
	{
		x *= aRight.x;
		y *= aRight.y;

		return self;
	}

	template<typename TYPE>
	__forceinline Vector2<TYPE>& Vector2<TYPE>::operator/=(const Vector2& aRight)
	{
		x /= aRight.x;
		y /= aRight.y;

		return self;
	}

	template<typename TYPE>
	__forceinline Vector2<TYPE> Vector2<TYPE>::operator+(const Vector2& aRight) const
	{
		Vector2 left = self;
		return left += aRight;
	}

	template<typename TYPE>
	__forceinline Vector2<TYPE> Vector2<TYPE>::operator-(const Vector2& aRight) const
	{
		Vector2 left = self;
		return left -= aRight;
	}

	template<typename TYPE>
	__forceinline Vector2<TYPE> Vector2<TYPE>::operator*(const Vector2& aRight) const
	{
		Vector2 left = self;
		return left *= aRight;
	}

	template<typename TYPE>
	__forceinline Vector2<TYPE> Vector2<TYPE>::operator/(const Vector2& aRight) const
	{
		Vector2 left = self;
		return left /= aRight;
	}

	template<typename TYPE>
	__forceinline Vector2<TYPE> Vector2<TYPE>::operator-() const
	{
		Vector2 right = self;
		
		right.x = -right.x;
		right.y = -right.y;

		return right;
	}

	template<typename TYPE>
	__forceinline Vector2<TYPE>& Vector2<TYPE>::operator*=(const TYPE aRight)
	{
		x *= aRight;
		y *= aRight;

		return self;
	}

	template<typename TYPE>
	__forceinline Vector2<TYPE>& Vector2<TYPE>::operator/=(const TYPE aRight)
	{
		x /= aRight;
		y /= aRight;

		return self;
	}

	template<typename TYPE>
	__forceinline Vector2<TYPE> Vector2<TYPE>::operator*(const TYPE aRight) const
	{
		Vector2 left = self;
		left *= aRight;

		return left;
	}

	template<typename TYPE>
	__forceinline Vector2<TYPE> Vector2<TYPE>::operator/(const TYPE aRight) const
	{
		Vector2 left = self;
		left /= aRight;

		return left;
	}

	template<typename TYPE>
	__forceinline bool Vector2<TYPE>::operator==(const Vector2& aRight) const
	{
		return (x == aRight.x) && (y == aRight.y);
	}

	template<typename TYPE>
	__forceinline bool Vector2<TYPE>::operator!=(const Vector2& aRight) const
	{
		return !(self == aRight);
	}

	template<typename TYPE>
	inline Vector2<TYPE> Vector2<TYPE>::Abs() const
	{
		return Vector2(std::abs(x), std::abs(y));
	}

	template<typename TYPE>
	TYPE Vector2<TYPE>::Length() const
	{
		return sqrt(Length2());
	}

	template<typename TYPE>
	TYPE Vector2<TYPE>::Length2() const
	{
		return (x * x) + (y * y);
	}

	template<typename TYPE>
	TYPE Vector2<TYPE>::Dot(const Vector2<TYPE>& aRight) const
	{
		return (x * aRight.x) + (y * aRight.y);
	}

	template<typename TYPE>
	TYPE Vector2<TYPE>::Cross(Vector2<TYPE> aVector) const
	{
		return (x * aVector.y - y * aVector.x);
	}

	template<typename TYPE>
	Vector2<TYPE> Vector2<TYPE>::GetNormalized() const
	{
		Vector2 normalized = self;
		normalized.Normalize();

		return normalized;
	}

	template<typename TYPE>
	Vector2<TYPE>& Vector2<TYPE>::Normalize()
	{
		TYPE length = Length();
		if (length != static_cast<TYPE>(0))
		{
			self /= length;
		}

		return self;
	}

	template<typename TYPE>
	inline Vector2<TYPE>& Vector2<TYPE>::Saturate()
	{
		return Clamp(Zero, One);
	}

	template<typename TYPE>
	inline Vector2<TYPE>& Vector2<TYPE>::Clamp(const Vector2& aLowest, const Vector2& aHighest)
	{
		if (x < aLowest.x) x = aLowest.x;
		else if (x > aHighest.x) x = aHighest.x;
		if (y < aLowest.y) y = aLowest.y;
		else if (y > aHighest.y) y = aHighest.y;

		return self;
	}

	template<typename TYPE>
	inline void Vector2<TYPE>::Print() const
	{
#ifdef DL_PRINT
		DL_PRINT("(%f, %f)", x, y);
#endif // DL_PRINT
	}
}

#undef self