#pragma once
#pragma message("Growing Array compiled")
#include <assert.h>
#include <string.h> // memcpy
#include <initializer_list>
#include "QuickSort.h"
#include "StaticTypeTraits.h"
#include "ReverseGrowingArray.h"

#define self (*this)

namespace CU
{
	template<bool IsPod, typename ObjectType, typename SizeType>
	struct CopyArray;

	template<bool IsPod, bool MoveUp, typename ObjectType, typename SizeType>
	struct MoveArray;

#pragma warning(disable : 4348)
	template<typename ObjectType, typename SizeType = unsigned int, bool USE_SAFE_MODE = true>
	class GrowingArray
	{
	public:
		using iterator = ObjectType*;
		using const_iterator = const ObjectType*;
		using object_type = ObjectType;
		using size_type = SizeType;

		GrowingArray();
		GrowingArray(const SizeType aStartCapacity);
		explicit GrowingArray(const SizeType aStartCapacity, const ObjectType& aItemToFillWith);
		GrowingArray(const std::initializer_list<ObjectType>& aInitializerList);
		GrowingArray(const GrowingArray& aGrowingArray);
		GrowingArray(GrowingArray&& aGrowingArray);

		~GrowingArray();

		GrowingArray& operator=(const GrowingArray& aGrowingArray);
		GrowingArray& operator=(GrowingArray&& aGrowingArray);

		void Init(const SizeType aStartCapacity);
		void Init(const SizeType aStartCapacity, const ObjectType& aItemToFillWith);
		void ReInit(const SizeType aStartCapacity);

		inline ObjectType& operator[](const SizeType aIndex);
		inline const ObjectType& operator[](const SizeType aIndex) const;

		inline ObjectType& At(const SizeType aIndex);
		inline const ObjectType& At(const SizeType aIndex) const;

		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;

		iterator rbegin();
		const_iterator rbegin() const;
		iterator rend();
		const_iterator rend() const;

		inline void Add();
		inline void Add(const ObjectType& aObject);
		inline void Add(ObjectType&& aObject);
		inline void Add(const GrowingArray& aArray);
		inline void AddChunk(const void* aChunkPointer, const SizeType aByteSize);
		inline void Insert(const SizeType aIndex, const ObjectType& aObject);
		inline void Insert(const SizeType aIndex, ObjectType&& aObject);

		inline void Remove(const ObjectType& aObject);
		inline void RemoveAtIndex(const SizeType aIndex);
		inline void RemoveCyclic(const ObjectType& aObject);
		inline void RemoveCyclicAtIndex(const SizeType aIndex);

		inline void Delete(const ObjectType& aObject);
		inline void DeleteAtIndex(const SizeType aIndex);
		inline void DeleteCyclic(const ObjectType& aObject);
		inline void DeleteCyclicAtIndex(const SizeType aIndex);
		inline SizeType Find(const ObjectType& aObject) const;
		inline bool Find(const ObjectType& aObject, SizeType& aReturnIndex) const;

		inline ObjectType& GetLast();
		inline const ObjectType& GetLast() const;
		inline ObjectType& GetFirst();
		inline const ObjectType& GetFirst() const;

		inline ObjectType* AsPointer(const SizeType aIndex = 0);
		inline const ObjectType* AsPointer(const SizeType aIndex = 0) const;
		inline void* AsVoidPointer(const SizeType aIndex = 0);
		inline const void* AsVoidPointer(const SizeType aIndex = 0) const;

		inline ObjectType Pop();

		inline void RemoveAll();
		inline void DeleteAll();

		inline void Optimize();
		inline void ShrinkToFit();

		inline void QuickSort(std::function<bool(ObjectType, ObjectType)> aCompareFunction);

		inline void Resize(const SizeType aNewSize);
		inline void Resize(const SizeType aNewSize, const ObjectType& aObject);
		inline void Destroy();

		__forceinline SizeType ByteSize() const;
		__forceinline SizeType Size() const;
		__forceinline SizeType Capacity() const;
		__forceinline bool Empty() const;
		__forceinline bool IsInitialized() const;
		__forceinline bool HasIndex(const SizeType aIndex) const;

		static const SizeType FoundNone = static_cast<SizeType>(-1);

	private:
		inline void Reallocate(const SizeType aNewSize);

		struct Copy : CopyArray<IsPod<ObjectType>::Result || !USE_SAFE_MODE, ObjectType, SizeType> {};
		template<bool MoveUp> struct Move : MoveArray<IsPod<ObjectType>::Result || !USE_SAFE_MODE, MoveUp, ObjectType, SizeType> {};

