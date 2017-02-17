#include "stdafx.h"
#include "AmmoComponent.h"
#include "AmmoData.h"

AmmoComponent::AmmoComponent()
{
	myCurrentAmmoInClip = 0;
	myCurrentAmmo = 0;
	myElapsedRealodingTime = 0.0f;
	myIsReloding = false;
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
	if(myIsReloding == true)
	{
		myElapsedRealodingTime += aDeltaTime;
		if(myElapsedRealodingTime >= myAmmoData->realoadDuration)
		{
			myIsReloding = false;
			if(myCurrentAmmo >= myAmmoData->ammoClip)
			{
				myCurrentAmmoInClip += myAmmoData->ammoClip;
				if(myCurrentAmmoInClip > myAmmoData->ammoClip)
				{
					myCurrentAmmoInClip = myAmmoData->ammoClip;
				}
			}
			else
			{
				myCurrentAmmoInClip += myCurrentAmmo;
				if (myCurrentAmmoInClip > myAmmoData->ammoClip)
				{
					myCurrentAmmoInClip = myAmmoData->ammoClip;
				}
			}
		}
	}
}