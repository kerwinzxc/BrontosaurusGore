#include "stdafx.h"
#include "LoadEnemy.h"
#include "EnemyComponentManager.h"
//#include "Enemy.h"
#include "Component.h"
#include "EnemyClientRepresentation.h"
#include "EnemyClientRepresentationManager.h"

int LoadEnemy(KLoader::SLoadedComponentData someData)
{
	static unsigned int ID = 0;

	GET_SERVERLOADMANAGER(loadManager);
	CEnemyComponentManager* enemyComponentManager = loadManager.GetCurrentGameServer().GetEnemyComponentManager();
	
	if (!enemyComponentManager) return NULL_COMPONENT;

	CEnemyComponentManager::EnemyBlueprint blueprint;
	blueprint.health = someData.myData.at("health").GetUInt();
	blueprint.speed = someData.myData.at("speed").GetFloat();
	blueprint.detactionRange = someData.myData.at("detactionRange").GetFloat();
	blueprint.startAttackRange = someData.myData.at("startAttackRange").GetFloat();
	blueprint.stopAttackRange = someData.myData.at("stopAttackRange").GetFloat();

	CComponent* component = enemyComponentManager->CreateComponentAbstract(blueprint, ID++);
	
	return component->GetId();
}

int ClientLoadEnemy(KLoader::SLoadedComponentData someData)
{
	//client sidan

	static unsigned int ID = 0;

	CEnemyClientRepresentation& rep = CEnemyClientRepresentationManager::GetInstance().CreateAndRegister(ID++);

	return rep.GetId();
}
