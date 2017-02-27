#pragma once
#include <mutex>
#include "Queue.h"
#include <cassert>

//TODO: Check if there is a way to make this better
namespace Container
{
	template<typename T>
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
		void Append(IQueue<T>& aQueue) override;
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

template <typename T>
Container::CLocklessQueue<T>::CLocklessQueue() : IQueue<T>(), myPushing(false), myPoping(false)
{
}

template <typename T>
Container::CLocklessQueue<T>::~CLocklessQueue()
{
	this->CLocklessQueue<T>::RemoveAll();
}

template <typename T>
void Container::CLocklessQueue<T>::Push(T aValue)
{
	typename IQueue<T>::QueueItem* item = new typename IQueue<T>::QueueItem;
	item->value = aValue;

	std::lock_guard<std::mutex> guard(myLock);
	if(IQueue<T>::myHead == nullptr)
	{
		IQueue<T>::myHead = item;
	}
	else
	{
		IQueue<T>::myTail->next = item;
	}

	IQueue<T>::myTail = item;

	IQueue<T>::IncrementCount();
}

template <typename T>
void Container::CLocklessQueue<T>::Append(IQueue<T>& aQueue)
{
	if(aQueue.IsEmpty() == true)
	{
		return;
	}
	std::lock_guard<std::mutex> guard(myLock);

	IQueue<T>::Append(aQueue);
}

template <typename T>
T Container::CLocklessQueue<T>::Pop()
{

	typename IQueue<T>::QueueItem* item = IQueue<T>::myHead;
	
	if(item == nullptr)
	{
		assert(false && "Attempting to pop from empty queue.");
	}

	myLock.lock();
	IQueue<T>::myHead = item->next;
	IQueue<T>::DecrementCount();
	myLock.unlock();

	T value = item->value;
	delete item;

	return value;
}

template <typename T>
typename Container::CLocklessQueue<T>::Error Container::CLocklessQueue<T>::TryPush(const T aValue)
{
	myPushing = true;

	if(myPoping == true && IQueue<T>::Size() <= 1)
	{
		myPushing = false;
		return Error::Blocked;
	}

	typename IQueue<T>::QueueItem* item = new typename IQueue<T>::QueueItem;
	item->value = aValue;

	std::lock_guard<std::mutex> guard(myLock);
	if (IQueue<T>::myHead == nullptr)
	{
		IQueue<T>::myHead = item;
	}
	else
	{
		IQueue<T>::myTail->next = item;
	}

	IQueue<T>::myTail = item;

	IQueue<T>::IncrementCount();

	myPushing = false;

	return Error::Ok;
}

template <typename T>
typename Container::CLocklessQueue<T>::Error Container::CLocklessQueue<T>::TryPop(T& aValue)
{
	myPoping = true;

	if (myPushing == true && IQueue<T>::Size() <= 1)
	{
		myPoping = false;
		return Error::Blocked;
	}

	typename IQueue<T>::QueueItem* item = IQueue<T>::myHead;

	if(item == nullptr)
	{
		myPoping = false;
		return Error::Empty;
	}

	IQueue<T>::myHead = item->next;
	
	IQueue<T>::DecrementCount();

	myPoping = false;


	T value = item->value;
	delete item;

	aValue = value;

	return Error::Ok;
}

template <typename T>
void Container::CLocklessQueue<T>::DeleteAll()
{
	std::lock_guard<std::mutex> lock(myLock);
	for (typename IQueue<T>::QueueItem* item = IQueue<T>::myHead; item != nullptr; )
	{
		IQueue<T>::DecrementCount();
		typename IQueue<T>::QueueItem* itemToDelete = item;
		item = item->next;
		delete itemToDelete->value;
		delete itemToDelete;
	}
	IQueue<T>::myHead = nullptr;
	IQueue<T>::myTail = nullptr;
}

template <typename T>
void Container::CLocklessQueue<T>::RemoveAll()
{
	std::lock_guard<std::mutex> lock(myLock);
	IQueue<T>::RemoveAll();
}
