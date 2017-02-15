#pragma once
#include <functional>

namespace CU
{
	template<typename type, typename sizet, bool safemode>
	class GrowingArray;


	template<typename type, typename sizet, bool safemode>
	int Partition(
		GrowingArray<type, sizet, safemode>& A, 
		int p, 
		int q, 
		std::function<bool(type, type)> aCompFunction);

	template<typename type, typename sizet, bool safemode>
	void QuickSort(
		GrowingArray<type, sizet, safemode>& aList, 
		int aStartIndex, 
		int aEndIndex, 
		std::function<bool(type, type)> aCompFunction);



	template<typename type, typename sizet, bool safemode>
	int Partition(GrowingArray<type, sizet, safemode>& A, int p, int q, std::function<bool(type, type)> aCompFunction)
	{
		sizet i = p;
		sizet j;

		type temp;

		for (j = p + static_cast<sizet>(1); j < q; j++)
		{
			if (aCompFunction(A[p], A[j]) == true)
			{
				i = i + 1;
				temp = A[i];
				A[i] = A[j];
				A[j] = temp;
			}
		}

		temp = A[i];
		A[i] = A[p];
		A[p] = temp;

		return i;
	}


	template<typename type, typename sizet, bool safemode>
	void QuickSort(GrowingArray<type, sizet, safemode>& aList, int aStartIndex, int aEndIndex, std::function<bool(type, type)> aCompFunction)
	{
		int r;
		if (aStartIndex < aEndIndex)
		{
			r = Partition(aList, aStartIndex, aEndIndex, aCompFunction);
			QuickSort(aList, aStartIndex, r, aCompFunction);
			QuickSort(aList, r + 1, aEndIndex, aCompFunction);
		}
	}


}