		ObjectType* myArray;
		SizeType mySize;
		SizeType myCapacity;
	};

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::GrowingArray()
	{
		mySize = 0;
		myCapacity = 0;
		myArray = nullptr;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::GrowingArray(const SizeType aStartCapacity) : GrowingArray()
	{
		Init(aStartCapacity);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::GrowingArray(const SizeType aStartCapacity, const ObjectType& aItemToFillWith) : GrowingArray()
	{
		Init(aStartCapacity, aItemToFillWith);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::GrowingArray(const std::initializer_list<ObjectType>& aInitializerList) : GrowingArray()
	{
		Init(static_cast<SizeType>(aInitializerList.size()));

		for (const ObjectType& initilizer : aInitializerList)
		{
			Add(initilizer);
		}
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::GrowingArray(const GrowingArray& aGrowingArray) : GrowingArray()
	{
		self = aGrowingArray;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::GrowingArray(GrowingArray&& aGrowingArray) : GrowingArray()
	{
		self = std::move(aGrowingArray);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::~GrowingArray()
	{
		Destroy();				//OM FEL, SKÄLL PÅ KEVIN! och Carl
	}


	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>& GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::operator=(const GrowingArray& aGrowingArray)
	{
		if (IsInitialized() == true)
		{
			Destroy();
		}

		if (aGrowingArray.IsInitialized() == true)
		{
			Init(aGrowingArray.myCapacity);
			mySize = aGrowingArray.mySize;

			Copy::DoCopy(myArray, aGrowingArray.myArray, mySize);
		}

		return self;
	}


	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>& GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::operator=(GrowingArray&& aGrowingArray)
	{
		if (IsInitialized() == true)
		{
			delete[] myArray;
		}

		myArray = aGrowingArray.myArray;
		aGrowingArray.myArray = nullptr;
		mySize = aGrowingArray.mySize;
		aGrowingArray.mySize = 0;
		myCapacity = aGrowingArray.myCapacity;
		aGrowingArray.myCapacity = 0;

		return self;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Init(const SizeType aStartCapacity)
	{
		assert(IsInitialized() == false && "Growing array must not be initialized twice, consider ReInit");
		assert(aStartCapacity > 0 && "Growing array must not be inited with zero capacity");

		myArray = new ObjectType[aStartCapacity];
		myCapacity = aStartCapacity;
		mySize = 0;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Init(const SizeType aStartCapacity, const ObjectType& aItemToFillWith)
	{
		assert(IsInitialized() == false && "Growing array must not be initialized twice, consider ReInit");
		assert(aStartCapacity > 0 && "Growing array must not be inited with zero capacity");

		Init(aStartCapacity);

		for (SizeType i = 0; i < aStartCapacity; ++i)
		{
			Add(aItemToFillWith); //fill array -> memset?
		}
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::ReInit(const SizeType aStartCapacity)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert(aStartCapacity > 0 && "Growing array must not be inited with zero capacity");

		Destroy();
		Init(aStartCapacity);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline ObjectType& GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::operator[](const SizeType aIndex)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds");
		return myArray[aIndex];
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline const ObjectType& GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::operator[](const SizeType aIndex) const
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds");
		return myArray[aIndex];
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline ObjectType& GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::At(const SizeType aIndex)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds");
		return myArray[aIndex];
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline const ObjectType& GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::At(const SizeType aIndex) const
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds");
		return myArray[aIndex];
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline typename GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::iterator GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::begin()
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		return myArray;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline typename GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::const_iterator GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::begin() const
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		return myArray;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline typename GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::iterator GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::end()
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		return (myArray + mySize);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline typename GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::const_iterator GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::end() const
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		return (myArray + mySize);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline typename GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::iterator GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::rbegin()
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		return myArray + mySize - 1;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline typename GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::const_iterator GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::rbegin() const
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		return myArray + mySize - 1;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline typename GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::iterator GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::rend()
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		return (myArray - 1);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline typename GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::const_iterator GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::rend() const
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		return (myArray - 1);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Add()
	{
		ObjectType temp;
		Add(std::move(temp));
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Add(const ObjectType& aObject)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");

		if (mySize >= myCapacity)
		{
			Reallocate(myCapacity * 2);
		}

		myArray[mySize++] = aObject;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Add(ObjectType&& aObject)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");

		if (mySize >= myCapacity)
		{
			Reallocate(myCapacity * 2);
		}

		myArray[mySize++] = std::move(aObject);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Add(const GrowingArray& aArray)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");

		for (ComponentId i = 0; i < anArray.Size(); ++i)
		{
			Add(anArray[i]);
		}
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::AddChunk(const void* aChunkPointer, const SizeType aByteSize)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		static_assert(!USE_SAFE_MODE || IsPod<ObjectType>::Result, "Should not add chunk to non-pod array");

		const SizeType numberOfAdds = aByteSize / sizeof(ObjectType);

		if (mySize + numberOfAdds >= myCapacity)
		{
#define TEMP_MAX(a,b) (((a) > (b)) ? (a) : (b))
			
			Reallocate(TEMP_MAX(myCapacity * 2, mySize + numberOfAdds));

#undef TEMP_MAX
		}

		memcpy(myArray + mySize, aChunkPointer, aByteSize);
		mySize += numberOfAdds;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Insert(const SizeType aIndex, const ObjectType& aObject)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");

		if (mySize >= myCapacity)
		{
			Reallocate(myCapacity * 2);
		}

		Move<true>::DoMove(myArray, 1, aIndex, mySize);

		myArray[aIndex] = aObject;
		++mySize;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Insert(const SizeType aIndex, ObjectType&& aObject)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");

		if (mySize >= myCapacity)
		{
			Reallocate(myCapacity * 2);
		}

		Move<true>::DoMove(myArray, 1, aIndex, mySize);

		myArray[aIndex] = std::move(aObject);
		++mySize;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Remove(const ObjectType& aObject)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");

		SizeType index = Find(aObject);
		if (index != FoundNone)
		{
			RemoveAtIndex(index);
		}
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::RemoveAtIndex(const SizeType aIndex)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds");

		if (mySize > 1)
		{
			Move<false>::DoMove(myArray, 1, aIndex, mySize);
		}

		--mySize;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Delete(const ObjectType& aObject)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		DeleteAtIndex(Find(aObject));
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::DeleteAtIndex(const SizeType aIndex)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds");

		delete myArray[aIndex];
		RemoveAtIndex(aIndex);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::DeleteCyclic(const ObjectType& aObject)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");

		SizeType index = Find(aObject);
		if (index != FoundNone)
		{
			DeleteCyclicAtIndex(index);
		}
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::DeleteCyclicAtIndex(const SizeType aIndex)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds!");

		delete myArray[aIndex];
		myArray[aIndex] = myArray[--mySize];
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::RemoveCyclic(const ObjectType& aObject)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");

		SizeType index = Find(aObject);
		if (index != FoundNone)
		{
			RemoveCyclicAtIndex(index);
		}
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::RemoveCyclicAtIndex(const SizeType aIndex)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds!");

		myArray[aIndex] = myArray[mySize - 1];
		myArray[mySize - 1] = ObjectType(); //trying this to fix sharedptr error, mvh carl
		--mySize;
	}


	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline SizeType GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Find(const ObjectType& aObject) const
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		for (SizeType i = 0; i < mySize; ++i)
		{
			if (myArray[i] == aObject)
			{
				return i;
			}
		}

		return FoundNone;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	bool GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Find(const ObjectType& aObject, SizeType& aReturnIndex) const
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");

		for (SizeType i = 0; i < mySize; ++i)
		{
			if (myArray[i] == aObject)
			{
				aReturnIndex = i;
				return true;
			}
		}

		return false;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline ObjectType& GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::GetLast()
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((0 < mySize) && "GrowingArray is empty");
		return myArray[mySize - 1];
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline const ObjectType& GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::GetLast() const
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((0 < mySize) && "GrowingArray is empty");
		return myArray[mySize - 1];
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline ObjectType& GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::GetFirst()
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((0 < mySize) && "GrowingArray is empty");
		return myArray[0];
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline const ObjectType& GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::GetFirst() const
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((0 < mySize) && "GrowingArray is empty");
		return myArray[0];
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline ObjectType* GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::AsPointer(const SizeType aIndex)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds");
		return (myArray + aIndex);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline const ObjectType* GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::AsPointer(const SizeType aIndex) const
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds");
		return (myArray + aIndex);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void* GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::AsVoidPointer(const SizeType aIndex)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds");
		return static_cast<void*>(myArray + aIndex);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline const void* GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::AsVoidPointer(const SizeType aIndex) const
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds");
		return static_cast<void*>(myArray + aIndex);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline ObjectType GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Pop()
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		assert(Size() > 0 && "GrowingArray is empty.");
		return myArray[--mySize];
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::RemoveAll()
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		mySize = 0;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::DeleteAll()
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		for (SizeType i = 0; i < mySize; ++i)
		{
			if (myArray[i] != nullptr)
			{
				delete myArray[i];
				myArray[i] = nullptr;
			}
		}
		mySize = 0;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Optimize()
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		Reallocate(mySize);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::ShrinkToFit()
	{
		Optimize();
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void CU::GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::QuickSort(std::function<bool(ObjectType, ObjectType)> aCompareFunction)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		CU::QuickSort(*this, static_cast<SizeType>(0), mySize, aCompareFunction);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	__forceinline SizeType GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::ByteSize() const
	{
		return mySize * sizeof(ObjectType);
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	__forceinline SizeType GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Size() const
	{
		return mySize;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	__forceinline SizeType GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Capacity() const
	{
		return myCapacity;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	__forceinline bool GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Empty() const
	{
		return mySize == 0;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline bool GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::IsInitialized() const
	{
		return myArray != nullptr;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline bool GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::HasIndex(const SizeType aIndex) const
	{
		return aIndex >= 0 && aIndex < Size();
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Resize(SizeType aNewSize)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");

		if (aNewSize > myCapacity)
		{
			Reallocate(aNewSize);
		}

		mySize = aNewSize;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Resize(SizeType aNewSize, const ObjectType& aObject)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");

		Resize(aNewSize);

		for (SizeType i = 0; i < Size(); ++i)
		{
			myArray[i] = aObject;
		}
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Reallocate(const SizeType aNewSize)
	{
		assert(IsInitialized() == true && "GrowingArray not yet initialized.");
		ObjectType *newArray = new ObjectType[aNewSize];

		Copy::DoCopy(newArray, myArray, mySize);

		delete[] myArray;
		myArray = newArray;
		myCapacity = aNewSize;
	}

	template<typename ObjectType, typename SizeType, bool USE_SAFE_MODE>
	inline void GrowingArray<ObjectType, SizeType, USE_SAFE_MODE>::Destroy()
	{
		delete[] myArray;
		myArray = nullptr;
		myCapacity = 0;
		mySize = 0;
	}

	template<typename ObjectType, typename SizeType>
	struct CopyArray<true, ObjectType, SizeType>
	{
		static void DoCopy(ObjectType aCopyToArray[], const ObjectType aCopyFromArray[], const SizeType aElementsToCopy)
		{
			if (aElementsToCopy > 0)
			{
				memcpy(aCopyToArray, aCopyFromArray, static_cast<size_t>(aElementsToCopy * sizeof(ObjectType)));
			}
		}
	};

	template<typename ObjectType, typename SizeType>
	struct CopyArray<false, ObjectType, SizeType>
	{
		static void DoCopy(ObjectType aCopyToArray[], const ObjectType aCopyFromArray[], const SizeType aElementsToCopy)
		{
			for (SizeType i = 0; i < aElementsToCopy; ++i)
			{
				aCopyToArray[i] = aCopyFromArray[i];
			}
		}
	};

	template<typename ObjectType, typename SizeType>
	struct MoveArray<true, true, ObjectType, SizeType>
	{
		static void DoMove(ObjectType aArray[], const SizeType aElementsToMove, const SizeType aStartIndex, const SizeType aArraySize)
		{
			memmove(aArray + aStartIndex + aElementsToMove, aArray + aStartIndex, sizeof(ObjectType) * (aArraySize - aStartIndex));
		}
	};

	template<typename ObjectType, typename SizeType>
	struct MoveArray<true, false, ObjectType, SizeType>
	{
		static void DoMove(ObjectType aArray[], const SizeType aElementsToMove, const SizeType aStartIndex, const SizeType aArraySize)
		{
			memmove(aArray + aStartIndex, aArray + aStartIndex + aElementsToMove, sizeof(ObjectType) * (aArraySize - aStartIndex));
		}
	};

	template<typename ObjectType, typename SizeType>
	struct MoveArray<false, false, ObjectType, SizeType>
	{
		static void DoMove(ObjectType aArray[], const SizeType aElementsToMove, const SizeType aStartIndex, const SizeType aArraySize)
		{
			for (SizeType i = aStartIndex; i < aArraySize - aElementsToMove; ++i)
			{
				aArray[i] = aArray[i + aElementsToMove];
			}
		}
	};

	template<typename ObjectType, typename SizeType>
	struct MoveArray<false, true, ObjectType, SizeType>
	{
		static void DoMove(ObjectType aArray[], const SizeType aElementsToMove, const SizeType aStartIndex, const SizeType aArraySize)
		{
			for (SizeType i = aStartIndex; i < aArraySize - aElementsToMove; ++i)
			{
				aArray[i] = aArray[i + aElementsToMove];
			}
		}
	};
}

#pragma warning(default : 4348)
#undef self
