#include "stdafx.h"
#include "EnemyComponentManager.h"
#include "Enemy.h"

CEnemyComponentManager::CEnemyComponentManager(CScene& aScene) : myScene(aScene)
{
	myEnemies.Init(10);
}

void CEnemyComponentManager::Update(const CU::Time& aDeltaTime)
{
	for(unsigned int i = 0; i < myEnemies.Size(); ++i)
	{
		myEnemies[i]->Update(aDeltaTime);
	}
}

Component::CEnemy* CEnemyComponentManager::CreateComponent(const EnemyBlueprint& anEnemyBlueprint)
{
	Component::CEnemy* enemy = new Component::CEnemy();
	enemy->SetHealth(anEnemyBlueprint.health);
	enemy->SetSpeed(anEnemyBlueprint.speed);
	enemy->SetDetectionRange(anEnemyBlueprint.detactionRange);
	enemy->SetStartAttackRange(anEnemyBlueprint.startAttackRange);
	enemy->SetStopAttackRange(anEnemyBlueprint.stopAttackRange);

	myEnemies.Add(enemy);

	CComponentManager::GetInstance().RegisterComponent(enemy);

	return enemy;
}

CComponent* CEnemyComponentManager::CreateComponentAbstract(const EnemyBlueprint& anEnemyBlueprint)
{
	return CreateComponent(anEnemyBlueprint);
}

void CEnemyComponentManager::DeleteComponent(Component::CEnemy* anEnemy)
{
	myEnemies.DeleteCyclic(anEnemy);
}

CEnemyComponentManager::~CEnemyComponentManager()
{
}
