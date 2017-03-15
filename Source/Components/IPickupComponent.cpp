#include "stdafx.h"
#include "IPickupComponent.h"


IPickupComponent::IPickupComponent()
{
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
