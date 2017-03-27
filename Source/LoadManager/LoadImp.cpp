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
	blueprint.speed = someData.myData.at("Speed").GetFloat();
	blueprint.detectionRange = someData.myData.at("DetectionRange").GetFloat();
	blueprint.startAttackRange = someData.myData.at("StartAttackRange").GetFloat();
	blueprint.stopAttackRange = someData.myData.at("StopAttackRange").GetFloat();
	blueprint.shouldGoMeleeRadius = someData.myData.at("GoingMeleeRange").GetFloat();
	blueprint.jumpHeight = someData.myData.at("JumpHeight").GetFloat();

	CComponent* component = enemyComponentManager->CreateComponentAbstract(&blueprint, ID++, eEnemyTypes::eImp);

	return component->GetId();
}