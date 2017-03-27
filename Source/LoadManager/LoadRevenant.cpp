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
	blueprint.shouldGoMeleeRadius = someData.myData.at("GoingMeleeRange").GetFloat();
	blueprint.flightHeight = someData.myData.at("FlightHeight").GetFloat();
	blueprint.hoverTime = someData.myData.at("HoverDuration").GetFloat();

	CComponent* component = enemyComponentManager->CreateComponentAbstract(&blueprint, ID++, eEnemyTypes::eRevenant);

	return component->GetId();
}