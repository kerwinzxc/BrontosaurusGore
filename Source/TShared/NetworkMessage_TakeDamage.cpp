#include "stdafx.h"
#include "NetworkMessage_TakeDamage.h"


CNetworkMessage_TakeDamage::CNetworkMessage_TakeDamage()
{
	myNetworkID = -1;
	myDamageTaken = 0;
}


CNetworkMessage_TakeDamage::~CNetworkMessage_TakeDamage()
{
}

ePackageType CNetworkMessage_TakeDamage::GetPackageType() const
{
	return ePackageType::eTakeDamage;
}

const unsigned int CNetworkMessage_TakeDamage::GetID()
{
	return myNetworkID;
}

void CNetworkMessage_TakeDamage::SetID(const int aId)
{
	myNetworkID = aId;
}

const unsigned int CNetworkMessage_TakeDamage::GetDamageTaken()
{
	return myDamageTaken;
}

void CNetworkMessage_TakeDamage::SetDamageTaken(const unsigned int aDamageTaken)
{
	myDamageTaken = aDamageTaken;
}

void CNetworkMessage_TakeDamage::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myNetworkID, aStream);
	serialize(myDamageTaken, aStream);
}

void CNetworkMessage_TakeDamage::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myNetworkID = deserialize<unsigned int>(aStream);
	myDamageTaken = deserialize<unsigned int>(aStream);
}
