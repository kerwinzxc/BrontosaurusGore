#pragma once
#include <atomic>
#include <assert.h>

namespace Container
{
	template<typename T, unsigned int QueueLimit = 5000>
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

template <typename T, unsigned int QueueLimit = 5000>
Container::IQueue<T, QueueLimit>::IQueue() :
myItemCount(0), 
myHead(nullptr), 
myTail(nullptr)
{
}

template <typename T, unsigned int QueueLimit = 5000>
Container::IQueue<T, QueueLimit>::~IQueue()
{
}

template <typename T, unsigned int QueueLimit = 5000>
void Container::IQueue<T, QueueLimit>::CheckQueueLength() const
{
	assert(Size() < 2000 && "Queue overflow, messages are probably never handled.");
}

template <typename T, unsigned int QueueLimit = 5000>
void Container::IQueue<T, QueueLimit>::Append(IQueue& aQueue)
{
	if(aQueue.IsEmpty() == true)
	{
		return;
	}

	if (IsEmpty() == false)
	{
		typename myTail->next = aQueue.myHead;
	}
	else
	{
		typename myHead = aQueue.myHead;
	}
	typename myTail = aQueue.myTail;
	myItemCount += aQueue.myItemCount;
	aQueue.myHead = nullptr;
	aQueue.myTail = nullptr;
	aQueue.myItemCount = 0;
	
	CheckQueueLength();
}

template <typename T, unsigned int QueueLimit = 5000>
unsigned Container::IQueue<T, QueueLimit>::Size() const
{
	return myItemCount;
}

template <typename T, unsigned int QueueLimit = 5000>
bool Container::IQueue<T, QueueLimit>::IsEmpty() const
{
	return Size() == 0;
}

template <typename T, unsigned int QueueLimit = 5000>
void Container::IQueue<T, QueueLimit>::RemoveAll()
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

template <typename T, unsigned int QueueLimit = 5000>
void Container::IQueue<T, QueueLimit>::IncrementCount()
{
	++myItemCount;
}

template <typename T, unsigned int QueueLimit = 5000>
void Container::IQueue<T, QueueLimit>::DecrementCount()
{
	--myItemCount;
}
