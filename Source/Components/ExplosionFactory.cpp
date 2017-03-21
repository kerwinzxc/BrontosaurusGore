#include "stdafx.h"
#include "ExplosionFactory.h"
#include "GameObjectManager.h"
#include "ExplosionComponentManager.h"
#include "ModelComponentManager.h"
#include "ExplosionBufferData.h"
#include "../ThreadedPostmaster/CreateExplosionMessage.h"'
#include "../ThreadedPostmaster/DeactivateExplosionMessage.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SetAsNewCheckPointMessage.h"
#include "ExplosionComponent.h"
#include "ColliderComponentManager.h"

CExplosionFactory::CExplosionFactory(CExplosionComponentManager* aExplosionComponentManager)
{
	myPassiveExplosions.Init(10);
	myActiveExplosions.Init(10);
	myExplosionComponentsManager = aExplosionComponentManager;
}


CExplosionFactory::~CExplosionFactory()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

void CExplosionFactory::Init(CGameObjectManager* aGameObjectManager, CModelComponentManager* aModelComponentManagerPointer, CColliderComponentManager* aCColliderComponentManager)
{
	myGameObjectManagerPointer = aGameObjectManager;
	myModelComponentManagerPointer = aModelComponentManagerPointer;
	myColliderComponentManagerPointer = aCColliderComponentManager;

	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eCreateExplosionMessage);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eDeactivateExplosion);
}

eMessageReturn CExplosionFactory::DoEvent(const CCreateExplosionMessage& aCreateExplosionMessage)
{
	if(myPassiveExplosions.Size() > 0)
	{
		myActiveExplosions.Add(myPassiveExplosions[0]);
		myPassiveExplosions.RemoveCyclicAtIndex(0);
		SComponentMessageData EXuPlosssiioooooooonData;
		EXuPlosssiioooooooonData.myExplosionData = myActiveExplosions.GetLast()->data;
		myActiveExplosions.GetLast()->gameObject->NotifyComponents(eComponentMessageType::eActivateExplosion, EXuPlosssiioooooooonData);
		SComponentMessageData visibilityData;
		visibilityData.myBool = true;
		myActiveExplosions.GetLast()->gameObject->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
	}
	else
	{
		CreateExplosion();
		DoEvent(aCreateExplosionMessage);
	}
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

void CExplosionFactory::CreateExplosion()
{
	CGameObject* newExplosionObject = myGameObjectManagerPointer->CreateGameObject();
	CExplosionComponent* tempExplosionComponent = myExplosionComponentsManager->CreateAndRegisterComponent();
	newExplosionObject->AddComponent(tempExplosionComponent);
	CModelComponent* modelComponent = myModelComponentManagerPointer->CreateComponent("Models/chromeBall/chromeBall.fbx");
	newExplosionObject->AddComponent(modelComponent);
	newExplosionObject->NotifyOnlyComponents(eComponentMessageType::eMoving, SComponentMessageData());
	SComponentMessageData visibilityData;
	visibilityData.myBool = false;
	newExplosionObject->NotifyOnlyComponents(eComponentMessageType::eSetVisibility, visibilityData);

	SRigidBodyData explosionRigidBodyData;
	explosionRigidBodyData.isKinematic = false;
	explosionRigidBodyData.angularDrag = 0.5f;
	explosionRigidBodyData.mass = 0.f;
	CColliderComponent* projectileCollider = myColliderComponentManagerPointer->CreateComponent(&explosionRigidBodyData);
	newExplosionObject->AddComponent(projectileCollider);
	SExplosionBufferData* newSexplosionData = new SExplosionBufferData();
	newSexplosionData->gameObject = newExplosionObject;
	myPassiveExplosions.Add(newSexplosionData);
}