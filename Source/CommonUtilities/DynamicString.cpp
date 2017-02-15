#include "stdafx.h"
#ifdef new
#undef new
#endif // new
#include "DynamicString.h"

#include <string>

#define self (*this)

const int MaxAllowedStringSize = 512;
const int CharacterCount = 256;
const int WideCharacterCount = 512;

using namespace CU;

const int DynamicString::FoundNone = -1;

DynamicString::DynamicString()
{
	myCapacity = 16;
	myString = new char[myCapacity] {};
	myString[0] = '\0';
	mySize = 0;
}

DynamicString::DynamicString(const int aCapacity)
{
	myCapacity = aCapacity;
	myString = new char[myCapacity];
	myString[0] = '\0';
	mySize = 0;
}

DynamicString::~DynamicString()
{
	if (myString != nullptr)
	{
		delete[] myString;
		myString = nullptr;
		mySize = 0;
		myCapacity = 0;
	}
}

DynamicString::DynamicString(const DynamicString& aString)
{
	InternalInit();
	self = aString;
}

DynamicString::DynamicString(DynamicString&& aString)
{
	myString = aString.myString;
	aString.myString = nullptr;
	
	mySize = aString.mySize;
	myCapacity = aString.myCapacity;
}

DynamicString::DynamicString(const char* aLiteralString)
{
	InternalInit();
	self = aLiteralString;
}

DynamicString::DynamicString(const char aCharacter, const int aSize)
{
	myCapacity = aSize + 1;
	myString = new char[myCapacity];

	for (int i = 0; i < aSize; ++i)
	{
		myString[i] = aCharacter;
	}

	mySize = aSize;
	myString[aSize] = '\0';
}

DynamicString& CU::DynamicString::operator=(DynamicString&& aString)
{
	if (myString != nullptr)
	{
		delete[] myString;
	}

	myString = aString.myString;
	aString.myString = nullptr;

	mySize = aString.mySize;
	aString.mySize = 0;

	myCapacity = aString.myCapacity;
	aString.myCapacity = 0;

	return self;
}

DynamicString& DynamicString::operator=(const DynamicString& aString)
{
	return CopyString(aString.c_str(), aString.Size());
}

DynamicString& DynamicString::operator=(const char* aLiteralString)
{
	if (aLiteralString == nullptr)
	{
		return self;
	}

	int size = CountLiteralStringLength(aLiteralString);

	return CopyString(aLiteralString, size);
}

DynamicString& DynamicString::operator+=(const DynamicString& aString)
{
	return AddLiteralString(aString.c_str(), aString.Size());
}

DynamicString& DynamicString::operator+=(const char* aLiteralString)
{
	int size = CountLiteralStringLength(aLiteralString);
	return AddLiteralString(aLiteralString, size);
}

DynamicString& DynamicString::operator+=(const char aCharacter)
{
	if (mySize + 1 > myCapacity - 1)
	{
		Grow(mySize + 2); // got heap corruption when just growing with mysize + 1, don't know why :(
	}

	myString[mySize] = aCharacter;
	myString[++mySize] = '\0';

	return self;
}

DynamicString& DynamicString::operator+=(const float aFloat)
{
	return self += std::to_string(aFloat).c_str();
}

DynamicString& DynamicString::operator+=(const double aDouble)
{
	return self += std::to_string(aDouble).c_str();
}

DynamicString& DynamicString::operator+=(const unsigned int aUInt)
{
	return self += std::to_string(aUInt).c_str();
}

DynamicString& DynamicString::operator+=(const int aInt)
{
	return self += std::to_string(aInt).c_str();
}

DynamicString DynamicString::operator+(const DynamicString& aString) const
{
	return DynamicString(self) += aString;
}

DynamicString DynamicString::operator+(const char* aLiteralString) const
{
	return DynamicString(self) += aLiteralString;
}

DynamicString DynamicString::operator+(const char aCharacter) const
{
	return DynamicString(self) += aCharacter;
}

DynamicString DynamicString::operator+(const float aFloat) const
{
	return DynamicString(self) += aFloat;
}

DynamicString DynamicString::operator+(const double aDouble) const
{
	return DynamicString(self) += aDouble;
}

DynamicString DynamicString::operator+(const unsigned int aUInt) const
{
	return DynamicString(self) += aUInt;
}

DynamicString DynamicString::operator+(const int aInt) const
{
	return DynamicString(self) += aInt;
}

bool DynamicString::operator==(const DynamicString& aString) const
{
	if (self.Size() == aString.Size())
	{
		return self == aString.c_str();
	}

	return false;
}

