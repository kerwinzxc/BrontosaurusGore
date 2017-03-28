#include "stdafx.h"
#include "Spawner.h"
#include "EnemyFactory.h"

CSpawnerComponent::CSpawnerComponent(const unsigned char aId, const CU::TimeUnit aSpawnInterval)
{
	mySpawnerId = aId;
	mySpawInterval = aSpawnInterval;
}


CSpawnerComponent::~CSpawnerComponent()
{
}

void CSpawnerComponent::Update(const float aDeltaTime)
{
	static CU::TimeUnit elapsedTime = 0.0f;

	elapsedTime += aDeltaTime;
	while (elapsedTime >= mySpawInterval)
	{
		elapsedTime -= mySpawInterval;
		SpawnEnemy();
	}
}

void CSpawnerComponent::SpawnEnemy()
{
	SImpBlueprint aData;
	aData.speed = 1.0f;
	aData.jumpHeight = 20.0f;
	aData.detectionRange = 10.0f;
	//aData.shouldGoMeleeRadius = 10.f;
	aData.startAttackRange = 5.0f;
	aData.stopAttackRange = 15.0f;
	CEnemyFactory::GetInstance()->CreateImp(&aData,GetParent()->GetWorldPosition());
}
