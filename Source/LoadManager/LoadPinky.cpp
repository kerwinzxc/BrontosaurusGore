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
	blueprint.speed = someData.myData.at("speed").GetFloat();
	blueprint.detectionRange = someData.myData.at("detactionRange").GetFloat();
	blueprint.startAttackRange = someData.myData.at("startAttackRange").GetFloat();
	blueprint.stopAttackRange = someData.myData.at("stopAttackRange").GetFloat();
	blueprint.shouldGoMeleeRadius = someData.myData.at("goingMeleeRange").GetFloat();
	blueprint.chargeCooldown = someData.myData.at("chargeCooldown").GetFloat();
	blueprint.chargeDamage = static_cast<healthPoint>(someData.myData.at("chargeCooldown").GetFloat());
	blueprint.chargeSpeed = someData.myData.at("chargeSpeed").GetFloat();
	blueprint.windupChargeTime = someData.myData.at("windupChargeDuration").GetFloat();

	CComponent* component = enemyComponentManager->CreateComponentAbstract(&blueprint, ID++, eEnemyTypes::ePinky);

	return component->GetId();
}