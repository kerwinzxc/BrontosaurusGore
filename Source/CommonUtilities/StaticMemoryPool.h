#pragma once
#include "StaticArray.h"
#include "VectorOnStack.h"

#ifndef assert
#include <assert.h>
#endif // !assert

namespace CU
{
	template<typename ObjectType, int ObjectCapacity>
	class CStaticMemoryPool
	{
	public:
		CStaticMemoryPool();
		~CStaticMemoryPool();

		ObjectType* Require();
		bool Recycle(ObjectType* aObject); //Recycle returns false if you try to recycle a nullptr

	private:
		StaticArray<ObjectType, ObjectCapacity> myData;
		VectorOnStack<ObjectType*, ObjectCapacity> myFreeData;
	};

	template<typename ObjectType, int ObjectCapacity>
	inline CStaticMemoryPool<ObjectType, ObjectCapacity>::CStaticMemoryPool()
	{
		for (ObjectType& object : myData)
		{
			myFreeData.Add(&object);
		}
	}

	template<typename ObjectType, int ObjectCapacity>
	inline CStaticMemoryPool<ObjectType, ObjectCapacity>::~CStaticMemoryPool()
	{
	}

	template<typename ObjectType, int ObjectCapacity>
	inline ObjectType* CStaticMemoryPool<ObjectType, ObjectCapacity>::Require()
	{
		if (myFreeData.Size() <= 0)
		{
			assert(!"MemoryPool out of memory");
			return nullptr;
		}

		return myFreeData.Pop();
	}

	template<typename ObjectType, int ObjectCapacity>
	inline bool CStaticMemoryPool<ObjectType, ObjectCapacity>::Recycle(ObjectType* aObject)
	{
		if (!aObject)
		{
			assert(!"Trying to recycle nullptr!");
			return false;
		}

		assert(myFreeData.Find(aObject) == myFreeData.FoundNone && "recycling twice in memory pool :(");

		myFreeData.Add(aObject);
		return true;
	}
}
