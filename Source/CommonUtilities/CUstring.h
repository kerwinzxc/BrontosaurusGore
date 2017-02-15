#pragma once
//#include <iostream>
#include "assert.h"

#define STRING_TEMPLATE_ARGS template<unsigned int SIZE>


#ifdef __APPLE__
	#define SPRINTF sprintf
#elif defined _WIN32 || defined _WIN64
	#define SPRINTF sprintf_s
#endif

namespace CU
{
	template <unsigned int SIZE = 30>
	class String
	{
	public:
		
		String();
		
		String(const String& aCopy);
		
		String(const char* aCopy);
	
		~String();
	
		String operator+(const String& aString) const;
	
		String operator+(const float aFloat) const;
	
		String operator+(const double aDouble) const;
	
		String operator+(const int aInt) const;
	
		String operator+(const char* aLiteralString) const;
	
		String& operator+=(const String& aString);
	
		String& operator+=(const float aFloat);
	
		String& operator+=(const double aDouble);
	
		String& operator+=(const int aInt);
	
		String& operator+=(const char* aLiteralString);

		bool operator==(const String& aString) const;

		bool operator!=(const String& aString) const;
	
		void operator=(const String& aString);

		template<unsigned int OtherSize>
		void operator=(const String<OtherSize>& aOtherSizedString) //sorry for defining here, jag vet bara inte hur man gör annars,,, mvh carl
		{
			*this = aOtherSizedString.c_str();
		}
	
		void operator=(const char* aLiteralString);
	
		int Find(const char* aLiteralString) const;
	
		int Find(const String& aString) const;
	
		String SubStr(const int aIndex, const int aLength) const;
	
		const char* c_str() const;
	
		int Size() const;

		int Length() const;
	
		char& operator[](const int aIndex);
	
		char operator[](const int aIndex) const;
	
		friend std::ostream &operator<<(std::ostream &os, const String& aStr)
		{
			return os << aStr.c_str();
		}
	private:
		char myString[SIZE + 1];
		unsigned int myLength;
	};


	STRING_TEMPLATE_ARGS
	String<SIZE>::String()
	{
		myString[0] = '\0';
		myLength = 0;
	}
	
	STRING_TEMPLATE_ARGS
	String<SIZE>::String(const String &aCopy)
	{
		*this = aCopy;
	}
	
	STRING_TEMPLATE_ARGS
	String<SIZE>::String(const char* aCopy)
	{
		*this = aCopy;
	}
	
	STRING_TEMPLATE_ARGS
	String<SIZE>::~String()
	{
		
	}

	
	STRING_TEMPLATE_ARGS
	String<SIZE> String<SIZE>::operator+(const String& aString) const
	{
		return String(*this) += aString;
	}
	
	
	STRING_TEMPLATE_ARGS
	String<SIZE> String<SIZE>::operator+(const float aFloat) const
	{
		return String(*this) += aFloat;
	}
	
	
	STRING_TEMPLATE_ARGS
	String<SIZE> String<SIZE>::operator+(const double aDouble) const
	{
		return String(*this) += aDouble;
	}
	
	STRING_TEMPLATE_ARGS
	String<SIZE> String<SIZE>::operator+(const int aInt) const
	{
		return String(*this) += aInt;
	}
	
	STRING_TEMPLATE_ARGS
	String<SIZE> String<SIZE>::operator+(const char* aLiteralString) const
	{
		return String(*this) += aLiteralString;
	}
	
	STRING_TEMPLATE_ARGS
	String<SIZE>& String<SIZE>::operator+=(const String& aString)
	{
		assert(myLength + aString.Length() < SIZE && "Resulting String To big to fit");
		for (int i = 0; i < aString.Size(); ++i)
		{
			myString[Size()] = aString[i];
			++myLength;
		}
		myString[Size()] = '\0';
		return *this;
	}
	
	STRING_TEMPLATE_ARGS
	String<SIZE>& String<SIZE>::operator+=(const float aFloat)
	{
		char buffer[256];
		SPRINTF(buffer, "%f", aFloat);

		for (int i = 0; i < SIZE; ++i)
		{
			if (buffer[(SIZE - 1) - i] == '\0')
			{
				for (int j = 1; buffer[(SIZE - 1) - (i + j)] == '0'; ++j)
				{
					buffer[(SIZE - 1) - (i + j)] = '\0';
				}
				break;
			}
		}

		return *this += buffer;
	}
	
