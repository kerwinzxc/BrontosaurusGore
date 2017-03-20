#include "stdafx.h"
#include "ArmorPickupComponent.h"


CArmorPickupComponent::CArmorPickupComponent()
{
	SetActive(true);
	myReplenishAmount = 0;
	myType = eComponentType::eArmorPickup;
}


CArmorPickupComponent::~CArmorPickupComponent()
{
}

void CArmorPickupComponent::SetReplenishAmount(const armorPoint aAmount)
{
	myReplenishAmount = aAmount;
}

void CArmorPickupComponent::DoMyEffect()
{
	SetActive(false);
}
