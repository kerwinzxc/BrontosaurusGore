#include "stdafx.h"
#include "EnemyComponentManager.h"
#include "Enemy.h"
#include "ImpController.h"
#include "RevenantController.h"
#include "PinkyController.h"
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
		CEnemy* enemy = new CEnemy(anId, aType);
		enemy->SetEnemyData(anEnemyBlueprint);

		myEnemies.Add(enemy);

		CComponentManager::GetInstance().RegisterComponent(enemy);

		return enemy;
		break;

	}
	case eEnemyTypes::eImp:
	{
		CImpController* enemy = new CImpController(anId, aType);
		enemy->SetEnemyData(anEnemyBlueprint);

		myEnemies.Add(enemy);

		CComponentManager::GetInstance().RegisterComponent(enemy);

		return enemy;
		break;
	
	}
	case eEnemyTypes::eRevenant:
	{
		CRevenantController* enemy = new CRevenantController(anId, aType);
		enemy->SetEnemyData(anEnemyBlueprint);

		myEnemies.Add(enemy);

		CComponentManager::GetInstance().RegisterComponent(enemy);

		return enemy;
		break;

	}
	case eEnemyTypes::ePinky:
	{
		CPinkyController* enemy = new CPinkyController(anId, aType);
		enemy->SetEnemyData(anEnemyBlueprint);

		myEnemies.Add(enemy);

		CComponentManager::GetInstance().RegisterComponent(enemy);

		return enemy;
		break;
	}
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
