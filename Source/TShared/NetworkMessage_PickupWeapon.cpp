#include "stdafx.h"
#include "NetworkMessage_PickupWeapon.h"
#include "../Components/GameObject.h"
#include "../Components/ComponentAnswer.h"

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
	serialize(myWeaponID, aStream);
	serialize(myNetworkID, aStream);
}
void CNetworkMessage_PickupWeapon::DoDeserialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myWeaponID = deserialize<unsigned short>(aStream);
	myNetworkID = deserialize<int>(aStream);
}