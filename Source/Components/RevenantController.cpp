#include "stdafx.h"
#include "RevenantController.h"


CRevenantController::CRevenantController(unsigned int aId, eEnemyTypes aType)
	:CEnemy(aId, aType)
{
}


CRevenantController::~CRevenantController()
{
}

void CRevenantController::SetEnemyData(const SEnemyBlueprint* aData)
{
	const SRevenantBlueprint* revenantData = static_cast<const SRevenantBlueprint*>(aData);
	myFlyingTime = revenantData->flyingTime;
	myFlightHeight = revenantData->flightHeight;
	CEnemy::SetEnemyData(aData);
}