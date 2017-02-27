#pragma once
#include "SmartPhysicsPointer.h"

namespace Physics
{
	template<typename T>
	class CSharedPhysicsPointer : ISmartPhysicsPointer<T>
	{
		using  ISmartPhysicsPointer<T>::myValue;
	public:
		CSharedPhysicsPointer();
		explicit CSharedPhysicsPointer(T*const aValue);
		CSharedPhysicsPointer(const CSharedPhysicsPointer& aPointer);
		const CSharedPhysicsPointer& operator==(const CSharedPhysicsPointer& aPointer);
	protected:
	};

	template <typename T>
	CSharedPhysicsPointer<T>::CSharedPhysicsPointer() : ISmartPhysicsPointer<T>()
	{
	}

	template <typename T>
	CSharedPhysicsPointer<T>::CSharedPhysicsPointer(T* const aValue) : ISmartPhysicsPointer<T>(aValue)
	{
	}

	template <typename T>
	CSharedPhysicsPointer<T>::CSharedPhysicsPointer(const CSharedPhysicsPointer& aPointer) : ISmartPhysicsPointer<T>(aPointer)
	{
	}

	template <typename T>
	const CSharedPhysicsPointer<T>& CSharedPhysicsPointer<T>::operator==(const CSharedPhysicsPointer& aPointer)
	{
		ISmartPhysicsPointer<T>::operator==(aPointer);
	}
}
