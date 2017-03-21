#include "stdafx.h"
#include "NetworkMessage_OKShoot.h"


CNetworkMessage_OKShoot::CNetworkMessage_OKShoot()
{
}


CNetworkMessage_OKShoot::~CNetworkMessage_OKShoot()
{
}

ePackageType CNetworkMessage_OKShoot::GetPackageType() const
{
	return ePackageType::eOKShoot;
}

void CNetworkMessage_OKShoot::SetDirection(const CU::Vector3f & aDirection)
{
	myDirection = aDirection;
}

CU::Vector3f CNetworkMessage_OKShoot::GetDirection()
{
	return myDirection;
}

void CNetworkMessage_OKShoot::SetWeaponIndex(const unsigned char aWeaponIndex)
{
	myWeaponIndex = aWeaponIndex;
}

int CNetworkMessage_OKShoot::GetWeaponIndex()
{
	return myWeaponIndex;
}

void CNetworkMessage_OKShoot::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myDirection, aStream);
	serialize(myWeaponIndex, aStream);
}

void CNetworkMessage_OKShoot::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myDirection = deserialize<CU::Vector3f>(aStream);
	myWeaponIndex = deserialize<unsigned char>(aStream);
}
