#pragma once
#include <atomic>
#include <assert.h>

namespace Container
{
	template<typename T>
	class IQueue
	{
	public:
		IQueue();
		virtual ~IQueue();

		virtual void Push(T aValue) = 0;
		void CheckQueueLength() const;
		virtual void Append(IQueue& aQueue);
		virtual T Pop() = 0;

		unsigned Size() const;
		bool IsEmpty() const;

		virtual void RemoveAll();

	protected:
		void IncrementCount();
		void DecrementCount();
		struct QueueItem
		{
			QueueItem* next = nullptr;
			T value;
		};

		volatile unsigned int myItemCount;

		QueueItem*  myHead;
		QueueItem* myTail;
	};

	
}

template <typename T>
Container::IQueue<T>::IQueue() : 
myItemCount(0), 
myHead(nullptr), 
myTail(nullptr)
{
}

template <typename T>
Container::IQueue<T>::~IQueue()
{
}

template <typename T>
void Container::IQueue<T>::CheckQueueLength() const
{
	assert(Size() < 2000 && "Queue overflow, messages are probably never handled.");
}

template <typename T>
void Container::IQueue<T>::Append(IQueue& aQueue)
{
	if(aQueue.IsEmpty() == true)
	{
		return;
	}

	if (IQueue<T>::IsEmpty() == false)
	{
		typename IQueue<T>::myTail->next = aQueue.myHead;
	}
	else
	{
		typename IQueue<T>::myHead = aQueue.myHead;
	}
	typename IQueue<T>::myTail = aQueue.myTail;
	myItemCount += aQueue.myItemCount;
	aQueue.myHead = nullptr;
	aQueue.myTail = nullptr;
	aQueue.myItemCount = 0;
	
	CheckQueueLength();
}

template <typename T>
unsigned Container::IQueue<T>::Size() const
{
	return myItemCount;
}

template <typename T>
bool Container::IQueue<T>::IsEmpty() const
{
	return Size() == 0;
}

template <typename T>
void Container::IQueue<T>::RemoveAll()
{
	for (typename IQueue<T>::QueueItem* item = myHead; item != nullptr; )
	{
		DecrementCount();
		typename IQueue<T>::QueueItem* itemToDelete = item;
		item = item->next;
		delete itemToDelete;
	}
	myHead = nullptr;
	myTail = nullptr;
}

template <typename T>
void Container::IQueue<T>::IncrementCount()
{
	++myItemCount;
}

template <typename T>
void Container::IQueue<T>::DecrementCount()
{
	--myItemCount;
}
