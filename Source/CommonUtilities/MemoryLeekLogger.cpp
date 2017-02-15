#include "stdafx.h"

#ifdef new
#undef new
#endif // new

#ifdef WATCH_THE_MEMORY_MKAY
#ifndef _RETAIL_BUILD

#include <cstdlib>
#include <map>
#include <string>
#include <mutex>

#define min(a,b) (((a) < (b)) ? (a) : (b))

void operator delete(void* aObject, const char*, int)
{
	free(aObject);
}

const unsigned int MAX_MEMORYLEEK_STRING_SIZE = 128u;

struct MemoryLeekData 
{
	struct FileName
	{
		FileName();
		FileName(const char* aFileName);
		~FileName();

		char* myString;
		int myID;

	private:
		static std::map<std::string, int> ourStringIDs;
		static CU::GrowingArray<char*, int> ourStrings;
	};

	MemoryLeekData();
	MemoryLeekData(void* aObject, const char* aFileName, const int aLine);
	MemoryLeekData(MemoryLeekData&& aTemporaryLeek);
	~MemoryLeekData();
	MemoryLeekData& operator=(const MemoryLeekData& aOther);
	MemoryLeekData& operator=(MemoryLeekData&& aTemporaryLeek);

	char myFileName3[MAX_MEMORYLEEK_STRING_SIZE];
	void* myCreatedObject;
	int myLine;
};

static CU::GrowingArray<MemoryLeekData, long long> locMemoryLeeks(4096 * 16);
static std::mutex locMemoryLeekMutex;

void* operator new(size_t aSize, const char* aFileName, const int aLine)
{
	void* newObject = malloc(aSize);

	if (newObject != nullptr)
	{
		locMemoryLeekMutex.lock();
		locMemoryLeeks.Add(MemoryLeekData(newObject, aFileName, aLine));
		locMemoryLeekMutex.unlock();
	}

	return newObject;
}

void operator delete(void* aPtr)
{
	locMemoryLeekMutex.lock();
	for (long long i = 0; i < locMemoryLeeks.Size(); ++i)
	{
		if (locMemoryLeeks[i].myCreatedObject == aPtr)
		{
			locMemoryLeeks.RemoveCyclicAtIndex(i);
			break;
		}
	}
	locMemoryLeekMutex.unlock();

	free(aPtr);
}

#include <cstring>
MemoryLeekData::MemoryLeekData()
	: myCreatedObject(nullptr)
	, myLine(0)
{
}

MemoryLeekData::MemoryLeekData(void* aObject, const char* aFileName, const int aLine)
	: myCreatedObject(aObject)
	, myLine(aLine)
{
	size_t stringLength = std::strlen(aFileName);
	size_t minLength = min(stringLength, MAX_MEMORYLEEK_STRING_SIZE);
	memcpy(myFileName3, aFileName, minLength);
	myFileName3[minLength] = '\0';
}

MemoryLeekData::MemoryLeekData(MemoryLeekData&& aTemporaryLeek)
{
	*this = std::move(aTemporaryLeek);
}

MemoryLeekData::~MemoryLeekData()
{
}

MemoryLeekData& MemoryLeekData::operator=(const MemoryLeekData& aOther)
{
	myCreatedObject = aOther.myCreatedObject;

	memcpy(myFileName3, aOther.myFileName3, MAX_MEMORYLEEK_STRING_SIZE);

	myLine = aOther.myLine;

	return *this;
}

MemoryLeekData& MemoryLeekData::operator=(MemoryLeekData&& aTemporaryLeek)
{
	myCreatedObject = aTemporaryLeek.myCreatedObject;
	aTemporaryLeek.myCreatedObject = nullptr;

	memcpy(myFileName3, aTemporaryLeek.myFileName3, MAX_MEMORYLEEK_STRING_SIZE);

	myLine = aTemporaryLeek.myLine;
	aTemporaryLeek.myLine = 0;

	return *this;
}

MemoryLeekData::FileName::FileName()
	: myString(nullptr)
	, myID(-1)
{
}

MemoryLeekData::FileName::FileName(const char* aFileName)
{
	auto it = ourStringIDs.find(aFileName);
	if (it == ourStringIDs.end())
	{
		const size_t stringLength = std::strlen(aFileName);
		char* newString = new char[stringLength + 1];
		memcpy(newString, aFileName, stringLength);
		newString[stringLength] = '\0';

		ourStringIDs[newString] = ourStrings.Size();
		ourStrings.Add(newString);
	}

	myID = ourStringIDs[aFileName];
	myString = ourStrings[myID];
}

MemoryLeekData::FileName::~FileName()
{
	myString = nullptr;
	myID = -1;
}

std::map<std::string, int> MemoryLeekData::FileName::ourStringIDs;
CU::GrowingArray<char*, int> MemoryLeekData::FileName::ourStrings(256);

#endif // !_RETAIL_BUILD
#endif // WATCH_THE_MEMORY_MKAY

#include <iostream>
void DumpMemoryLeeks()
{
#ifdef WATCH_THE_MEMORY_MKAY
#ifndef _RETAIL_BUILD

	std::ofstream memoryLeekLoggo("memory_leeks.loggo");
	for (long long i = 0; i < locMemoryLeeks.Size(); ++i)
	{
		std::cout
			<< "[" << locMemoryLeeks[i].myCreatedObject << "]" << " not destroyed" << std::endl
			//<< "File: " << locMemoryLeeks[i].myFileName << std::endl
			<< "File: " << locMemoryLeeks[i].myFileName3 << std::endl
			//<< "File: " << locMemoryLeeks[i].myFileName2.myString << std::endl
			<< "Line: " << locMemoryLeeks[i].myLine << std::endl
			<< std::endl;

		memoryLeekLoggo
			<< "[" << locMemoryLeeks[i].myCreatedObject << "]" << " not destroyed" << std::endl
			//<< "File: " << locMemoryLeeks[i].myFileName << std::endl
			<< "File: " << locMemoryLeeks[i].myFileName3 << std::endl
			//<< "File: " << locMemoryLeeks[i].myFileName2.myString << std::endl
			<< "Line: " << locMemoryLeeks[i].myLine << std::endl
			<< std::endl;
	}

	memoryLeekLoggo.close();

	if (locMemoryLeeks.Size() > 0)
	{
		std::cout << locMemoryLeeks.Size() << " objects are not deleted" << std::endl << std::endl;
		system("pause");
	}

	locMemoryLeeks.Destroy();

#endif // !_RETAIL_BUILD
#endif // WATCH_THE_MEMORY_MKAY
}
