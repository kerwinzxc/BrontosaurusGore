#include "stdafx.h"
#include "NetworkMessage_Position.h"


CNetworkMessage_Position::CNetworkMessage_Position()
{
}


CNetworkMessage_Position::~CNetworkMessage_Position()
{
}

const CU::Vector3f& CNetworkMessage_Position::GetPosition()
{
	return myPosition;
}

const unsigned CNetworkMessage_Position::GetID()
{
	return myNetworkComponentID;
}

void CNetworkMessage_Position::SetPosition(const CU::Vector3f& aPosition)
{
	myPosition = aPosition;
}

ePackageType CNetworkMessage_Position::GetPackageType() const
{
	return ePackageType::ePosition;
}

void CNetworkMessage_Position::DoSerialize(StreamType& aStream)
{
	serialize(myPosition, aStream);
	serialize(myNetworkComponentID, aStream);
}

void CNetworkMessage_Position::DoDeserialize(StreamType& aStream)
{
	if (aStream.size() > 0)
	{
		myPosition = deserialize<CU::Vector3f>(aStream);
		myNetworkComponentID = deserialize<unsigned>(aStream);
	}
}

void CNetworkMessage_Position::SetID(const unsigned aNetworkComponentID)
{
	myNetworkComponentID = aNetworkComponentID;
}
