#include "stdafx.h"
#include "NetworkMessage_WeaponShoot.h"


CNetworkMessage_WeaponShoot::CNetworkMessage_WeaponShoot() : myShooter(Shooter::Player)
{

}


CNetworkMessage_WeaponShoot::~CNetworkMessage_WeaponShoot()
{
}

ePackageType CNetworkMessage_WeaponShoot::GetPackageType() const
{
	return ePackageType::eWeaponShoot;
}

void CNetworkMessage_WeaponShoot::SetDirection(const CU::Vector3f & aDirection)
{
	myDirection = aDirection;
}

CU::Vector3f CNetworkMessage_WeaponShoot::GetDirection()
{
	return myDirection;
}

void CNetworkMessage_WeaponShoot::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	SERIALIZE(myDirection, aStream);
	SERIALIZE(myWeaponIndex, aStream);
	SERIALIZE(myShooter, aStream);
	SERIALIZE(myId, aStream);
}

void CNetworkMessage_WeaponShoot::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	DESERIALIZE(myDirection, aStream);
	DESERIALIZE(myWeaponIndex, aStream);
	DESERIALIZE(myShooter, aStream);
	DESERIALIZE(myId, aStream);
}

void CNetworkMessage_WeaponShoot::SetWeaponIndex(const unsigned char aWeaponIndex)
{
	myWeaponIndex = aWeaponIndex;
}
int CNetworkMessage_WeaponShoot::GetWeaponIndex()
{
	return myWeaponIndex;
}

CNetworkMessage_WeaponShoot::Shooter CNetworkMessage_WeaponShoot::GetShooter()
{
	return myShooter;
}

unsigned CNetworkMessage_WeaponShoot::GetId()
{
	return myId;
}

void CNetworkMessage_WeaponShoot::SetShooter(Shooter aShooter)
{
	myShooter = aShooter;
}

void CNetworkMessage_WeaponShoot::SetShooterId(unsigned anId)
{
	myId = anId;
}