	STRING_TEMPLATE_ARGS
	String<SIZE>& String<SIZE>::operator+=(const double aDouble)
	{
		char buffer[SIZE];
		SPRINTF(buffer, "%f", aDouble);

		for (int i = 0; i < SIZE; ++i)
		{
			if (buffer[(SIZE - 1) - i] == '\0')
			{
				for (int j = 1; buffer[(SIZE - 1) - (i + j)] == '0'; ++j)
				{
					buffer[(SIZE - 1) - (i + j)] = '\0';
				}
				break;
			}
		}
		return *this += buffer;
	}
	
	STRING_TEMPLATE_ARGS
	String<SIZE>& String<SIZE>::operator+=(const int aInt)
	{
		char intStr[256];
		SPRINTF(intStr, "%i", aInt);
		*this += intStr;
		return *this;
	}
	
	STRING_TEMPLATE_ARGS
	String<SIZE>& String<SIZE>::operator+=(const char* aLiteralString)
	{
		String ltStr(aLiteralString);
		*this += ltStr;
		return *this;
	}
		
	STRING_TEMPLATE_ARGS
	bool String<SIZE>::operator==(const String& aString) const
	{
		if (aString.Size() != Size())
		{
			return false;
		}
		else
		{
			for (int i = 0; i < Size(); ++i)
			{
				if (aString[i] != myString[i] || aString[(aString.Size() - 1 )- i] != myString[(Size() - 1) - i])
				{
					return false;
				}
			}
		}
		return true;
	}

	STRING_TEMPLATE_ARGS
	bool String<SIZE>::operator!=(const String& aString) const
	{
		return !(aString == *this);
	}
	
	STRING_TEMPLATE_ARGS
	void String<SIZE>::operator=(const String& aString)
	{
		myLength = aString.myLength;
		for (int i = 0; aString.myString[i] != '\0'; ++i)
		{
			myString[i] = aString.myString[i];
		}
		myString[Size()] = '\0';
	}
	
	STRING_TEMPLATE_ARGS
	void String<SIZE>::operator=(const char* aLiteralString)
	{
		myLength = 0;
		for (int i = 0; aLiteralString[i] != '\0'; ++i)
		{
			myString[i] = aLiteralString[i];
			++myLength;
		}
		myString[Size()] = '\0';
	}
	
	STRING_TEMPLATE_ARGS
	int String<SIZE>::Find(const char* aLiteralString) const
	{
		return Find(String(aLiteralString));
	}
	
	STRING_TEMPLATE_ARGS
	int String<SIZE>::Find(const String& aString) const
	{
		int T[256];

		for (int i = 0; i < 256; ++i)
		{
			T[i] = aString.Length();
		}

		for (int i = 0; i < aString.Size(); ++i)
		{
			T[aString[i]] = aString.Length() - 1 - i;
		}

		int skip = 0;

		while (Length() - skip >= aString.Length())
		{
			int i = aString.Length() - 1;
			while (myString[skip + i] == aString[i])
			{
				if (i == 0)
				{
					return skip;
				}
				i = i - 1;
			}
			skip = skip + T[myString[skip + aString.Length() - 1]];
		}

		return -1;
	}
	
	STRING_TEMPLATE_ARGS
	String<SIZE> String<SIZE>::SubStr(const int aIndex, const int aLength) const
	{
		char substr[SIZE];
		for (int i = 0; aIndex + i < aIndex + aLength; ++i)
		{
			substr[i] = myString[aIndex + i];
		}
		substr[aLength] = '\0';
		
		return String(substr);
	}
	
	STRING_TEMPLATE_ARGS
	const char* String<SIZE>::c_str() const
	{
		return &myString[0];
	}
	
	STRING_TEMPLATE_ARGS
	int String<SIZE>::Size() const
	{
		return myLength;
	}

	STRING_TEMPLATE_ARGS
		int String<SIZE>::Length() const
	{
		return myLength;
	}
	
	STRING_TEMPLATE_ARGS
	char& String<SIZE>::operator[](const int aIndex)
	{
		assert((aIndex >= 0 && aIndex < Size()) && "Index out of bounds!");
		return myString[aIndex];
	}
	
	STRING_TEMPLATE_ARGS
	char String<SIZE>::operator[](const int aIndex) const
	{
		assert((aIndex >= 0 && aIndex < Size()) && "Index out of bounds!");
		return myString[aIndex];
	}
	
}