#include "stdafx.h"
#include "LoadEnemyRunTowardsComponent.h"
#include "EnemyRunTowardsComponent.h"
#include "Enemy.h"

int LoadEnemyRunTowardsComponent(KLoader::SLoadedComponentData someData)
{
		CEnemyRunTowardsComponent* newEnemyRunTowardsComponent = new CEnemyRunTowardsComponent();
		CComponentManager::GetInstance().RegisterComponent(newEnemyRunTowardsComponent);
		CEnemy::AddEnemyRunTowardsComponent(newEnemyRunTowardsComponent);
		return newEnemyRunTowardsComponent->GetId();

	return NULL_COMPONENT;
}
