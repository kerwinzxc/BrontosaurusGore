#pragma once

#include <assert.h>

//FORWARD DECLERATION BEGIN
//std::string and std::wstring

template<class _Elem, class _Traits, class _Alloc>
class basic_string;

template<class _Elem>
struct char_traits;

template<class _Ty>
class allocator;

typedef basic_string<char, char_traits<char>, allocator<char>> string;
typedef basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t>> wstring;

//FORWARD DECLERATION END

namespace CU
{
	
	class DynamicString
	{
	public:
		DynamicString();
		DynamicString(const int aCapacity);
		DynamicString(const DynamicString& aString);
		DynamicString(DynamicString&& aString);
		DynamicString(const char* aLiteralString);
		DynamicString(const char aCharacter, const int aSize = 1);
		~DynamicString();

		DynamicString& operator=(DynamicString&& aString);
		DynamicString& operator=(const DynamicString& aString);
		DynamicString& operator=(const char* aLiteralString);

		DynamicString& operator+=(const DynamicString& aString);
		DynamicString& operator+=(const char* aLiteralString);
		DynamicString& operator+=(const char aCharacter);
		DynamicString& operator+=(const float aFloat);
		DynamicString& operator+=(const double aDouble);
		DynamicString& operator+=(const unsigned int aUInt);
		DynamicString& operator+=(const int aInt);

		DynamicString operator+(const DynamicString& aString) const;
		DynamicString operator+(const char* aLiteralString) const;
		DynamicString operator+(const char aCharacter) const;
		DynamicString operator+(const float aFloat) const;
		DynamicString operator+(const double aDouble) const;
		DynamicString operator+(const unsigned int aUInt) const;
		DynamicString operator+(const int aInt) const;

		bool operator==(const DynamicString& aString) const;
		bool operator==(const char* aLiteralString) const;
		bool operator==(const char aCharacter) const;

		int Find(const DynamicString& aSubString) const;
		int Find(const char* aSubString, const int aSubStringSize) const;
		int Find(const char* aSubString) const;
		int FindFirst(const char aCharacter) const;
		int FindLast(const char aCharacter) const;

		DynamicString SubStr(const int aIndex, const int aLength) const;

		bool IsInt() const;
		int AsInteger() const;

		bool IsFloat() const;
		double AsFloat() const;

		bool IsBool() const;
		bool AsBool() const;

		inline char& operator[](const int aIndex);
		inline char operator[](const int aIndex) const;
		inline char& at(const int aIndex);
		inline char at(const int aIndex) const;

		__forceinline const char* c_str() const;
		__forceinline int Size() const;
		__forceinline bool Empty() const;

		static const int FoundNone;

	private:
		void InternalInit();
		void Grow(const int aNewCapacity);
		DynamicString& CopyString(const char* aLiteralString, const int aSize);
		DynamicString& AddLiteralString(const char* aLiteralString, const int aSize);

		static void PreprocessBoyerMoore(int aTable[], const int aTableSize, const DynamicString& aSubString);
		static int CountLiteralStringLength(const char* aLiteralString);

		char* myString;
		int mySize;
		int myCapacity;
	};

	inline char & DynamicString::operator[](const int aIndex)
	{
		assert(aIndex >= 0 && aIndex < mySize && "Index is out of range");
		return myString[aIndex];
	}

	inline char DynamicString::operator[](const int aIndex) const
	{
		assert(aIndex >= 0 && aIndex < mySize && "Index is out of range");
		return myString[aIndex];
	}

	inline char & DynamicString::at(const int aIndex)
	{
		assert(aIndex >= 0 && aIndex < mySize && "Index is out of range");
		return myString[aIndex];
	}

	inline char DynamicString::at(const int aIndex) const
	{
		assert(aIndex >= 0 && aIndex < mySize && "Index is out of range");
		return myString[aIndex];
	}

	__forceinline const char* DynamicString::c_str() const
	{
		return myString;
	}

	__forceinline int DynamicString::Size() const
	{
		return mySize;
	}

	__forceinline bool DynamicString::Empty() const
	{
		return mySize == 0;
	}
}

//to use it, include iostream/ostream above dynamicstring
//#ifdef _OSTREAM_
//std::ostream& operator<<(std::ostream& os, const DynamicString& aString);
//#endif
