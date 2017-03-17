#include "stdafx.h"
#include "ExplosionFactory.h"
#include "GameObjectManager.h"
#include "ExplosionComponentManager.h"
#include "ModelComponentManager.h"
#include "../ThreadedPostmaster/CreateExplosionMessage.h"'
#include "../ThreadedPostmaster/DeactivateExplosionMessage.h"
#include "ExplosionBufferData.h"

CExplosionFactory::CExplosionFactory(CExplosionComponentManager* aExplosionComponentManager)
{
	myPassiveExplosions.Init(10);
	myActiveExplosions.Init(10);
	myExplosionComponentsManager = aExplosionComponentManager;
}


CExplosionFactory::~CExplosionFactory()
{
}

void CExplosionFactory::Init(CGameObjectManager* aGameObjectManager, CModelComponentManager* aModelComponentManagerPointer)
{
	myGameObjectManagerPointer = aGameObjectManager;
	myModelComponentManagerPointer = aModelComponentManagerPointer;
}

eMessageReturn CExplosionFactory::DoEvent(const CCreateExplosionMessage& aCreateExplosionMessage)
{
	
	return eMessageReturn::eContinue;
}

eMessageReturn CExplosionFactory::DoEvent(const CDeactivateExplosionMessage& aDeactivateExplosionMessage)
{
	for(unsigned int i = 0; i < myActiveExplosions.Size(); i++)
	{
		if(myActiveExplosions[i]->gameObject == aDeactivateExplosionMessage.GetGameObject())
		{
			myPassiveExplosions.Add(myActiveExplosions[i]);
			myActiveExplosions.RemoveCyclicAtIndex(i);
			break;
		}
	}
	return eMessageReturn::eContinue;
}