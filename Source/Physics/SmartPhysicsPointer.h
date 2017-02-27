#pragma once
#include <atomic>

namespace Physics
{
	template<typename T>
	class ISmartPhysicsPointer
	{
	public:
		ISmartPhysicsPointer();
		explicit ISmartPhysicsPointer(T*const aValue);
		ISmartPhysicsPointer(const ISmartPhysicsPointer& aSmartPointer);
		virtual ~ISmartPhysicsPointer();

		const ISmartPhysicsPointer& operator==(const ISmartPhysicsPointer& aSmartPointer);

		const unsigned int GetCount() const;
	protected:
		virtual void Remove();

		std::atomic<unsigned int>* myCounter;
		T* myValue;
	};
}

template <typename T>
Physics::ISmartPhysicsPointer<T>::ISmartPhysicsPointer() : myCounter(nullptr), myValue(nullptr)
{
}

template <typename T>
Physics::ISmartPhysicsPointer<T>::ISmartPhysicsPointer(T*const aValue): 
myCounter(new std::atomic<unsigned int>(1)), 
myValue(aValue)
{
}

template <typename T>
Physics::ISmartPhysicsPointer<T>::ISmartPhysicsPointer(const ISmartPhysicsPointer& aSmartPointer) :
myCounter(aSmartPointer.myCounter),
myValue(aSmartPointer.myValue)
{
	++(*myCounter);
}

template <typename T>
Physics::ISmartPhysicsPointer<T>::~ISmartPhysicsPointer()
{
	--(*myCounter);

	if((*myCounter) == 0)
	{
		ISmartPhysicsPointer<T>::Remove();
	}
}

template <typename T>
const Physics::ISmartPhysicsPointer<T>& Physics::ISmartPhysicsPointer<T>::operator==(const ISmartPhysicsPointer& aSmartPointer)
{
	myCounter = aSmartPointer.myCounter;
	++(*myCounter);

	return *this;
}

template <typename T>
const unsigned Physics::ISmartPhysicsPointer<T>::GetCount() const
{
	return *myCounter;
}

template <typename T>
void Physics::ISmartPhysicsPointer<T>::Remove()
{
	myValue->release();
}
