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
	blueprint.speed = someData.myData.at("Speed").GetFloat();
	blueprint.detectionRange = someData.myData.at("DetectionRange").GetFloat();
	blueprint.startAttackRange = someData.myData.at("StartAttackRange").GetFloat();
	blueprint.stopAttackRange = someData.myData.at("StopAttackRange").GetFloat();

	if (someData.myData.HasKey("GoingMeleeRange") == true)
	{
		// REMOVE THIS LATER, temp just incase LDs stuff were to get corrupted with the name change.
		blueprint.walkToMeleeRange = someData.myData.at("GoingMeleeRange").GetFloat();
	}
	else
	{
		blueprint.walkToMeleeRange = someData.myData.at("WalkToMeleeRange").GetFloat();
	}

	blueprint.flightHeight = someData.myData.at("FlightHeight").GetFloat();
	blueprint.hoverTime = someData.myData.at("HoverDuration").GetFloat();

	CComponent* component = enemyComponentManager->CreateComponentAbstract(&blueprint, ID++, eEnemyTypes::eRevenant);

	return component->GetId();
}