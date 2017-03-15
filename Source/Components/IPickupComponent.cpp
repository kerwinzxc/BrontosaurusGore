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
