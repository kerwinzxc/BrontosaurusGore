#include "stdafx.h"
#include "PollingStation.h"
#include "AmmoPickupComponent.h"

CAmmoPickupComponent::CAmmoPickupComponent()
{
	myType = eComponentType::eAmmoPickup;
	myPickupData.ammoType = "";
	myPickupData.replenishAmount = 0;
}


CAmmoPickupComponent::~CAmmoPickupComponent()
{
}

void CAmmoPickupComponent::SetType(const std::string & aType)
{
	myPickupData.ammoType = aType.c_str();
}

void CAmmoPickupComponent::SetPickupAmount(const unsigned short aAmount)
{
	myPickupData.replenishAmount = aAmount;
}

void CAmmoPickupComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnTriggerEnter:
		if (aMessageData.myComponent->GetParent() == CPollingStation::GetInstance()->GetPlayerObject())
		{
			DoMyEffect();
		}
	default:
		break;
	}
}

bool CAmmoPickupComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData & aQuestionData)
{
	return false;
}

void CAmmoPickupComponent::GiveAmmoType()
{
	SComponentMessageData data;
	data.myAmmoReplenishData->ammoType = myPickupData.ammoType;
	data.myAmmoReplenishData->replenishAmount = myPickupData.replenishAmount;
	GetParent()->NotifyComponents(eComponentMessageType::eGiveAmmo, data);
}

void CAmmoPickupComponent::DoMyEffect()
{
	IPickupComponent::DoMyEffect();
	GiveAmmoType();
}
