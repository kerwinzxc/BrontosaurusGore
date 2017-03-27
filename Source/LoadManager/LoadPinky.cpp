#include "stdafx.h"
#include "LoadPinky.h"
#include "EnemyComponentManager.h"

int LoadPinky(KLoader::SLoadedComponentData someData)
{
	static unsigned int ID = 0;


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
	blueprint.shouldGoMeleeRadius = someData.myData.at("GoingMeleeRange").GetFloat();
	blueprint.chargeCooldown = someData.myData.at("GoingMeleeRange").GetFloat();
	blueprint.chargeDamage = static_cast<healthPoint>(someData.myData.at("ChargeDamage").GetFloat());
	blueprint.chargeSpeed = someData.myData.at("ChargeSpeed").GetFloat();
	blueprint.windupChargeTime = someData.myData.at("WindUpChargeDuration").GetFloat();

	CComponent* component = enemyComponentManager->CreateComponentAbstract(&blueprint, ID++, eEnemyTypes::ePinky);

	return component->GetId();
}