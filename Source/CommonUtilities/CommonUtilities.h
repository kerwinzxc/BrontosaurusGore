#pragma once
//#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <map>
#include <unordered_map>

#ifdef _WIN32
	#include <Windows.h>
#endif

#define MIN(A,B) ((A) < (B)) ? A : B
#define MAX(A,B) ((A) > (B)) ? A : B
#define SWAP(A,B) A^=B^=A^=B
#define RANGE(A,B,C) ((A) < (B)) ? B : ((A) > (C)) ? C : A
#define CLAMP(V,A_MIN,A_MAX) RANGE(V,A_MIN,A_MAX)

#define FLOAT_IS_ZERO(a) abs((a)) <= 1e-10
#define PI 3.14159265f
#define POO 2

#define E_MATH 2,71828

//1337
#define KYLE 1337
//nullptr
#define KEVIN nullptr
#define CARL delete new int
#define MARCUS bool myIsActive

template<typename Key, typename Value>
void MapDelete(std::map < Key, Value> &aMap)
{
	for (std::map<Key, Value>::iterator itr = aMap.begin(); itr != aMap.end(); ++itr)
	{
		delete itr->second;
	}
	aMap.clear();
}

template<typename Key, typename Value>
void UMapDelete(std::unordered_map < Key, Value> &aMap)
{
	for (std::unordered_map<Key, Value>::iterator itr = aMap.begin(); itr != aMap.end(); ++itr)
	{
		delete itr->second;
	}
	aMap.clear();
}

//Returns a Random float between 0 and 1
#define RAND_FLOAT static_cast<float>(rand()) / static_cast<float>(RAND_MAX)
#define RAND_FLOAT_RANGE(LOW, HIGH) (LOW) + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((HIGH)-(LOW))));

#ifndef SAFE_DELETE
#define SAFE_DELETE(POINTER) delete POINTER; POINTER = nullptr
#endif // safe_delete

#define CYCLIC_ERASE(INDEX, VECTOR)									\
				VECTOR[INDEX] = VECTOR[VECTOR.size() - 1];			\
				VECTOR.erase(VECTOR.begin() + VECTOR.size() - 1);

#define DEGREES_TO_RADIANS(DEGREES) DEGREES = ((3.1415/180)  * DEGREES)
#define RADIANS_TO_DEGREES(RADIANS) RADIANS = ((180/3.1415)  * RADIANS)
#define SQUARE(NUMBER) ((NUMBER) * (NUMBER))

//This one is rather slow, use CU::sqrt instead.
#define SQUARE_ROOT(NUMBER)\
	{					\
		double i = 0;	\
		double x1;		\
		double x2;		\
		while((i * i) <= (NUMBER))\
		{						\
			i += 0.1;			\
		}						\
		x1 = i;					\
		for(int j = 0; j < 10; ++j)		\
		{								\
			x2 = (NUMBER);				\
			x2 /= x1;					\
			x2 += x1;					\
			x2 /= 2;					\
			x1 = x2;					\
		}								\
		(NUMBER) = x2;					\
	}									\

#define PAUSE_AND_CLEAR system("pause"); system ("cls");


namespace CU
{
	//float Remap(float value, float from1, float to1, float from2, float to2) 
	//{
	//	return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
	//}
#ifdef _WIN32
	//float inline __declspec (naked) __fastcall Sqrt(double n)
	//{
	//	n;
	//	_asm fld qword ptr[esp + 4]
	//	_asm fsqrt
	//	_asm ret 8
	//}
#else
	float Sqrt(double n)
	{
		SQUARE_ROOT(n);
		return n;
	}
#endif
//	static const DWORD MS_VC_EXCEPTION = 0x406D1388;
//
//#pragma pack(push,8)
//	typedef struct tagTHREADNAME_INFO
//	{
//		DWORD dwType; // Must be 0x1000.
//		LPCSTR szName; // Pointer to name (in user addr space).
//		DWORD dwThreadID; // Thread ID (-1=caller thread).
//		DWORD dwFlags; // Reserved for future use, must be zero.
//	} THREADNAME_INFO;
//#pragma pack(pop)
//
//	static void SetThreadName(DWORD dwThreadID, char* threadName)
//	{
//		THREADNAME_INFO info;
//		info.dwType = 0x1000;
//		info.szName = threadName;
//		info.dwThreadID = dwThreadID;
//		info.dwFlags = 0;
//
//		__try
//		{
//			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
//		}
//		__except (EXCEPTION_EXECUTE_HANDLER)
//		{
//		}
//	}

} 
