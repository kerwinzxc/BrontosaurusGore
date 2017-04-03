#include "stdafx.h"
#include "NetworkMessage_PickupWeapon.h"


CNetworkMessage_PickupWeapon::CNetworkMessage_PickupWeapon()
{
}


CNetworkMessage_PickupWeapon::~CNetworkMessage_PickupWeapon()
{
}

void CNetworkMessage_PickupWeapon::SetWeapon(const std::string & aWeaponName)
{
	if (aWeaponName == "Shotgun")
	{
		myWeaponPickup = eWeaponPickupType::eShotgun;
		return;
	}
	if (aWeaponName == "PlasmaRifle")
	{
		eWeaponPickupType::ePlasmaRifle;
		return;
	}
	eWeaponPickupType::eBFG;
}

const CNetworkMessage_PickupWeapon::eWeaponPickupType CNetworkMessage_PickupWeapon::GetWeaponPickup() const
{
	return myWeaponPickup;
}

void CNetworkMessage_PickupWeapon::SetID(const int aId)
{
	myNetworkID = aId;
}

const int CNetworkMessage_PickupWeapon::GetID()
{
	return myNetworkID;
}

ePackageType CNetworkMessage_PickupWeapon::GetPackageType() const
{
	return ePackageType::ePickupWeapon;
}
