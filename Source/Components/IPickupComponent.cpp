#include "stdafx.h"
#include "IPickupComponent.h"
#include "PollingStation.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"
#include "../BrontosaurusEngine/Engine.h"

#define VERTICAL_OFFSET 0.25f
#define VERTICAL_RATE 1.0f


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
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CAddToCheckPointResetList(GetParent()));
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
	case eComponentMessageType::eAddComponent:
		if(aMessageData.myComponentTypeAdded == eComponentType::eModel)
		{
			SComponentMessageData message;
			message.myComponentTypeAdded = myType;
			GetParent()->NotifyComponents(eComponentMessageType::eAddComponent, message);
		}
		if (aMessageData.myComponent == this)
		{
			GetParent()->GetLocalTransform().myPosition.y += VERTICAL_OFFSET;
			GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		}

		break;
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

void IPickupComponent::Update(const float aDeltaTime)
{

	GetParent()->GetLocalTransform().RotateAroundAxis(aDeltaTime * PI * 0.5f, CU::Axees::Y);
	float offset = sin(ENGINE->GetTime().GetSeconds() * PI * VERTICAL_RATE);

	offset *= VERTICAL_OFFSET;
	offset *= aDeltaTime;
	
	GetParent()->GetLocalTransform().myPosition.y += offset;
	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}
