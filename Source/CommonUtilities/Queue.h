#pragma once

namespace CU
{
	template<typename TYPE, typename SIZE_TYPE = unsigned>
	class Queue
	{
	public:
		Queue();
		explicit Queue(SIZE_TYPE);
		~Queue();

		void Push(TYPE anObject);
		void Push(const Queue aQueue);

		TYPE Peek()const;
		TYPE Pop();

		SIZE_TYPE Size() const;

		void Clear();
	private:

		GrowingArray<TYPE, SIZE_TYPE> myData;
		SIZE_TYPE myTail;
		SIZE_TYPE mySize;
	};

	template <typename TYPE, typename SIZE_TYPE>
	Queue<TYPE, SIZE_TYPE>::Queue() : Queue(64)
	{
	}

	template <typename TYPE, typename SIZE_TYPE>
	Queue<TYPE, SIZE_TYPE>::Queue(SIZE_TYPE aSize)
	{
		myData.Init(aSize, TYPE());
		myTail = 0;
		mySize = 0;
	}

	template <typename TYPE, typename SIZE_TYPE>
	Queue<TYPE, SIZE_TYPE>::~Queue()
	{
	}

	template <typename TYPE, typename SIZE_TYPE>
	void Queue<TYPE, SIZE_TYPE>::Push(TYPE anObject)
	{

		if (mySize + 1 >= myData.Size())
		{
			GrowingArray<TYPE, SIZE_TYPE> tempArray;
			while (Size() > 0)
			{
				tempArray.Add(Pop());
			}

			myData = tempArray;
			myTail = 0;
			mySize = tempArray.Size();
			for (int i = 0; i < mySize; ++i)
			{
				myData.Add(TYPE());
			}
		}

		SIZE_TYPE newIndex = myTail + mySize;

		if (newIndex >= myData.Size())
		{
			const SIZE_TYPE distanceToEnd = myData.Size() - myTail;
			newIndex = mySize - distanceToEnd;
		}

		myData[newIndex] = anObject;
		++mySize;
	}

	template <typename TYPE, typename SIZE_TYPE>
	void Queue<TYPE, SIZE_TYPE>::Push(const Queue aQueue)
	{
		while (aQueue.Size() > 0)
		{
			Push(aQueue.Pop());
		}
	}

	template <typename TYPE, typename SIZE_TYPE>
	TYPE Queue<TYPE, SIZE_TYPE>::Peek() const
	{
		if (Size() > 0)
		{
			return myData[myTail];
		}
		DL_ASSERT("Queue is empty");

		return TYPE();
	}

	template <typename TYPE, typename SIZE_TYPE>
	TYPE Queue<TYPE, SIZE_TYPE>::Pop()
	{
		TYPE returnObject = Peek();

		++myTail;
		--mySize;

		if (myTail >= myData.Size())
		{
			myTail = 0;
		}

		return returnObject;
	}

	template <typename TYPE, typename SIZE_TYPE>
	SIZE_TYPE Queue<TYPE, SIZE_TYPE>::Size() const
	{
		return mySize;
	}

	template <typename TYPE, typename SIZE_TYPE>
	void Queue<TYPE, SIZE_TYPE>::Clear()
	{
		myData.RemoveAll();
		myTail = 0;
		mySize = 0;
	}
}
