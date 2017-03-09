#include "stdafx.h"
#include "PickupAmmoComponent.h"


CPickupAmmoComponent::CPickupAmmoComponent()
{
	myType = eComponentType::eAmmoPickup;
	myPickupData.ammoType = "";
	myPickupData.replenishAmount = 0;
}


CPickupAmmoComponent::~CPickupAmmoComponent()
{
}

void CPickupAmmoComponent::SetType(const std::string & aType)
{
	myPickupData.ammoType = aType.c_str();
}

void CPickupAmmoComponent::SetPickupAmount(const unsigned short aAmount)
{
	myPickupData.replenishAmount = aAmount;
}

void CPickupAmmoComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
		//Needs collision to ammo pickup, will be implemented later
	default:
		break;
	}
}

bool CPickupAmmoComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData & aQuestionData)
{
	return false;
}

void CPickupAmmoComponent::GiveAmmoType()
{
	SComponentMessageData data;
	data.myAmmoReplenishData->ammoType = myPickupData.ammoType;
	data.myAmmoReplenishData->replenishAmount = myPickupData.replenishAmount;
	GetParent()->NotifyComponents(eComponentMessageType::eGiveAmmo, data);
}
