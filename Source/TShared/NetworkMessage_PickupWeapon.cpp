#include "stdafx.h"
#include "NetworkMessage_PickupWeapon.h"


CNetworkMessage_PickupWeapon::CNetworkMessage_PickupWeapon()
{
}


CNetworkMessage_PickupWeapon::~CNetworkMessage_PickupWeapon()
{
}

void CNetworkMessage_PickupWeapon::SetWeapon(const unsigned short aWeaponID)
{
	myWeaponID = aWeaponID;
}

const unsigned short CNetworkMessage_PickupWeapon::GetWeaponPickup() const
{
	return myWeaponID;
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

void CNetworkMessage_PickupWeapon::DoSerialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myNetworkID, aStream);
	serialize(myWeaponID, aStream);
}
void CNetworkMessage_PickupWeapon::DoDeserialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myNetworkID = deserialize<int>(aStream);
	myWeaponID = deserialize<unsigned short>(aStream);
}