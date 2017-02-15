#include "stdafx.h"
#include "AudioSourceComponentManager.h"
#include "ComponentManager.h"


CAudioSourceComponentManager* CAudioSourceComponentManager::myInstance = nullptr;

CAudioSourceComponentManager::CAudioSourceComponentManager()
{
	myComponents.Init(64);
}


CAudioSourceComponentManager::~CAudioSourceComponentManager()
{
	RemoveAll();
}

CAudioSourceComponent * CAudioSourceComponentManager::CreateComponent()
{
	Audio::GameObjectID AudioObjectID = Audio::CAudioInterface::GetInstance()->RegisterGameObject();
	myIds[AudioObjectID] = myComponents.Size();
	myComponents.Add(new CAudioSourceComponent());
	myComponents.GetLast()->myGameObjectID = AudioObjectID;
	
	CComponentManager::GetInstance().RegisterComponent(myComponents.GetLast());

	return myComponents.GetLast();
}

CAudioSourceComponentManager& CAudioSourceComponentManager::GetInstance()
{
	assert(myInstance != nullptr && "audio source component manager not created (is NULL)");
	return *myInstance;
}

void CAudioSourceComponentManager::Create()
{
	assert(myInstance == nullptr && "audio source component manager already created");
	myInstance = new CAudioSourceComponentManager();
}

void CAudioSourceComponentManager::Destroy()
{
	assert(myInstance != nullptr && "audio source component manager not created (is NULL)");
	SAFE_DELETE(myInstance);
}

void CAudioSourceComponentManager::Update()
{
	for (unsigned int i = 0; i < myComponents.Size(); ++i)
	{
		myComponents[i]->Update();
	}
}

void CAudioSourceComponentManager::RemoveAll()
{
	myIds.clear();
	//for (unsigned int i = 0; i < myComponents.Size(); ++i)
	//{
	//	CComponentManager::GetInstance().DeleteComponent(myComponents.GetLast().GetId());
	//}

	myComponents.RemoveAll();
	Audio::CAudioInterface::GetInstance()->UnregisterAllGameOBjects();
}

void CAudioSourceComponentManager::Remove(CAudioSourceComponent* aComponentToRemove)
{
	//Audio::CAudioInterface::GetInstance()->UnregisterGameObject(aComponentToRemove->myGameObjectID);
	//CComponentManager::GetInstance().DeleteComponent(aComponentToRemove->GetId());
	myComponents.RemoveAtIndex(myIds[aComponentToRemove->myGameObjectID]);
	myIds.erase(aComponentToRemove->myGameObjectID);
}
