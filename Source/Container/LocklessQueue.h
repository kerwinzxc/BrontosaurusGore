#pragma once
#include <mutex>
#include "Queue.h"
#include <cassert>

//TODO: Check if there is a way to make this better
namespace Container
{
	template<typename T, unsigned int QueueLimit = 5000>
	class CLocklessQueue : public IQueue<T>
	{
	public:
		enum class Error
		{
			Ok,
			Blocked,
			Empty
		};

		CLocklessQueue();
		~CLocklessQueue();

		void Push(T aValue) override;
		void Append(IQueue<T, QueueLimit>& aQueue) override;
		T Pop() override;

		Error TryPush(const T aValue);
		Error TryPop(T& aValue);
		void DeleteAll();
		void RemoveAll() override;
	private:
		using IQueue<T>::myHead;
		using IQueue<T>::myTail;

		std::atomic<bool> myPushing;
		std::atomic<bool> myPoping;

		std::mutex myLock;
	};

	
}

template <typename T, unsigned int QueueLimit = 5000>
Container::CLocklessQueue<T, QueueLimit>::CLocklessQueue() : IQueue<T, QueueLimit>(), myPushing(false), myPoping(false)
{
}

template <typename T, unsigned int QueueLimit = 5000>
Container::CLocklessQueue<T, QueueLimit>::~CLocklessQueue()
{
	this->CLocklessQueue<T, QueueLimit>::RemoveAll();
}

template <typename T, unsigned int QueueLimit = 5000>
void Container::CLocklessQueue<T, QueueLimit>::Push(T aValue)
{
	typename IQueue<T, QueueLimit>::QueueItem* item = new typename IQueue<T, QueueLimit>::QueueItem;
	item->value = aValue;

	std::lock_guard<std::mutex> guard(myLock);
	if(IQueue<T, QueueLimit>::myHead == nullptr)
	{
		IQueue<T, QueueLimit>::myHead = item;
	}
	else
	{
		IQueue<T, QueueLimit>::myTail->next = item;
	}

	IQueue<T, QueueLimit>::myTail = item;

	IQueue<T, QueueLimit>::IncrementCount();

	IQueue<T, QueueLimit>::CheckQueueLength();
}

template <typename T, unsigned int QueueLimit = 5000>
void Container::CLocklessQueue<T, QueueLimit>::Append(IQueue<T, QueueLimit>& aQueue)
{
	if(aQueue.IsEmpty() == true)
	{
		return;
	}
	std::lock_guard<std::mutex> guard(myLock);

	IQueue<T, QueueLimit>::Append(aQueue);
}

template <typename T, unsigned int QueueLimit = 5000>
T Container::CLocklessQueue<T, QueueLimit>::Pop()
{

	typename IQueue<T, QueueLimit>::QueueItem* item = IQueue<T, QueueLimit>::myHead;
	
	if(item == nullptr)
	{
		assert(false && "Attempting to pop from empty queue.");
	}

	myLock.lock();
	IQueue<T, QueueLimit>::myHead = item->next;
	IQueue<T, QueueLimit>::DecrementCount();
	myLock.unlock();

	T value = item->value;
	delete item;

	return value;
}

template <typename T, unsigned int QueueLimit = 5000>
typename Container::CLocklessQueue<T, QueueLimit>::Error Container::CLocklessQueue<T, QueueLimit>::TryPush(const T aValue)
{
	myPushing = true;

	if(myPoping == true && IQueue<T, QueueLimit>::Size() <= 1)
	{
		myPushing = false;
		return Error::Blocked;
	}

	typename IQueue<T, QueueLimit>::QueueItem* item = new typename IQueue<T, QueueLimit>::QueueItem;
	item->value = aValue;

	std::lock_guard<std::mutex> guard(myLock);
	if (IQueue<T, QueueLimit>::myHead == nullptr)
	{
		IQueue<T, QueueLimit>::myHead = item;
	}
	else
	{
		IQueue<T, QueueLimit>::myTail->next = item;
	}

	IQueue<T, QueueLimit>::myTail = item;

	IQueue<T, QueueLimit>::IncrementCount();

	myPushing = false;

	return Error::Ok;
}

template <typename T, unsigned int QueueLimit = 5000>
typename Container::CLocklessQueue<T, QueueLimit>::Error Container::CLocklessQueue<T, QueueLimit>::TryPop(T& aValue)
{
	myPoping = true;

	if (myPushing == true && IQueue<T, QueueLimit>::Size() <= 1)
	{
		myPoping = false;
		return Error::Blocked;
	}

	typename IQueue<T, QueueLimit>::QueueItem* item = IQueue<T>::myHead;

	if(item == nullptr)
	{
		myPoping = false;
		return Error::Empty;
	}

	IQueue<T, QueueLimit>::myHead = item->next;
	
	IQueue<T, QueueLimit>::DecrementCount();

	myPoping = false;


	T value = item->value;
	delete item;

	aValue = value;

	return Error::Ok;
}

template <typename T, unsigned int QueueLimit = 5000>
void Container::CLocklessQueue<T, QueueLimit>::DeleteAll()
{
	std::lock_guard<std::mutex> lock(myLock);
	for (typename IQueue<T, QueueLimit>::QueueItem* item = IQueue<T, QueueLimit>::myHead; item != nullptr; )
	{
		IQueue<T, QueueLimit>::DecrementCount();
		typename IQueue<T, QueueLimit>::QueueItem* itemToDelete = item;
		item = item->next;
		delete itemToDelete->value;
		delete itemToDelete;
	}
	IQueue<T, QueueLimit>::myHead = nullptr;
	IQueue<T, QueueLimit>::myTail = nullptr;
}

template <typename T, unsigned int QueueLimit = 5000>
void Container::CLocklessQueue<T, QueueLimit>::RemoveAll()
{
	std::lock_guard<std::mutex> lock(myLock);
	IQueue<T, QueueLimit>::RemoveAll();
}
