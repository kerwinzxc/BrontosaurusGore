#include "stdafx.h"
#include "EnemyClientRepresentationManager.h"

CEnemyClientRepresentationManager* CEnemyClientRepresentationManager::ourInstance = nullptr;

CEnemyClientRepresentationManager& CEnemyClientRepresentationManager::GetInstance()
{
	
	return *ourInstance;
}

CEnemyClientRepresentation& CEnemyClientRepresentationManager::CreateAndRegister(unsigned int anId)
{
	CEnemyClientRepresentation* rep = new CEnemyClientRepresentation(anId);
	myRepresentations[anId] = rep;
	CComponentManager::GetInstance().RegisterComponent(rep);
	return *rep;
}

CEnemyClientRepresentation& CEnemyClientRepresentationManager::GetRepresentation(unsigned int aId)
{
	return *myRepresentations.at(aId);
}

void CEnemyClientRepresentationManager::Create()
{
	if(ourInstance != nullptr)
	{
		Destroy();
	}
	ourInstance = new CEnemyClientRepresentationManager;
}

void CEnemyClientRepresentationManager::Destroy()
{
	SAFE_DELETE(ourInstance);
}

CEnemyClientRepresentationManager::CEnemyClientRepresentationManager()
{
}


CEnemyClientRepresentationManager::~CEnemyClientRepresentationManager()
{
}

void CEnemyClientRepresentationManager::Update(const CU::Time& aDeltaTime)
{
	for (auto enemyRepresentation : myRepresentations)
	{
		myRepresentations.at(enemyRepresentation.first)->Update(aDeltaTime.GetSeconds());
	}
}

void CEnemyClientRepresentationManager::Init()
{
	for (auto enemyRepresentation : myRepresentations)
	{
		myRepresentations.at(enemyRepresentation.first)->Init();
	}
}