bool DynamicString::operator==(const char* aLiteralString) const
{
	if (mySize == std::strlen(aLiteralString))
	{
		for (int i = 0; i < mySize; ++i)
		{
			if (myString[i] != aLiteralString[i])
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

bool CU::DynamicString::operator==(const char aCharacter) const
{
	return mySize == 1 && myString[0] == aCharacter;
}

int DynamicString::Find(const DynamicString& aSubString) const
{
	if (aSubString.Size() < self.Size())
	{
		int table[CharacterCount];
		PreprocessBoyerMoore(table, CharacterCount, aSubString);
		int jump = 0;

		while (Size() - jump >= aSubString.Size())
		{
			for (int i = aSubString.Size() - 1; self.at(jump + i) == aSubString[i]; --i)
			{
				if (i == 0)
				{
					return jump;
				}
			}

			jump += table[self.at(jump + aSubString.Size() - 1)];
		}
	}
	else if (aSubString == self)
	{
		return 0;
	}

	return FoundNone;
}

int DynamicString::Find(const char* aSubString, const int aSubStringSize) const
{
	if (aSubStringSize > Size())
	{
		return FoundNone;
	}

	return Find(aSubString);
}

int DynamicString::Find(const char* aSubString) const
{
	return Find(DynamicString(aSubString));
}

int DynamicString::FindFirst(const char aCharacter) const
{
	for (int i = 0; i < Size(); ++i)
	{
		if (myString[i] == aCharacter)
		{
			return i;
		}
	}

	return FoundNone;
}

int DynamicString::FindLast(const char aCharacter) const
{
	for (int i = Size() - 1; i >= 0; --i)
	{
		if (myString[i] == aCharacter)
		{
			return i;
		}
	}

	return FoundNone;
}

DynamicString DynamicString::SubStr(const int aIndex, const int aLength) const
{
	DynamicString string;
	for (int i = aIndex; i < aIndex + aLength; ++i)
	{
		string += c_str()[i];
	}

	return string;
}

bool DynamicString::IsInt() const
{
	assert(myString != nullptr);

	const CU::DynamicString numbers = "1234567890-";

	for (int i = 0; i < mySize; ++i)
	{
		if (numbers.FindFirst(at(i)) == FoundNone)
		{
			return false;
		}
	}

	return true;
}

int CU::DynamicString::AsInteger() const
{
	assert(myString != nullptr);
	return std::atoi(myString);
}

bool DynamicString::IsFloat() const
{
	assert(myString != nullptr);

	const CU::DynamicString numbers = "1234567890.-";

	for (int i = 0; i < mySize; ++i)
	{
		if (numbers.FindFirst(at(i)) == FoundNone)
		{
			return false;
		}
	}

	return true;
}

double DynamicString::AsFloat() const
{
	assert(IsFloat() == true);
	return  std::atof(myString);
}

bool DynamicString::IsBool() const
{
	if (*this == "true" || *this == "false")
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DynamicString::AsBool() const
{
	assert(IsBool() == true);

	return *this == "true";
}

void DynamicString::InternalInit()
{
	myCapacity = 0;
	mySize = 0;
	myString = nullptr;
}

void DynamicString::Grow(const int aNewCapacity)
{
	char* newString = new char[aNewCapacity];

	for (int i = 0; i < mySize; ++i)
	{
		newString[i] = myString[i];
	}

	newString[mySize] = '\0';
	myCapacity = aNewCapacity;

	delete[] myString;
	myString = newString;
	newString = nullptr;
}

DynamicString& CU::DynamicString::CopyString(const char* aLiteralString, const int aSize)
{
	if (aSize == FoundNone)
	{
		DL_PRINT_WARNING("Tried to assign invalid c-string to DynamicString");
		return self;
	}

	if (aSize >= myCapacity - 1)
	{
		if (myString != nullptr)
		{
			delete[] myString;
		}

		myCapacity = aSize + 1;
		myString = new char[myCapacity];
	}

	for (int i = 0; i < aSize; ++i)
	{
		myString[i] = aLiteralString[i];
	}

	myString[aSize] = '\0';
	mySize = aSize;

	return self;
}

DynamicString& CU::DynamicString::AddLiteralString(const char* aLiteralString, const int aSize)
{
	if (aSize == FoundNone)
	{
		DL_PRINT_WARNING("Tried to assign invalid c-string to DynamicString");
		return self;
	}

	if (aSize + mySize > myCapacity - 1)
	{
		Grow(aSize + mySize + 1);
	}

	for (int i = mySize; i < aSize + mySize; ++i)
	{
		myString[i] = aLiteralString[i - mySize];
	}

	mySize += aSize;
	myString[mySize] = '\0';

	return self;
}

void DynamicString::PreprocessBoyerMoore(int aTable[], const int aTableSize, const DynamicString & aSubString)
{
	for (int i = 0; i < aTableSize; ++i)
	{
		aTable[i] = aSubString.Size();
	}
	for (int i = 0; i < aSubString.Size() - 1; ++i)
	{
		aTable[aSubString[i]] = aSubString.Size() - 1 - i;
	}
}

int DynamicString::CountLiteralStringLength(const char* aLiteralString)
{
	if (aLiteralString == nullptr)
	{
		return FoundNone;
	}

	for (int length = 0; length < MaxAllowedStringSize; ++length)
	{
		if (aLiteralString[length] == '\0')
		{
			return length;
		}
	}

	return FoundNone;
}

std::ostream& operator<<(std::ostream& os, const DynamicString& aString)
{
	os << aString.c_str();

	return os;
}
