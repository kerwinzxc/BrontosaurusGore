#include "stdafx.h"
#include "PollingStation.h"
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
	case eComponentMessageType::eOnTriggerEnter:
		if (aMessageData.myComponent->GetParent() == CPollingStation::GetInstance()->GetPlayerObject())
		{
			DoMyEffect();
		}
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

void CPickupAmmoComponent::DoMyEffect()
{
	CPickupAmmoComponent::DoMyEffect();
	GiveAmmoType();
}
