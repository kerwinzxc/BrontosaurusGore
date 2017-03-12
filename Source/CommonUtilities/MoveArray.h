#pragma once

namespace CU
{
	template<bool IsPod, bool MoveUp, typename ObjectType, typename SizeType>
	struct MoveArray;

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