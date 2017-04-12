#include "stdafx.h"
#include "NetWorkMessage_PickupAmmo.h"


CNetWorkMessage_PickupAmmo::CNetWorkMessage_PickupAmmo()
{
	myNetworkID = -1;
}


CNetWorkMessage_PickupAmmo::~CNetWorkMessage_PickupAmmo()
{
}

ePackageType CNetWorkMessage_PickupAmmo::GetPackageType() const
{
	return ePackageType::ePickupAmmo;
}

const int CNetWorkMessage_PickupAmmo::GetID()
{
	return myNetworkID;
}

void CNetWorkMessage_PickupAmmo::SetID(const int aId)
{
	myNetworkID = aId;
}

void CNetWorkMessage_PickupAmmo::SetWeaponID(const short aID)
{
	myWeaponID = aID;
}

const short CNetWorkMessage_PickupAmmo::GetWeaponID() const
{
	return myWeaponID;
}

const void CNetWorkMessage_PickupAmmo::SetReplenishAmount(const short aReplenishAmount)
{
	myReplenishAmount = aReplenishAmount;
}

const short CNetWorkMessage_PickupAmmo::GetReplenishAmount() const
{
	return myReplenishAmount;
}
void CNetWorkMessage_PickupAmmo::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myNetworkID, aStream);
	serialize(myReplenishAmount, aStream);
	serialize(myWeaponID, aStream);
}

void CNetWorkMessage_PickupAmmo::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myNetworkID = deserialize<int>(aStream);
	myReplenishAmount = deserialize<short>(aStream);
	myWeaponID = deserialize<short>(aStream);
}
