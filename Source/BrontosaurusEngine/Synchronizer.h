#pragma once

//#include <GrowingArray.h>
//#include <StaticArray.h>
#include <mutex>
#include <cstdint>


#define BUFFER_SIZE 512

template<typename type, typename sizeType = std::uint32_t>
class CSynchronizer
{
public:
	using size_type = sizeType;

private:
	enum eBufferIndex : char
	{
		eFirst,
		eSecond,
		eThird,
		eSize
	};

public:
	CSynchronizer();
	~CSynchronizer();

	inline type operator[](const size_type aIndex);
	inline void operator <<(type aObject);
	inline operator int()const;
	inline size_type operator!() const;
	inline void SwapWrite();
	inline void SwapRead();
	inline void ClearWrite();
	inline void ClearAll();

private:
	CU::StaticArray<CU::GrowingArray<type, size_type>, eSize> myBuffers; //TODO: Fixa staticArray sizetype?

	std::mutex mySwapMutex;

	eBufferIndex myWriteTo;
	eBufferIndex myReadFrom;
	eBufferIndex myFreeBuffer;

	volatile bool myHasFresh;
};


template<typename type, typename sizeType>
inline void CSynchronizer<type, sizeType>::ClearAll()
{
	mySwapMutex.lock();

	for (char i = 0; i < myBuffers.Size(); ++i)
	{
		myBuffers[i].DeleteAll();
	}

	mySwapMutex.unlock();
}

template<typename type, typename sizeType>
inline CSynchronizer<type, sizeType>::CSynchronizer()
{
	myBuffers[eFirst].Init(BUFFER_SIZE);
	myBuffers[eSecond].Init(BUFFER_SIZE);
	myBuffers[eThird].Init(BUFFER_SIZE);

	myWriteTo = eFirst;
	myReadFrom = eSecond/*eFirst*/;
	myFreeBuffer = eThird;

	myHasFresh = false;
}

template<typename type, typename sizeType>
inline CSynchronizer<type, sizeType>::~CSynchronizer()
{
	ClearAll();
}

template<typename type, typename sizeType>
type CSynchronizer<type, sizeType>::operator[](const size_type aIndex)
{
	return myBuffers[myReadFrom][aIndex];
}

template<typename type, typename sizeType>
inline void CSynchronizer<type, sizeType>::operator<<(type aObject)
{
	myBuffers[myWriteTo].Add(aObject);
}

template<typename type, typename sizeType>
inline sizeType CSynchronizer<type, sizeType>::operator!() const
{
	return myBuffers[myReadFrom].Size();
}


template<typename type, typename sizeType /*= std::uint32_t*/>
CSynchronizer<type, sizeType>::operator int() const
{
	return myBuffers[myReadFrom].Size();
}

template<typename type, typename sizeType>
inline void CSynchronizer<type, sizeType>::SwapWrite()
{
	mySwapMutex.lock();

	eBufferIndex lastFree = myFreeBuffer;
	myFreeBuffer = myWriteTo;
	myWriteTo = lastFree;

	myHasFresh = true;

	mySwapMutex.unlock();

	ClearWrite();

}

template<typename type, typename sizeType>
inline void CSynchronizer<type, sizeType>::SwapRead()
{
	if (myHasFresh == false)
	{
		return;
	}
	
	mySwapMutex.lock();

	myHasFresh = false;

	//// MemoryPoolio this!
	//myBuffers[myReadFrom].DeleteAll();

	eBufferIndex lastFree = myFreeBuffer;
	myFreeBuffer = myReadFrom;
	myReadFrom = lastFree;
	//DL_PRINT("Read buffer: %d", myReadFrom);


	mySwapMutex.unlock();
}

template<typename type, typename sizeType>
inline void CSynchronizer<type, sizeType>::ClearWrite()
{
	myBuffers[myWriteTo].DeleteAll();
}
