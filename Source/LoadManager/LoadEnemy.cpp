#include "stdafx.h"
#include "LoadEnemy.h"
#include "EnemyComponentManager.h"
#include "Component.h"
#include "EnemyClientRepresentation.h"
#include "EnemyClientRepresentationManager.h"

int LoadEnemy(KLoader::SLoadedComponentData someData)
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

	/*blueprint.startAttackRange = 3.0f;
	blueprint.flightHeight = 6.0f;
	blueprint.hoverTime = 3.0f;
	blueprint.shouldGoMeleeRadius = 6.0f;
	blueprint.detectionRange = 9.0f;*/

	blueprint.startAttackRange = 3.0f;
	blueprint.chargeCooldown = 1.0f;
	blueprint.chargeSpeed = 4.0f;
	blueprint.chargeDamage = 400.0f;
	blueprint.windupChargeTime = 1.0f;
	blueprint.shouldGoMeleeRadius = 6.0f;
	blueprint.detectionRange = 9.0f;

	CComponent* component = enemyComponentManager->CreateComponentAbstract(&blueprint, ID++, eEnemyTypes::ePinky);
	
	return component->GetId();
}

int ClientLoadEnemy(KLoader::SLoadedComponentData someData)
{
	//client sidan

	static unsigned int ID = 0;

	CEnemyClientRepresentation& rep = CEnemyClientRepresentationManager::GetInstance().CreateAndRegister(ID++);

	return rep.GetId();
}
