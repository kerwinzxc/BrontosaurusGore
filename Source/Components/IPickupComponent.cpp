#include "stdafx.h"
#include "IPickupComponent.h"
#include "PollingStation.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"

IPickupComponent::IPickupComponent()
{
	myHasBeenPickedUp = false;
}


IPickupComponent::~IPickupComponent()
{
}

void IPickupComponent::ReInit()
{
	SetActive(true);
}

void IPickupComponent::SetActive(const bool aFlag)
{
	myHasBeenPickedUp = !aFlag;
	if (GetParent() != nullptr)
	{
		SComponentMessageData data;
		data.myBool = aFlag;
		GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, data);
	}
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
				if (GetShouldReset() == true)
				{
					Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CAddToCheckPointResetList(GetParent()));
				}
			}
		}
		break;

	case eComponentMessageType::eCheckPointReset:
		ReInit();
		break;
	default:
		break;
	}
}

const bool IPickupComponent::GetShouldReset() const
{
	return true;
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
