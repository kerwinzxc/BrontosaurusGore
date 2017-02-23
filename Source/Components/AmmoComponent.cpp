#include "stdafx.h"
#include "AmmoComponent.h"
#include "AmmoData.h"

AmmoComponent::AmmoComponent()
{
	myCurrentAmmo = 0;
}


AmmoComponent::~AmmoComponent()
{
}

void AmmoComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eCheckIfHaveAmmoForShooting:
		if (myCurrentAmmo > 0)
		{
			myCurrentAmmo--;
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
	aDeltaTime;
	//this function doesn't do anthing anymore :/;
}