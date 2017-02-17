#include "stdafx.h"
#include "InputListener.h"

namespace CU
{
	IInputListener::IInputListener()
		: myPriority(0)
	{
	}

	IInputListener::~IInputListener()
	{
	}

	void IInputListener::SetPriority(const int aPriority)
	{
		myPriority = aPriority;
	}

	int IInputListener::GetPriority() const
	{
		return myPriority;
	}
}
