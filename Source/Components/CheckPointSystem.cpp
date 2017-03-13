#include "stdafx.h"
#include "CheckPointSystem.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"

CCheckPointSystem::CCheckPointSystem()
{
}


CCheckPointSystem::~CCheckPointSystem()
{
}

eMessageReturn CCheckPointSystem::DoEvent(const CAddToCheckPointResetList& aAddToCheckPointResetList)
{
	myObjectsToReset.Add(aAddToCheckPointResetList.GetObjectToReset());
}