#include "stdafx.h"
#include "LoadRevenant.h"


int LoadRevenant(KLoader::SLoadedComponentData someData)
{

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
	if (someData.myData.HasKey("chargeRangedAttackDuration") == true)
	{
		// REMOVE THIS LATER, temp just incase LDs stuff were to get corrupted with the name change.
		blueprint.chargeRangedAttackAttackDuration = someData.myData.at("chargeRangedAttackDuration").GetFloat();
	}
	else
	{
		blueprint.chargeRangedAttackAttackDuration = 2.0f;
	}
	if (someData.myData.HasKey("chargeMeleeAttackDuration") == true)
	{
		// REMOVE THIS LATER, temp just incase LDs stuff were to get corrupted with the name change.
		blueprint.chargeMeleeAttackDuration = someData.myData.at("chargeMeleeAttackDuration").GetFloat();
	}
	else
	{
		blueprint.chargeMeleeAttackDuration = 1.0f;
	}
	if (someData.myData.HasKey("chargeAirBarrageAttackDuration") == true)
	{
		// REMOVE THIS LATER, temp just incase LDs stuff were to get corrupted with the name change.
		blueprint.chargeAirBarrageAttackDuration = someData.myData.at("chargeAirBarrageAttackDuration").GetFloat();
	}
	else
	{
		blueprint.chargeAirBarrageAttackDuration = 2.0f;
	}

	if (someData.myData.HasKey("ShootingRange") == true)
	{
		blueprint.shootingRange = someData.myData.at("ShootingRange").GetFloat();
	}
	else
	{
		blueprint.shootingRange = blueprint.detectionRange;
	}

	blueprint.flightHeight = someData.myData.at("FlightHeight").GetFloat();
	blueprint.hoverTime = someData.myData.at("HoverDuration").GetFloat();

	CComponent* component = enemyComponentManager->CreateComponentAbstract(&blueprint, eEnemyTypes::eRevenant);

	return component->GetId();
}