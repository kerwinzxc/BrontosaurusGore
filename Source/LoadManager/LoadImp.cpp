#include "stdafx.h"
#include "LoadImp.h"
#include "EnemyComponentManager.h"

int LoadImp(KLoader::SLoadedComponentData someData)
{
	static unsigned int ID = 0;


	GET_SERVERLOADMANAGER(loadManager);
	CEnemyComponentManager* enemyComponentManager = loadManager.GetCurrentGameServer().GetEnemyComponentManager();

	if (!enemyComponentManager)
	{

		return NULL_COMPONENT;
	}
	SImpBlueprint blueprint;
	blueprint.speed = someData.myData.at("speed").GetFloat();
	blueprint.detectionRange = someData.myData.at("detactionRange").GetFloat();
	blueprint.startAttackRange = someData.myData.at("startAttackRange").GetFloat();
	blueprint.stopAttackRange = someData.myData.at("stopAttackRange").GetFloat();
	blueprint.shouldGoMeleeRadius = someData.myData.at("goingMeleeRange").GetFloat();
	blueprint.jumpHeight = someData.myData.at("jumpHeight").GetFloat();

	CComponent* component = enemyComponentManager->CreateComponentAbstract(&blueprint, ID++, eEnemyTypes::eImp);

	return component->GetId();
}