#include "stdafx.h"
#include "WeaponPickupComponent.h"
#include "PollingStation.h"

CWeaponPickupComponent::CWeaponPickupComponent()
{
	myNetworkId = -1;
	myType = eComponentType::eWeaponPickupComponent;
	myWeaponPickup = "";
}


CWeaponPickupComponent::~CWeaponPickupComponent()
{
}

void CWeaponPickupComponent::SetWeaponPickup(const std::string& aWeaponName)
{
	myWeaponPickup = aWeaponName;
}

void CWeaponPickupComponent::DoMyEffect()
{
	SComponentMessageData data;
	data.myString = myWeaponPickup.c_str();
	CPollingStation::GetInstance()->GetPlayerObject()->NotifyComponents(eComponentMessageType::eAddWeapon,data);
}
