#include "stdafx.h"
#include "NetworkMessage_EnemyPosition.h"


CNetworkMessage_EnemyPosition::CNetworkMessage_EnemyPosition(): myEnemyId(0)
{
}


CNetworkMessage_EnemyPosition::~CNetworkMessage_EnemyPosition()
{
}

void CNetworkMessage_EnemyPosition::SetId(unsigned anId)
{
	myEnemyId = anId;
}

unsigned CNetworkMessage_EnemyPosition::GetId() const
{
	return myEnemyId;
}

void CNetworkMessage_EnemyPosition::SetPosition(const CU::Vector3f& anEnemyPosition)
{
	myEnemyPosition = anEnemyPosition;
}

const CU::Vector3f& CNetworkMessage_EnemyPosition::GetPosition()
{
	return myEnemyPosition;
}

ePackageType CNetworkMessage_EnemyPosition::GetPackageType() const
{
	return ePackageType::eEnemyPosition;
}

void CNetworkMessage_EnemyPosition::DoSerialize(StreamType& aStream)
{
	serialize(myEnemyId, aStream);
	serialize(myEnemyPosition, aStream);
}

void CNetworkMessage_EnemyPosition::DoDeserialize(StreamType& aStream)
{
	myEnemyId = deserialize<unsigned int>(aStream);
	myEnemyPosition = deserialize<CU::Vector3f>(aStream);
}
