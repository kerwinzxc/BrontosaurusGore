#include "stdafx.h"
#include "NetworkMessage_EnemyTransformation.h"


CNetworkMessage_EnemyTransformation::CNetworkMessage_EnemyTransformation()
{
}


CNetworkMessage_EnemyTransformation::~CNetworkMessage_EnemyTransformation()
{
}

void CNetworkMessage_EnemyTransformation::SetId(unsigned int aId)
{
	myEnemyId = aId;
}

unsigned int CNetworkMessage_EnemyTransformation::GetId() const
{
	return myEnemyId;
}

void CNetworkMessage_EnemyTransformation::SetTransformation(const CU::Matrix44f & anEnemyTransformaion)
{
	myEnemyTransformation = anEnemyTransformaion;
}

const CU::Matrix44f & CNetworkMessage_EnemyTransformation::GetTransformation()
{
	return myEnemyTransformation;
}

ePackageType CNetworkMessage_EnemyTransformation::GetPackageType() const
{
	return ePackageType::eEnemyTransformaion;
}

void CNetworkMessage_EnemyTransformation::DoSerialize(StreamType & aStream)
{
	serialize(myEnemyId, aStream);
	serialize(myEnemyTransformation, aStream);
}

void CNetworkMessage_EnemyTransformation::DoDeserialize(StreamType & aStream)
{
	myEnemyId = deserialize<unsigned int>(aStream);
	myEnemyTransformation = deserialize<CU::Matrix44f>(aStream);
}
