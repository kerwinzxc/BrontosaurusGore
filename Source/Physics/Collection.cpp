#include "stdafx.h"
#include "Collection.h"
#include <PxFiltering.h>
#include <common/PxCollection.h>

Physics::CCollection::CCollection(physx::PxCollection* aCollection) : myCollection(aCollection)
{
}

Physics::CCollection::~CCollection()
{
}

physx::PxCollection* Physics::CCollection::GetCollection()
{
	return myCollection;
}

