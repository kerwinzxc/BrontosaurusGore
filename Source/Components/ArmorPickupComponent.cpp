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
	SComponentMessageData data;
	data.myInt = myReplenishAmount;
	//armor systems needs to be added, maybe healthcomponent can hold how much armor the player has
	GetParent()->NotifyComponents(eComponentMessageType::eAddArmor, data);
}
