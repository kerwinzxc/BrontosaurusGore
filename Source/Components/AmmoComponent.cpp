#include "stdafx.h"
#include "AmmoComponent.h"
#include "AmmoData.h"

AmmoComponent::AmmoComponent()
{
	myCurrentAmmoInClip = 0;
	myCurrentAmmo = 0;
	myElapsedRealodingTime = 0.0f;
}


AmmoComponent::~AmmoComponent()
{
}

void AmmoComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eCheckIfHaveAmmoForShooting:
		if (myCurrentAmmoInClip > 0)
		{
			myCurrentAmmoInClip--;
			GetParent()->NotifyComponents(eComponentMessageType::eShoot, aMessageData);
		}
		break;
	default:
		break;
	}
}

void AmmoComponent::Destroy()
{
}

void AmmoComponent::Update(float aDeltaTime)
{
	if(myElapsedRealodingTime < myAmmoData->realoadDuration)
	{
		myElapsedRealodingTime += aDeltaTime;
	}
}