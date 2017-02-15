#include "stdafx.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "ComponentManager.h"
#include "ComponentMessage.h"

CGameObjectManager::CGameObjectManager()
{
	InitGrowingArrays();
}

void CGameObjectManager::InitGrowingArrays()
{
	myMatrices.Init(128);
	myObjectsAboutToBeDestroyed.Init(2);
	myObjectsCreated.Init(1024);
}

CGameObjectManager::~CGameObjectManager()
{
//	ClearAll();
	myFreeMatrices.Clear();
	myMatrices.RemoveAll();
}


CGameObject* CGameObjectManager::CreateGameObject(const CU::Matrix44f& aMatrix)
{
	CGameObject* object;
	object = new CGameObject(*this);

	CComponentManager::GetInstance().RegisterComponent(object);

	myObjectsCreated.Add(object);
	
	object->myTransformId = CreateMatrix(aMatrix);

	return  object;
}

CU::Matrix44f& CGameObjectManager::GetTransform(CGameObject& aGameObject)
{
	return myMatrices[aGameObject.myTransformId];
}

void CGameObjectManager::DestroyObject(CGameObject* aGameObject)
{
	if (aGameObject == nullptr)
	{
		DL_ASSERT("trying to destroy a nullponter as an gameobject");
	}

	myMatrices[aGameObject->myTransformId] = CU::Matrix44f::Identity;
	myFreeMatrices.Push(aGameObject->myTransformId);

	CComponentManager::GetInstance().DeleteComponent(aGameObject->GetId());

	myObjectsCreated.RemoveCyclic(aGameObject);
}

void CGameObjectManager::AddObjectForDestruction(CGameObject* aGameObject)
{
	myObjectsAboutToBeDestroyed.Add(aGameObject);
}

void CGameObjectManager::DestroyObjectsWaitingForDestruction()
{
	for (unsigned int i = 0; i < myObjectsAboutToBeDestroyed.Size(); ++i)
	{
		myObjectsAboutToBeDestroyed[i]->Destroy();
	}
	myObjectsAboutToBeDestroyed.RemoveAll();
}

void CGameObjectManager::ClearAll()
{
	myObjectsCreated.DeleteAll();
}

void CGameObjectManager::DumpAllAndReInit()
{
	myMatrices.Destroy();
	myFreeMatrices.Clear();

	myObjectsCreated.DeleteAll();
	myObjectsCreated.Destroy();

	myObjectsAboutToBeDestroyed.Destroy();
	
	InitGrowingArrays();
}

void CGameObjectManager::SendObjectsDoneMessage()
{
	SComponentMessageData data;
	//CU::GrowingArray<ComponentId> searchonIDs;
	//searchonIDs.Init(244);
	
	for (unsigned short i = 0; i < myObjectsCreated.Size(); i++)
	{
		//searchonIDs.Add(myObjectsCreated[i]->GetId());

		myObjectsCreated[i]->NotifyOnlyComponents(eComponentMessageType::eObjectDone, data);
	}
}

ComponentId CGameObjectManager::CreateMatrix(const CU::Matrix44f& aMatrix44f)
{
	if (myFreeMatrices.Size() == 0)
	{
		const ComponentId oldSize = myMatrices.Size();
		myMatrices.Add(aMatrix44f);
		return oldSize;
	}

	const ComponentId freeSlot = myFreeMatrices.Pop();
	myMatrices[freeSlot] = aMatrix44f;

	return freeSlot;
}

