#include "stdafx.h"
#include "EnemyComponentManager.h"
#include "Enemy.h"

CEnemyComponentManager::CEnemyComponentManager()
{
	myEnemies.Init(10);
}

void CEnemyComponentManager::Update(const float aDeltaTime)
{
	for(unsigned int i = 0; i < myEnemies.Size(); ++i)
	{
		myEnemies[i]->Update(aDeltaTime);
	}
}

CEnemy* CEnemyComponentManager::CreateComponent(const SEnemyBlueprint* anEnemyBlueprint, unsigned int anId, eEnemyTypes aType)
{
	switch (aType)
	{
	case eEnemyTypes::eDefault:
	{
		CEnemy* enemy = new CEnemy(anId);
		enemy->SetEnemyData(anEnemyBlueprint);

		myEnemies.Add(enemy);

		CComponentManager::GetInstance().RegisterComponent(enemy);

		return enemy;
		break;

	}
	case eEnemyTypes::eImp:
	{
		CEnemy* enemy = new CEnemy(anId);
		enemy->SetEnemyData(anEnemyBlueprint);

		myEnemies.Add(enemy);

		CComponentManager::GetInstance().RegisterComponent(enemy);

		return enemy;
		break;
	
	}
	case eEnemyTypes::eRevenant:
		break;
	case eEnemyTypes::ePinky:
		break;
	default:
		break;
	}

	DL_ASSERT("invalid type sent to enemycomponentManger");
	return nullptr;
}

CComponent* CEnemyComponentManager::CreateComponentAbstract(const SEnemyBlueprint* anEnemyBlueprint, unsigned int anId, eEnemyTypes aType)
{
	return CreateComponent(anEnemyBlueprint,anId, aType);
}

void CEnemyComponentManager::DeleteComponent(CEnemy* anEnemy)
{
	myEnemies.DeleteCyclic(anEnemy);
}

CEnemyComponentManager::~CEnemyComponentManager()
{
}
