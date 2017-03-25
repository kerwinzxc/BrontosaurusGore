#include "stdafx.h"
#include "LoadEnemy.h"
#include "EnemyComponentManager.h"
#include "Component.h"
#include "EnemyClientRepresentation.h"
#include "EnemyClientRepresentationManager.h"
#include "ImpController.h"

int LoadEnemy(KLoader::SLoadedComponentData someData)
{
	static unsigned int ID = 0;

	
	GET_SERVERLOADMANAGER(loadManager);
	CEnemyComponentManager* enemyComponentManager = loadManager.GetCurrentGameServer().GetEnemyComponentManager();
	
	if (!enemyComponentManager)
	{

		return NULL_COMPONENT;
	}
	SImpBlueprint blueprint;
	blueprint.speed = someData.myData.at("speed").GetFloat();
	blueprint.detectionRange = someData.myData.at("detactionRange").GetFloat();
	blueprint.startAttackRange = someData.myData.at("startAttackRange").GetFloat();
	blueprint.stopAttackRange = someData.myData.at("stopAttackRange").GetFloat();

	blueprint.startAttackRange = 0.0f;
	blueprint.jumpHeight = 3.0f;
	blueprint.goMeleeRadius = 100.0f;

	CComponent* component = enemyComponentManager->CreateComponentAbstract(&blueprint, ID++, eEnemyTypes::eImp);
	
	return component->GetId();
}

int ClientLoadEnemy(KLoader::SLoadedComponentData someData)
{
	//client sidan

	static unsigned int ID = 0;

	CEnemyClientRepresentation& rep = CEnemyClientRepresentationManager::GetInstance().CreateAndRegister(ID++);

	return rep.GetId();
}
