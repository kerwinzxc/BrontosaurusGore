#include "stdafx.h"
#include "LoadEnemy.h"
#include "EnemyComponentManager.h"
#include "Enemy.h"
#include "Component.h"

int LoadEnemy(KLoader::SLoadedComponentData someData)
{

	CEnemyComponentManager::EnemyBlueprint blueprint;
	blueprint.health = someData.myData.at("health").GetUInt();
	blueprint.speed = someData.myData.at("speed").GetFloat();
	blueprint.detactionRange = someData.myData.at("detactionRange").GetFloat();
	blueprint.startAttackRange = someData.myData.at("startAttackRange").GetFloat();
	blueprint.stopAttackRange = someData.myData.at("stopAttackRange").GetFloat();
	CComponent* component = CEnemyComponentManager::GetInstance().CreateComponent(blueprint);

	return component->GetId();
}
