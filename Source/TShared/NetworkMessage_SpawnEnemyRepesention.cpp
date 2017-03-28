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

const short CNetworkMessage_SpawnEnemyRepesention::GetHealth() const
{
	return myEnemyLife;
}

const eEnemyTypes CNetworkMessage_SpawnEnemyRepesention::GetEnemyType() const
{
	return myEnemyType;
}

void CNetworkMessage_SpawnEnemyRepesention::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myEnemyLife, aStream);
	serialize(myEnemyType, aStream);
}

void CNetworkMessage_SpawnEnemyRepesention::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	DESERIALIZE(myEnemyLife, aStream);
	DESERIALIZE(myEnemyType, aStream);
}
