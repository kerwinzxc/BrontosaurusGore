#pragma once
#include <utility>
#include <assert.h>

namespace CU
{
	template <typename T>
	class Lesser
	{
	public:
		bool operator()(const T& aLeft, const T& aRight)
		{
			return aLeft <= aRight;
		}
	};

	template <typename T>
	// womba womba
	class Greater
	{
	public:
		bool operator()(const T& aLeft, const T& aRight)
		{
			return aLeft >= aRight;
		}
	};

	template<typename T, typename COMP> // öh din gurka!
	class Heap
	{
	public:
		Heap()
		{
			myArray.Init(8);
		}

		Heap(const CU::GrowingArray<T> aArray) : myArray(aArray)
		{
			int lastNonLeafNode = myArray.Size() / 2 - 1;
			for (int i = lastNonLeafNode; i >= 0; --i)
			{
				MoveDown(i, myArray.Size() - 1);
			}
		}

		~Heap()
		{
		}

		void Enqueue(T aSomeData)
		{
			myArray.Add(aSomeData);
			unsigned int pos = myArray.Size() - 1;
		
			while (pos > 0 && COMP()(myArray[pos], myArray[(pos - 1) / 2]))
			{
				std::swap(myArray[(pos - 1) / 2], myArray[pos]);
				pos = (pos - 1) / 2;
			}
		}

		T Dequeue()
		{
			assert(myArray.Size() > 0 && "Cannot dequeue an empty array");
			T tempVal = myArray[0];
			myArray.RemoveCyclicAtIndex(0);
			MoveDown(0, myArray.Size() - 1);
			return tempVal;
			
		}

		bool IsEmpty()
		{
			return !myArray.Size();
		}

		void PrintMe()
		{
			for (int i = 0; i < myArray.Size(); ++i)
			{
				std::cout << myArray[i] << std::endl;
			}
		}

		void MoveDown(int aFirst, int aLast)
		{
			int largest = 2 * aFirst + 1;
			while (largest <= aLast)
			{
				if (largest < aLast && myComparer(myArray[largest + 1], myArray[largest]))
				{
					++largest;
				}

				if (myComparer(myArray[largest], myArray[aFirst]))
				{
					std::swap(myArray[largest], myArray[aFirst]);
					aFirst = largest;
					largest = 2 * aFirst + 1;
				}
				else
				{
					largest = aLast + 1;
				}
			}

			/*unsigned int largest = 2 * aFirst + 1;
			while (largest <= aLast)
			{
				if (largest < aLast && myComparer(myArray[largest + 1], myArray[largest]) == true)
				{
					++largest;
				}

				if (myComparer(myArray[largest], myArray[aFirst]) == true)
				{
					T swapData = myArray[aFirst];
					myArray[aFirst] = myArray[largest];
					myArray[largest] = swapData;
					aFirst = largest;
					largest = 2 * aFirst + 1;
				}
				else
				{
					largest = aLast + 1;
				}
			}*/
		}

		bool Find(const T& aSomeData)
		{
			return myArray.Find(aSomeData) != myArray.FoundNone;
		}


	private:
		CU::GrowingArray<T> myArray;
		COMP myComparer;
	};

} 


