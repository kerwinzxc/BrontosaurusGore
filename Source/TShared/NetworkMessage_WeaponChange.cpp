#include "stdafx.h"
#include "NetworkMessage_WeaponChange.h"


CNetworkMessage_WeaponChange::CNetworkMessage_WeaponChange(): myWeaponIndex(-1)
{
}


CNetworkMessage_WeaponChange::~CNetworkMessage_WeaponChange()
{
}

void CNetworkMessage_WeaponChange::SetWeapon(unsigned short aActiveWeaponIndex)
{
	myWeaponIndex = aActiveWeaponIndex;
}

ePackageType CNetworkMessage_WeaponChange::GetPackageType() const
{
	return ePackageType::eWeaponChange;
}

int CNetworkMessage_WeaponChange::GetWeaponIndex() const
{
	return myWeaponIndex;
}

void CNetworkMessage_WeaponChange::DoSerialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	SERIALIZE(myWeaponIndex, aStream);
}

void CNetworkMessage_WeaponChange::DoDeserialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	DESERIALIZE(myWeaponIndex, aStream);
}
