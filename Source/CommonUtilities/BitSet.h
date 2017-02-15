#pragma once

#ifndef assert
#include <assert.h>
#endif // !assert

#define INDEX_TO_BIT(A_INDEX) (static_cast<Type>(1) << A_INDEX)
#define self (*this)

namespace CU
{
	template<typename INTEGER_TYPE, unsigned long long BIT_COUNT>
	struct AllBitsSet
	{
		using Type = INTEGER_TYPE;
		static const Type AllBits = INDEX_TO_BIT(BIT_COUNT) | AllBitsSet<Type, BIT_COUNT - 1>::AllBits;
	};

	template<typename INTEGER_TYPE>
	struct AllBitsSet<INTEGER_TYPE, 0>
	{
		using Type = INTEGER_TYPE;
		static const Type AllBits = INDEX_TO_BIT(0);
	};

	template<bool CONDITION, typename IF_TRUE, typename IF_FALSE>
	struct COMPILE_TIME_IF
	{
		using IfType = IF_FALSE;
	};

	template<typename IF_TRUE, typename IF_FALSE>
	struct COMPILE_TIME_IF<true, IF_TRUE, IF_FALSE>
	{
		using IfType = IF_TRUE;
	};

	template<unsigned int BIT_COUNT>
	class CBitSet
	{
	public:
		using Type = typename COMPILE_TIME_IF<(BIT_COUNT <= 32u), unsigned int, unsigned long long>::IfType;

		class CReference
		{
		public:
			friend class CBitSet<BIT_COUNT>;
			~CReference();

			CReference& operator=(const CReference& aValue);
			CReference& operator=(const bool aValue);
			operator bool() const;

		private:
			CReference(CBitSet& aBitSet, const Type aIndex);

			CBitSet& myBitSet;
			Type myIndex;
		};

		CBitSet();
		CBitSet(const Type aNumber);
		~CBitSet();

		inline CReference operator[](const Type aIndex);
		inline bool operator[](const Type aIndex) const;

		//template<typename ENUM_TYPE>
		//inline CReference operator[](const ENUM_TYPE aIndex);
		//template<typename ENUM_TYPE>
		//inline bool operator[](const ENUM_TYPE aIndex) const;

		inline void Set(const Type aIndex, const bool aValue = true);
		inline void Flip(const Type aIndex);

		inline bool Get(const Type aIndex) const;
		inline bool Any() const;
		inline bool All() const;

		inline Type GetBits() const;

	private:
		Type myBits;
	};

	template<unsigned int BIT_COUNT>
	CBitSet<BIT_COUNT>::CBitSet()
		: myBits(0u)
	{
		static_assert(BIT_COUNT <= 64u, "bitset not implemented to hold larger than unsigned long long");
		static_assert(BIT_COUNT > 0u, "bitset can not hold zero bits");
	}

	template<unsigned int BIT_COUNT>
	CBitSet<BIT_COUNT>::CBitSet(const Type aNumber)
		: myBits(aNumber)
	{
		static_assert(BIT_COUNT <= 64u, "bitset not implemented to hold larger than unsigned long long");
		static_assert(BIT_COUNT > 0u, "bitset can not hold zero bits");
	}

	template<unsigned int BIT_COUNT>
	CBitSet<BIT_COUNT>::~CBitSet()
	{
	}

	template<unsigned int BIT_COUNT>
	inline typename CBitSet<BIT_COUNT>::CReference CBitSet<BIT_COUNT>::operator[](const Type aIndex)
	{
		assert(aIndex < BIT_COUNT && "bitset index out of range");
		return CReference(self, aIndex);
	}

	template<unsigned int BIT_COUNT>
	inline bool CBitSet<BIT_COUNT>::operator[](const Type aIndex) const
	{
		assert(aIndex < BIT_COUNT && "bitset index out of range");
		return Get(aIndex);
	}

	//template<typename ENUM_TYPE>
	//template<unsigned int BIT_COUNT>
	//inline typename CBitSet<BIT_COUNT>::CReference CBitSet<BIT_COUNT>::operator[](const ENUM_TYPE aIndex)
	//{
	//	return CReference(self, static_cast<Type>(aIndex));
	//}

	//template<typename ENUM_TYPE>
	//template<unsigned int BIT_COUNT>
	//inline bool CBitSet<BIT_COUNT>::operator[](const ENUM_TYPE aIndex) const
	//{
	//	return Get(static_cast<Type>(aIndex));
	//}

	template<unsigned int BIT_COUNT>
	inline void CBitSet<BIT_COUNT>::Set(const Type aIndex, const bool aValue)
	{
		assert(aIndex < BIT_COUNT && "bitset index out of range");
		aValue ? (myBits |= INDEX_TO_BIT(aIndex)) : (myBits ^= INDEX_TO_BIT(aIndex));
	}

	template<unsigned int BIT_COUNT>
	inline void CBitSet<BIT_COUNT>::Flip(const Type aIndex)
	{
		assert(aIndex < BIT_COUNT && "bitset index out of range");
		myBits ^= INDEX_TO_BIT(aIndex);
	}

	template<unsigned int BIT_COUNT>
	inline bool CBitSet<BIT_COUNT>::Get(const Type aIndex) const
	{
		assert(aIndex < BIT_COUNT && "bitset index out of range");
		return (myBits & INDEX_TO_BIT(aIndex)) > 0u;
	}

	template<unsigned int BIT_COUNT>
	inline bool CBitSet<BIT_COUNT>::Any() const
	{
		return myBits > 0u;
	}

	template<unsigned int BIT_COUNT>
	inline bool CBitSet<BIT_COUNT>::All() const
	{		
		return AllBitsSet<Type, BIT_COUNT - 1>::AllBits == myBits;
	}

	template<unsigned int BIT_COUNT>
	inline typename CBitSet<BIT_COUNT>::Type CBitSet<BIT_COUNT>::GetBits() const
	{
		return myBits;
	}

	template<unsigned int BIT_COUNT>
	inline typename CBitSet<BIT_COUNT>::CReference& CBitSet<BIT_COUNT>::CReference::operator=(const CReference& aValue)
	{
		self = static_cast<bool>(aValue);
		return self;
	}

	template<unsigned int BIT_COUNT>
	inline typename CBitSet<BIT_COUNT>::CReference& CBitSet<BIT_COUNT>::CReference::operator=(const bool aValue)
	{
		myBitSet.Set(myIndex, aValue);
		return self;
	}

	template<unsigned int BIT_COUNT>
	inline CBitSet<BIT_COUNT>::CReference::operator bool() const
	{
		return myBitSet.Get(myIndex);
	}

	template<unsigned int BIT_COUNT>
	CBitSet<BIT_COUNT>::CReference::CReference(CBitSet& aBitSet, const Type aIndex)
		: myBitSet(aBitSet)
		, myIndex(aIndex)
	{
		assert(aIndex < BIT_COUNT && "bitset index out of range");
	}

	template<unsigned int BIT_COUNT>
	CBitSet<BIT_COUNT>::CReference::~CReference()
	{
	}
}

#undef INDEX_TO_BIT
#undef self