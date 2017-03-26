#include "stdafx.h"
#include "PinkyController.h"

CPinkyController::CPinkyController(unsigned int aId, eEnemyTypes aType)
	:CEnemy(aId, aType)
{
}


CPinkyController::~CPinkyController()
{
}

void CPinkyController::SetEnemyData(const SEnemyBlueprint* aData)
{
	const SPinkyBlueprint* pinkyData = static_cast<const SPinkyBlueprint*>(aData);
	myWindupChargeTime = pinkyData->windupChargeTime;
	myChargeDamage = pinkyData->chargeDamage;
	CEnemy::SetEnemyData(aData);
}