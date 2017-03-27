#include "stdafx.h"
#include "LoadRevenant.h"


int LoadRevenant(KLoader::SLoadedComponentData someData)
{
	static unsigned int ID = 0;


	GET_SERVERLOADMANAGER(loadManager);
	CEnemyComponentManager* enemyComponentManager = loadManager.GetCurrentGameServer().GetEnemyComponentManager();

	if (!enemyComponentManager)
	{

		return NULL_COMPONENT;
	}
	SRevenantBlueprint blueprint;
	blueprint.speed = someData.myData.at("speed").GetFloat();
	blueprint.detectionRange = someData.myData.at("detactionRange").GetFloat();
	blueprint.startAttackRange = someData.myData.at("startAttackRange").GetFloat();
	blueprint.stopAttackRange = someData.myData.at("stopAttackRange").GetFloat();
	blueprint.shouldGoMeleeRadius = someData.myData.at("goingMeleeRange").GetFloat();
	blueprint.flightHeight = someData.myData.at("flightHeight").GetFloat();
	blueprint.hoverTime = someData.myData.at("hoverDuration").GetFloat();

	CComponent* component = enemyComponentManager->CreateComponentAbstract(&blueprint, ID++, eEnemyTypes::eRevenant);

	return component->GetId();
}