#include "stdafx.h"
#include "ArmorPickupComponent.h"


CArmorPickupComponent::CArmorPickupComponent()
{
	myHasBeenPickedUp = false;
	myReplenishAmount = 0;
	myType = eComponentType::eArmorPickup;
}


CArmorPickupComponent::~CArmorPickupComponent()
{
}

void CArmorPickupComponent::DoMyEffect()
{
	
}
