#include "stdafx.h"
#include "FireEmitterManager.h"
#include "FireEmitter.h"

CFireEmitterManager::CFireEmitterManager()
	: myFireEmitters(8u)
{
}

CFireEmitterManager::~CFireEmitterManager()
{
}

FireEmitterID CFireEmitterManager::CreateFireEmitter(const SFireEmitterData& aFireEmitterData)
{
	FireEmitterID fireEmitter = myFireEmitters.Size();

	if (myFreeIDs.Size() > 0)
	{
		fireEmitter = myFreeIDs.Pop();
	}
	else
	{
		myFireEmitters.Add(CFireEmitter());
	}

	myFireEmitters[fireEmitter].Init(aFireEmitterData);
	myFireEmitters[fireEmitter].AddRef();

	return fireEmitter;
}

int CFireEmitterManager::CopyFireEmitter(const FireEmitterID aFireEmitter)
{
	if (aFireEmitter >= 0 && aFireEmitter < myFireEmitters.Size())
	{
		if (myFireEmitters[aFireEmitter].IsInitialized() == true)
		{
			return myFireEmitters[aFireEmitter].AddRef();
		}
	}

	return -1;
}

int CFireEmitterManager::DestroyFireEmitter(const FireEmitterID aFireEmitter)
{
	int refCount = myFireEmitters[aFireEmitter].DecRef();
	if (refCount <= 0)
	{
		myFireEmitters[aFireEmitter].Destroy();
		myFreeIDs.Push(aFireEmitter);
	}

	return refCount;
}

CFireEmitter* CFireEmitterManager::GetFireEmitter(const FireEmitterID aID)
{
	if (aID >= 0 && aID < myFireEmitters.Size())
	{
		if (myFireEmitters[aID].IsInitialized() == true)
		{
			return myFireEmitters.AsPointer(aID);
		}
	}

	return nullptr;
}
