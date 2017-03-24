#include "stdafx.h"
#include "NetworkMessage_TryToShoot.h"


CNetworkMessage_TryToShoot::CNetworkMessage_TryToShoot()
{
}


CNetworkMessage_TryToShoot::~CNetworkMessage_TryToShoot()
{
}

ePackageType CNetworkMessage_TryToShoot::GetPackageType() const
{
	return ePackageType::eTryToShoot;
}

void CNetworkMessage_TryToShoot::SetDirection(const CU::Vector3f & aDirection)
{
	myDirection = aDirection;
}

CU::Vector3f CNetworkMessage_TryToShoot::GetDirection()
{
	return myDirection;
}

void CNetworkMessage_TryToShoot::SetWeaponIndex(const unsigned char aWeaponIndex)
{
	myWeaponIndex = aWeaponIndex;
}

int CNetworkMessage_TryToShoot::GetWeaponIndex()
{
	return myWeaponIndex;
}

void CNetworkMessage_TryToShoot::SetWeaponName(const std::string aWeaponName)
{
	myWeaponName = aWeaponName;
}

const std::string& CNetworkMessage_TryToShoot::GetWeaponName()
{
	return myWeaponName;
}

void CNetworkMessage_TryToShoot::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myDirection, aStream);
	serialize(myWeaponIndex, aStream);
	serialize(myWeaponName, aStream);
}

void CNetworkMessage_TryToShoot::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myDirection = deserialize<CU::Vector3f>(aStream);
	myWeaponIndex = deserialize<unsigned char>(aStream);
	myWeaponName = deserialize<std::string>(aStream);
}
