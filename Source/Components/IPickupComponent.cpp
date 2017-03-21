#include "stdafx.h"
#include "IPickupComponent.h"
#include "PollingStation.h"

IPickupComponent::IPickupComponent()
{
	myHasBeenPickedUp = false;
}


IPickupComponent::~IPickupComponent()
{
}

void IPickupComponent::SetActive(const bool aFlag)
{
	myHasBeenPickedUp = aFlag;

	SComponentMessageData data;
	data.myBool = aFlag;
	GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, data);
}

void IPickupComponent::SetNetworkId(const int aID)
{
	myNetworkId = aID;
}

const int IPickupComponent::GetNetworkId() const
{
	return myNetworkId;
}

void IPickupComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnTriggerEnter:
		if (aMessageData.myComponent->GetParent() == CPollingStation::GetInstance()->GetPlayerObject())
		{
			if (myHasBeenPickedUp == false)
			{
				IPickupComponent::DoMyEffect();
				DoMyEffect();
			}
		}
		break;
	default:
		break;
	}
}

const bool IPickupComponent::GetIsActive() const
{
	return myHasBeenPickedUp;
}

void IPickupComponent::DoMyEffect()
{
	SetActive(false);
	SComponentMessageData data;
	data.myBool = false;
	GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, data);
}
