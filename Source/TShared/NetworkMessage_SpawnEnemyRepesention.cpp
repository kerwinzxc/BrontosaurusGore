#include "stdafx.h"
#include "NetworkMessage_SpawnEnemyRepesention.h"


CNetworkMessage_SpawnEnemyRepesention::CNetworkMessage_SpawnEnemyRepesention()
{
}


CNetworkMessage_SpawnEnemyRepesention::~CNetworkMessage_SpawnEnemyRepesention()
{
}

ePackageType CNetworkMessage_SpawnEnemyRepesention::GetPackageType() const
{
	return ePackageType::eSpawnEnemyRepresentation;
}

void CNetworkMessage_SpawnEnemyRepesention::SetHealth(const short aHealthValue)
{
	myEnemyLife = aHealthValue;
}

void CNetworkMessage_SpawnEnemyRepesention::SetEnemyType(const eEnemyTypes aType)
{
	myEnemyType = aType;
}

void CNetworkMessage_SpawnEnemyRepesention::SetPosition(const CU::Vector3f& aPosition)
{
	myPosition = aPosition;
}

const short CNetworkMessage_SpawnEnemyRepesention::GetHealth() const
{
	return myEnemyLife;
}

const eEnemyTypes CNetworkMessage_SpawnEnemyRepesention::GetEnemyType() const
{
	return myEnemyType;
}

const CU::Vector3f  CNetworkMessage_SpawnEnemyRepesention::GetPosition() const
{
	return myPosition;
}

void CNetworkMessage_SpawnEnemyRepesention::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myEnemyLife, aStream);
	serialize(myEnemyType, aStream);
	serialize(myPosition, aStream);
}

void CNetworkMessage_SpawnEnemyRepesention::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	DESERIALIZE(myEnemyLife, aStream);
	DESERIALIZE(myEnemyType, aStream);
	DESERIALIZE(myPosition, aStream);
}
