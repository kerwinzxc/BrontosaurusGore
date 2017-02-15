#pragma once
#pragma message ("Vector on stack compiled")
#include <assert.h>

#define self (*this)

namespace CU
{
	template <typename ObjectType, int Capacity, typename SizeType = unsigned short, bool UseSafeModeFlag = true>
	class VectorOnStack
	{
	public:
		using iterator = ObjectType*;
		using const_iterator = const ObjectType*;

		VectorOnStack();
		VectorOnStack(const VectorOnStack& aVectorOnStack);
		explicit VectorOnStack(std::nullptr_t aNullPtr);
		VectorOnStack(const ObjectType& aObject, const SizeType aStartSize);
		~VectorOnStack();

		VectorOnStack& operator=(const VectorOnStack& aVectorOnStack);

		inline const ObjectType& operator[](const SizeType aIndex) const;
		inline ObjectType& operator[](const SizeType aIndex);

		inline ObjectType& At(const SizeType aIndex);
		inline const ObjectType& At(const SizeType aIndex) const;

		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;

		inline void Add(const ObjectType& aObject);
		inline bool SafeAdd(const ObjectType& aObject);
		inline void Add(ObjectType&& aObject);
		inline void Insert(const SizeType aIndex, ObjectType& aObject);
		inline void Remove(const ObjectType& aObject);
		inline void RemoveAtIndex(const SizeType aIndex);
		inline void DeleteCyclic(const ObjectType& aObject);
		inline void DeleteCyclicAtIndex(const SizeType aIndex);
		inline void RemoveCyclic(const ObjectType& aObject);
		inline void RemoveCyclicAtIndex(const SizeType aIndex);
		inline SizeType Find(const ObjectType& aObject) const;

		inline void Clear();
		inline void RemoveAll();
		inline void DeleteAll();
		inline ObjectType Pop();

		inline ObjectType& GetLast();
		inline const ObjectType& GetLast() const;
		inline SizeType Size() const;

		static const SizeType FoundNone = static_cast<SizeType>(-1);

