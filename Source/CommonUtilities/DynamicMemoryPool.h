#pragma once

#ifndef assert
#include <assert.h>
#endif // !assert

namespace CU
{
	template<typename ObjectType>
	class CDynamicMemoryPool
	{
	public:
		CDynamicMemoryPool();
		~CDynamicMemoryPool();

		ObjectType* Require();
		bool Recycle(ObjectType* aObject);

	private:
		GrowingArray<ObjectType> myData;
		GrowingArray<ObjectType*, unsigned int, false> myFreeData;
	};

	template<typename ObjectType>
	inline CDynamicMemoryPool<ObjectType>::CDynamicMemoryPool()
	{
		for (ObjectType& object : myData)
		{
			myFreeData.Add(&object);
		}
	}

	template<typename ObjectType>
	inline CDynamicMemoryPool<ObjectType>::~CDynamicMemoryPool()
	{
	}

	template<typename ObjectType>
	inline ObjectType* CDynamicMemoryPool<ObjectType>::Require()
	{
		if (myFreeData.Size() <= 0)
		{
			assert(!"MemoryPool out of memory");
			return nullptr;
		}

		return myFreeData.Pop();
	}

	template<typename ObjectType>
	inline bool CDynamicMemoryPool<ObjectType>::Recycle(ObjectType* aObject)
	{
		if (aObject == nullptr)
		{
			assert(!"Trying to recycle nullptr!");
			return false;
		}

		myFreeData.Add(aObject);
		return true;
	}
}
