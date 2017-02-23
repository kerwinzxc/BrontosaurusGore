#include "stdafx.h"
#include "EnemyComponentManager.h"
#include "Enemy.h"


CEnemyComponentManager* CEnemyComponentManager::ourInstance = nullptr;


CEnemyComponentManager& CEnemyComponentManager::GetInstance()
{
	assert(ourInstance != nullptr && "Model component manager not created (is NULL)");
	return *ourInstance;
}

CEnemyComponentManager::CEnemyComponentManager(CScene& aScene) : myScene(aScene)
{
	assert(ourInstance == nullptr);
	ourInstance = this;
	myEnemies.Init(10);
}

void CEnemyComponentManager::Update(const CU::Time& aDeltaTime)
{
	for(int i = 0; i < myEnemies.Size(); ++i)
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

void CEnemyComponentManager::DeleteComponent(Component::CEnemy* anEnemy)
{
	myEnemies.Delete(anEnemy);
}

CEnemyComponentManager::~CEnemyComponentManager()
{
	assert(ourInstance == this);
	ourInstance = nullptr;
}
