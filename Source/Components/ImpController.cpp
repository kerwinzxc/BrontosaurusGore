#include "stdafx.h"
#include "ImpController.h"


CImpController::CImpController(unsigned int aId)
	: CEnemy(aId)
{
}


CImpController::~CImpController()
{
}

void CImpController::Update(const float aDeltaTime)
{
	DL_PRINT("Marvelous");
}