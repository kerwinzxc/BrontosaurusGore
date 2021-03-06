#include "stdafx.h"
#include "LoadImp.h"
#include "EnemyComponentManager.h"

int LoadImp(KLoader::SLoadedComponentData someData)
{

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

	if (someData.myData.HasKey("GoingMeleeRange") == true)
	{
		// REMOVE THIS LATER, temp just incase LDs stuff were to get corrupted with the name change.
		blueprint.walkToMeleeRange = someData.myData.at("GoingMeleeRange").GetFloat();
	}
	else
	{
		blueprint.walkToMeleeRange = someData.myData.at("WalkToMeleeRange").GetFloat();
	}
	if (someData.myData.HasKey("wanderDistance") == true)
	{
		// REMOVE THIS LATER, temp just incase LDs stuff were to get corrupted with the name change.
		blueprint.wanderDistance = someData.myData.at("wanderDistance").GetFloat();
	}
	else
	{
		blueprint.wanderDistance = 5.0f;
	}
	if (someData.myData.HasKey("wanderDuration") == true)
	{
		// REMOVE THIS LATER, temp just incase LDs stuff were to get corrupted with the name change.
		blueprint.wanderDuration = someData.myData.at("wanderDuration").GetFloat();
	}
	else
	{
		blueprint.wanderDuration = 3.0f;
	}
	if (someData.myData.HasKey("wanderAngle") == true)
	{
		// REMOVE THIS LATER, temp just incase LDs stuff were to get corrupted with the name change.
		blueprint.wanderAngle = someData.myData.at("wanderAngle").GetUchar();
	}
	else
	{
		blueprint.wanderAngle = 120.0f;
	}
	if (someData.myData.HasKey("attacksUntillRunningAway") == true)
	{
		// REMOVE THIS LATER, temp just incase LDs stuff were to get corrupted with the name change.
		blueprint.attacksUntillRunningAway = someData.myData.at("attacksUntillRunningAway").GetUchar();
	}
	else
	{
		blueprint.attacksUntillRunningAway = 3;
	}
	if (someData.myData.HasKey("chargeAttackDuration") == true)
	{
		// REMOVE THIS LATER, temp just incase LDs stuff were to get corrupted with the name change.
		blueprint.chargeAttackDuration = someData.myData.at("chargeAttackDuration").GetFloat();
		blueprint.chargeAttackDuration = 0.1f;
	}
	else
	{
		blueprint.chargeAttackDuration = 0.1f;
	}
	if (someData.myData.HasKey("chargeMeleeAttackDuration") == true)
	{
		// REMOVE THIS LATER, temp just incase LDs stuff were to get corrupted with the name change.
		blueprint.chargeMeleeAttackDuration = someData.myData.at("chargeMeleeAttackDuration").GetFloat();
	}
	else
	{
		blueprint.chargeMeleeAttackDuration = 0.5f;
	}

	if (someData.myData.HasKey("ShootingRange") == true)
	{
		blueprint.shootingRange = someData.myData.at("ShootingRange").GetFloat();
	}
	else
	{
		blueprint.shootingRange = blueprint.detectionRange-1;
	}

	blueprint.jumpHeight = someData.myData.at("JumpHeight").GetFloat();

	CComponent* component = enemyComponentManager->CreateComponentAbstract(&blueprint, eEnemyTypes::eImp);

	return component->GetId();
}