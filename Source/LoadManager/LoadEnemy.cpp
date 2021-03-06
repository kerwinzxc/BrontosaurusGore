#include "stdafx.h"
#include "LoadEnemy.h"
#include "EnemyComponentManager.h"
#include "Component.h"
#include "EnemyClientRepresentation.h"
#include "EnemyClientRepresentationManager.h"

int LoadEnemy(KLoader::SLoadedComponentData someData)
{

	
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

	/*blueprint.startAttackRange = 3.0f;
	blueprint.flightHeight = 6.0f;
	blueprint.hoverTime = 3.0f;
	blueprint.walkToMeleeRange = 6.0f;
	blueprint.detectionRange = 9.0f;*/

	blueprint.startAttackRange = 3.0f;
	blueprint.chargeCooldown = 1.0f;
	blueprint.chargeSpeed = 4.0f;
	blueprint.chargeDamage = 400.0f;
	blueprint.windupChargeTime = 1.0f;
	blueprint.walkToMeleeRange = 6.0f;
	blueprint.detectionRange = 9.0f;

	CComponent* component = enemyComponentManager->CreateComponentAbstract(&blueprint, eEnemyTypes::ePinky);
	
	return component->GetId();
}

int ClientLoadEnemy(KLoader::SLoadedComponentData someData)
{
	//client sidan

	CEnemyClientRepresentation& rep = CEnemyClientRepresentationManager::GetInstance().CreateAndRegister();

	return rep.GetId();
}

int ClientLoadImp(KLoader::SLoadedComponentData someData)
{
	//client sidan

	CEnemyClientRepresentation& rep = CEnemyClientRepresentationManager::GetInstance().CreateAndRegister();
	rep.SetEnemyType(eEnemyTypes::eImp);
	return rep.GetId();
}

int ClientLoadRevenant(KLoader::SLoadedComponentData someData)
{
	//client sidan

	CEnemyClientRepresentation& rep = CEnemyClientRepresentationManager::GetInstance().CreateAndRegister();
	rep.SetEnemyType(eEnemyTypes::eRevenant);
	return rep.GetId();
}

int ClientLoadPinky(KLoader::SLoadedComponentData someData)
{
	//client sidan

	CEnemyClientRepresentation& rep = CEnemyClientRepresentationManager::GetInstance().CreateAndRegister();
	rep.SetEnemyType(eEnemyTypes::ePinky);
	return rep.GetId();
}
