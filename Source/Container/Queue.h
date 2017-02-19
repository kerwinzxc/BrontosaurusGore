#pragma once
#include <atomic>

namespace Container
{
	template<typename T>
	class IQueue
	{
	public:
		IQueue();
		virtual ~IQueue();

		virtual void Push(T aValue) = 0;
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
