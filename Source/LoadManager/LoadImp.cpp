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
		blueprint.walkToMeleeRange = someData.myData.at("wanderDistance").GetFloat();
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
		blueprint.wanderDuration = someData.myData.at("chargeAttackDuration").GetFloat();
	}
	else
	{
		blueprint.chargeAttackDuration = 2.0f;
	}

	blueprint.jumpHeight = someData.myData.at("JumpHeight").GetFloat();

	CComponent* component = enemyComponentManager->CreateComponentAbstract(&blueprint, eEnemyTypes::eImp);

	return component->GetId();
}