	private:
		ObjectType myVectorOnStack[Capacity];
		SizeType mySize;

	};

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::VectorOnStack()
		: mySize(0)
	{
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::VectorOnStack(const VectorOnStack& aVectorOnStack)
		: VectorOnStack()
	{
		if (UseSafeModeFlag == false)
		{
			memcpy(myVectorOnStack, aVectorOnStack.myVectorOnStack, sizeof(VectorOnStack));
		}
		else
		{
			for (SizeType i = 0; i < Capacity; ++i)
			{
				myVectorOnStack[i] = aVectorOnStack[i];
			}
		}
		mySize = aVectorOnStack.Size();
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::VectorOnStack(std::nullptr_t aNullPtr)
		: VectorOnStack()
	{
		for (SizeType i = 0; i < Capacity; i++)
		{
			myVectorOnStack[i] = aNullPtr;
		}
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::VectorOnStack(const ObjectType& aObject, const SizeType aStartSize)
		: VectorOnStack()
	{
		mySize = aStartSize;
		for (SizeType i = 0; i < mySize; ++i)
		{
			At(i) = aObject;
		}
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::~VectorOnStack()
	{
		mySize = 0;
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>& VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::operator=(const VectorOnStack& aVectorOnStack)
	{
		if (UseSafeModeFlag == false)
		{
			memcpy(myVectorOnStack, aVectorOnStack.myVectorOnStack, sizeof(VectorOnStack));
		}
		else
		{
			for (SizeType i = 0; i < Capacity; ++i)
			{
				myVectorOnStack[i] = aVectorOnStack[i];
			}
		}
		mySize = aVectorOnStack.Size();

		return self;
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline const ObjectType& VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::operator[](const SizeType aIndex) const
	{
		return At(aIndex);
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline ObjectType& VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::operator[](const SizeType aIndex)
	{
		return At(aIndex);
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline ObjectType & VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::At(const SizeType aIndex)
	{
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds!");
		return myVectorOnStack[aIndex];
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline const ObjectType & VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::At(const SizeType aIndex) const
	{
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds!");
		return myVectorOnStack[aIndex];
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline typename VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::iterator VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::begin()
	{
		return myVectorOnStack;
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline typename VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::const_iterator VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::begin() const
	{
		return myVectorOnStack;
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline typename VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::iterator VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::end()
	{
		return myVectorOnStack + mySize;
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline typename VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::const_iterator VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::end() const
	{
		return myVectorOnStack + mySize;
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::Add(const ObjectType& aObject)
	{
		assert((mySize < Capacity) && "VectorOnStack is full!");
		SafeAdd(aObject);
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline bool VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::SafeAdd(const ObjectType& aObject)
	{
		if (mySize < Capacity)
		{
			myVectorOnStack[mySize++] = aObject;
			return true;
		}

		return false;
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::Add(ObjectType&& aObject)
	{
		assert((mySize < Capacity) && "VectorOnStack is full!");
		if (mySize < Capacity)
		{
			myVectorOnStack[mySize++] = std::move(aObject);
		}
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::Insert(const SizeType aIndex, ObjectType& aObject)
	{
		assert((mySize < Capacity + 1) && "VectorOnStack is full!");

		assert((aIndex >= 0 && aIndex < Capacity) && "Index out of bounds!");

		if (aIndex >= mySize)
		{
			Add(aObject);
			return;
		}

		for (SizeType i = (Size() - 1); i > aIndex; i--)
		{
			myVectorOnStack[i] = myVectorOnStack[i - 1];
		}

		myVectorOnStack[aIndex] = aObject;
		++mySize;
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::Remove(const ObjectType & aObject)
	{
		SizeType index = Find(aObject);
		if (index != FoundNone)
		{
			RemoveAtIndex(index);
		}
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool USE_SAFE_MODE>
	inline void VectorOnStack<ObjectType, Capacity, SizeType, USE_SAFE_MODE>::RemoveAtIndex(const SizeType aIndex)
	{
#ifdef _DEBUG
		bool useSafeMode = USE_SAFE_MODE;
		if (useSafeMode) useSafeMode;
#endif // _DEBUG

		if (mySize > 1)
		{
			if (USE_SAFE_MODE)
			{
				for (SizeType i = aIndex; i < Capacity; ++i)
				{
					myVectorOnStack[i] = myVectorOnStack[i + 1];
				}
			}
			else
			{
				memmove(myVectorOnStack + aIndex, myVectorOnStack + aIndex + 1, sizeof(ObjectType) * (mySize - aIndex)); //if wrong, it's here
			}

			--mySize;
		}
		else
		{
			RemoveAll();
		}
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::DeleteCyclic(const ObjectType& aObject)
	{
		SizeType index = Find(aObject);
		if (index != FoundNone)
		{
			DeleteCyclicAtIndex(index);
		}
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::DeleteCyclicAtIndex(const SizeType aIndex)
	{
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds!");

		delete myVectorOnStack[aIndex];
		myVectorOnStack[aIndex] = myVectorOnStack[--mySize];
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::RemoveCyclic(const ObjectType& aObject)
	{
		SizeType index = Find(aObject);
		if (index != FoundNone)
		{
			RemoveCyclicAtIndex(index);
		}
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::RemoveCyclicAtIndex(const SizeType aIndex)
	{
		assert((aIndex >= 0 && aIndex < mySize) && "Index out of bounds!");
		myVectorOnStack[aIndex] = myVectorOnStack[--mySize];
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline SizeType VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::Find(const ObjectType& aObject) const
	{
		for (SizeType i = 0; i < mySize; ++i)
		{
			if (myVectorOnStack[i] == aObject)
			{
				return i;
			}
		}

		return FoundNone;
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::Clear()
	{
		mySize = 0;
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::RemoveAll()
	{
		Clear();
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::DeleteAll()
	{
		for (SizeType i = 0; i < mySize; ++i)
		{
			delete myVectorOnStack[i];
			myVectorOnStack[i] = nullptr;
		}
		mySize = 0;
	}

	template<typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline ObjectType VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::Pop()
	{
		assert(mySize > 0 && "Cannot pop from empty vector on stack");
		return myVectorOnStack[--mySize];
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline SizeType VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::Size() const
	{
		return mySize;
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline ObjectType& VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::GetLast()
	{
		assert(mySize > 0 && "Cannot GetLast from empty vector on stack");
		return myVectorOnStack[mySize - 1];
	}

	template <typename ObjectType, int Capacity, typename SizeType, bool UseSafeModeFlag>
	inline const ObjectType& VectorOnStack<ObjectType, Capacity, SizeType, UseSafeModeFlag>::GetLast() const
	{
		assert(mySize > 0 && "Cannot GetLast from empty vector on stack");
		return myVectorOnStack[mySize - 1];
	}
} 

#undef self