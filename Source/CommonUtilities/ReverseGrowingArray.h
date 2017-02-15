#pragma once

namespace CU
{
	template<typename GrowingArrayType>
	class Reverse;

	template<typename GrowingArrayType>
	Reverse<GrowingArrayType> MakeReversed(GrowingArrayType& aGrowingArray)
	{
		return Reverse<GrowingArrayType>(aGrowingArray);
	}

	template<typename GrowingArrayType>
	class Reverse
	{
	public:
		using iterator = typename GrowingArrayType::iterator;
		using const_iterator = typename GrowingArrayType::const_iterator;

		Reverse(GrowingArrayType& aGrowingArray);

		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;

	private:
		GrowingArrayType& myGrowingArrayReference;
	};

	template<typename GrowingArrayType>
	inline Reverse<GrowingArrayType>::Reverse(GrowingArrayType& aGrowingArray)
		: myGrowingArrayReference(aGrowingArray)
	{
		static_assert(false, "reveres iterators not implemented");
	}

	template<typename GrowingArrayType>
	inline typename Reverse<GrowingArrayType>::iterator Reverse<GrowingArrayType>::begin()
	{
		return myGrowingArrayReference.rbegin();
	}

	template<typename GrowingArrayType>
	inline typename Reverse<GrowingArrayType>::const_iterator Reverse<GrowingArrayType>::begin() const
	{
		return myGrowingArrayReference.rbegin();
	}

	template<typename GrowingArrayType>
	inline typename Reverse<GrowingArrayType>::iterator Reverse<GrowingArrayType>::end()
	{
		return myGrowingArrayReference.rend();
	}

	template<typename GrowingArrayType>
	inline typename Reverse<GrowingArrayType>::const_iterator Reverse<GrowingArrayType>::end() const
	{
		return myGrowingArrayReference.rend();
	}
}