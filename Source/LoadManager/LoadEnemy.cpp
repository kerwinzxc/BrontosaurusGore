#include "stdafx.h"
#include "LoadEnemy.h"
#include "EnemyComponentManager.h"
//#include "Enemy.h"
#include "Component.h"

int LoadEnemy(KLoader::SLoadedComponentData someData)
{
	GET_LOADMANAGER(loadManager);
	CEnemyComponentManager* enemyComponentManager = loadManager.GetCurrentPLaystate().GetEnemyComponentManager();
	if (!enemyComponentManager) return NULL_COMPONENT;

	CEnemyComponentManager::EnemyBlueprint blueprint;
	blueprint.health = someData.myData.at("health").GetUInt();
	blueprint.speed = someData.myData.at("speed").GetFloat();
	blueprint.detactionRange = someData.myData.at("detactionRange").GetFloat();
	blueprint.startAttackRange = someData.myData.at("startAttackRange").GetFloat();
	blueprint.stopAttackRange = someData.myData.at("stopAttackRange").GetFloat();

	CComponent* component = enemyComponentManager->CreateComponentAbstract(blueprint);

	return component->GetId();
}
