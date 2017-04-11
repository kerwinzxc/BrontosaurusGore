#pragma once

namespace CU
{
	template<bool IsPod, typename ObjectType, typename SizeType>
	struct CopyArray;

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
		static void DoCopy(ObjectType aCopyToArray[], ObjectType aCopyFromArray[], const SizeType aElementsToCopy)
		{
			for (SizeType i = 0; i < aElementsToCopy; ++i)
			{
				aCopyToArray[i] = std::move(aCopyFromArray[i]);
			}
		}
	};
}
