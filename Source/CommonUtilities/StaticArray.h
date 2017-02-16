#pragma once
#pragma message("Static array compiled")
#include <assert.h>
#include <cstring>

namespace CU
{
	template <typename Type, int ArraySize>
	class StaticArray
	{
	public:
		using iterator = Type*;
		using const_iterator = const Type*;

		StaticArray();
		StaticArray(const StaticArray& aStaticArray);
		StaticArray(Type (&aArray)[ArraySize]);
		StaticArray(const Type& aObject);
		StaticArray(std::nullptr_t);
		~StaticArray();

		StaticArray& operator=(const StaticArray& aStaticArray);

		inline const Type& operator[](const int aIndex) const;
		inline Type& operator[](const int aIndex);

		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;

		inline int Find(const Type& aObject) const;
		inline const void* AsVoidPointer() const;

		inline void Insert(int aIndex,Type& aObject);
		inline void DeleteAll();

		__forceinline int ByteSize() const;
		__forceinline int Size() const;

		static const int FoundNone = -1;

	private:
		Type myStaticArray[ArraySize];

	};

	template <typename Type, int ArraySize>
	StaticArray<Type, ArraySize>::StaticArray()
	{
	}

	template <typename Type, int ArraySize>
	StaticArray<Type, ArraySize>::StaticArray(const StaticArray& aStaticArray)
	{
		for (int i = 0; i < ArraySize; ++i)
		{
			myStaticArray[i] = aStaticArray[i];
		}
	}

	template<typename Type, int ArraySize>
	inline StaticArray<Type, ArraySize>::StaticArray(Type (&aArray)[ArraySize])
	{
		for (int i = 0; i < ArraySize; ++i)
		{
			myStaticArray[i] = aArray[i];
		}
	}

	template<typename Type, int ArraySize>
	inline StaticArray<Type, ArraySize>::StaticArray(const Type& aObject)
	{
		for (int i = 0; i < ArraySize; ++i)
		{
			myStaticArray[i] = aObject;
		}
	}

	template<typename Type, int ArraySize>
	inline StaticArray<Type, ArraySize>::StaticArray(std::nullptr_t)
	{
		for (int i = 0; i < ArraySize; ++i)
		{
			myStaticArray[i] = nullptr;
		}
	}

	template <typename Type, int ArraySize>
	StaticArray<Type, ArraySize>::~StaticArray()
	{
	}

	template <typename Type, int ArraySize>
	StaticArray<Type, ArraySize>& StaticArray<Type, ArraySize>::operator=(const StaticArray& aStaticArray)
	{
		for (int i = 0; i < ArraySize; ++i)
		{
			myStaticArray[i] = aStaticArray[i];
		}
		return *this;
	}

	template <typename Type, int ArraySize>
	inline Type& StaticArray<Type, ArraySize>::operator[](const int aIndex)
	{
		assert((aIndex >= 0 && aIndex < ArraySize) && "Index out of bounds!");
		return myStaticArray[aIndex];
	}

	template <typename Type, int ArraySize>
	inline const Type& StaticArray<Type, ArraySize>::operator[](const int aIndex) const
	{
		assert((aIndex >= 0 && aIndex < ArraySize) && "Index out of bounds!");
		return myStaticArray[aIndex];
	}

	template <typename Type, int ArraySize>
	inline typename StaticArray<Type, ArraySize>::iterator StaticArray<Type, ArraySize>::begin()
	{
		return myStaticArray;
	}

	template <typename Type, int ArraySize>
	inline typename StaticArray<Type, ArraySize>::const_iterator StaticArray<Type, ArraySize>::begin() const
	{
		return myStaticArray;
	}

	template <typename Type, int ArraySize>
	inline typename StaticArray<Type, ArraySize>::iterator StaticArray<Type, ArraySize>::end()
	{
		return (myStaticArray + ArraySize);
	}

	template <typename Type, int ArraySize>
	inline typename StaticArray<Type, ArraySize>::const_iterator StaticArray<Type, ArraySize>::end() const
	{
		return (myStaticArray + ArraySize);
	}

	template<typename Type, int ArraySize>
	inline int StaticArray<Type, ArraySize>::Find(const Type& aObject) const
	{
		for (int i = 0; i < ArraySize; ++i)
		{
			if (myStaticArray[i] == aObject)
			{
				return i;
			}
		}

		return FoundNone;
	}

	template<typename Type, int ArraySize>
	inline const void * StaticArray<Type, ArraySize>::AsVoidPointer() const
	{
		return static_cast<const void*>(myStaticArray);
	}

	template <typename Type, int ArraySize>
	inline void StaticArray<Type, ArraySize>::Insert(int aIndex, Type& aObject)
	{
		assert((aIndex >= 0 && aIndex < ArraySize) && "Index out of bounds!");

		for (int i = ArraySize - 1; i > aIndex; --i)
		{
			myStaticArray[i] = myStaticArray[i - 1];
		}

		myStaticArray[aIndex] = aObject;
	}

	template <typename Type, int ArraySize>
	inline void StaticArray<Type, ArraySize>::DeleteAll()
	{
		for (int i = 0; i < ArraySize; ++i)
		{
			delete myStaticArray[i];
			myStaticArray[i] = nullptr;
		}
	}

	template<typename Type, int ArraySize>
	inline int StaticArray<Type, ArraySize>::ByteSize() const
	{
		return ArraySize * sizeof(Type);
	}

	template<typename Type, int ArraySize>
	inline int StaticArray<Type, ArraySize>::Size() const
	{
		return ArraySize;
	}
}
