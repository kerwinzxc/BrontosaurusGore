#include "stdafx.h"
#include "NetworkMessage_WeaponChange.h"


CNetworkMessage_WeaponChange::CNetworkMessage_WeaponChange(): myWeaponIndex(-1)
{
	myShooter = Shooter::Player;
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

CNetworkMessage_WeaponChange::Shooter CNetworkMessage_WeaponChange::GetShooter()
{
	return myShooter;
}

void CNetworkMessage_WeaponChange::SetShooterId(unsigned int anId)
{
	myId = anId;
}

unsigned CNetworkMessage_WeaponChange::GetId()
{
	return myId;
}

void CNetworkMessage_WeaponChange::SetShooter(Shooter aShooter)
{
	myShooter = aShooter;
}

void CNetworkMessage_WeaponChange::DoSerialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	SERIALIZE(myWeaponIndex, aStream);
	SERIALIZE(myShooter, aStream);
	SERIALIZE(myId, aStream);
}

void CNetworkMessage_WeaponChange::DoDeserialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	DESERIALIZE(myWeaponIndex, aStream);
	DESERIALIZE(myShooter, aStream);
	DESERIALIZE(myId, aStream);
}
