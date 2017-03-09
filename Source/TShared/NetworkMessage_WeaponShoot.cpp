#include "stdafx.h"
#include "NetworkMessage_WeaponShoot.h"


CNetworkMessage_WeaponShoot::CNetworkMessage_WeaponShoot()
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
}

void CNetworkMessage_WeaponShoot::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	DESERIALIZE(myDirection, aStream);
}
