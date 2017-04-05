#include "stdafx.h"
#include "LoadPinky.h"
#include "EnemyComponentManager.h"

int LoadPinky(KLoader::SLoadedComponentData someData)
{

	GET_SERVERLOADMANAGER(loadManager);
	CEnemyComponentManager* enemyComponentManager = loadManager.GetCurrentGameServer().GetEnemyComponentManager();

	if (!enemyComponentManager)
	{

		return NULL_COMPONENT;
	}
	SPinkyBlueprint blueprint;
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
	if (someData.myData.HasKey("chargeMeleeAttackDuration") == true)
	{
		// REMOVE THIS LATER, temp just incase LDs stuff were to get corrupted with the name change.
		blueprint.chargeMeleeAttackDuration = someData.myData.at("chargeMeleeAttackDuration").GetFloat();
	}
	else
	{
		blueprint.chargeMeleeAttackDuration = 1.0f;
	}

	if (someData.myData.HasKey("ChargeRange") == true)
	{
		blueprint.shootingRange = someData.myData.at("ChargeRange").GetFloat();
	}
	else
	{
		blueprint.shootingRange = blueprint.detectionRange;
	}

	blueprint.chargeDamage = static_cast<healthPoint>(someData.myData.at("ChargeDamage").GetFloat());
	blueprint.chargeSpeed = someData.myData.at("ChargeSpeed").GetFloat();
	blueprint.chargeDistance = someData.myData.at("ChargeDistance").GetFloat();
	blueprint.windupChargeTime = someData.myData.at("WindUpChargeDuration").GetFloat();

	CComponent* component = enemyComponentManager->CreateComponentAbstract(&blueprint, eEnemyTypes::ePinky);

	return component->GetId();
}