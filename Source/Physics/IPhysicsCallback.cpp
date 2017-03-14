#include "stdafx.h"
#include "IPhysicsCallback.h"


namespace Physics
{
	IPhysicsCallback::IPhysicsCallback()
	{
		myUserData = nullptr;
	}

	IPhysicsCallback::~IPhysicsCallback()
	{
		myUserData = nullptr;
	}
}
