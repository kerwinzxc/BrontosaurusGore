#include "stdafx.h"
#include "NetworkMessage_PlayerPositionMessage.h"


CNetworkMessage_PlayerPositionMessage::CNetworkMessage_PlayerPositionMessage()
{
}


CNetworkMessage_PlayerPositionMessage::~CNetworkMessage_PlayerPositionMessage()
{
}

ePackageType CNetworkMessage_PlayerPositionMessage::GetPackageType() const
{
	return ePackageType::ePlayerPosition;
}


const CU::Matrix44f& CNetworkMessage_PlayerPositionMessage::GetTransformation()
{
	return myTransform;
}

const unsigned CNetworkMessage_PlayerPositionMessage::GetID()
{
	return myID;
}

void CNetworkMessage_PlayerPositionMessage::SetTransformation(const CU::Matrix44f& aTransform)
{
	myTransform = aTransform;
}

void CNetworkMessage_PlayerPositionMessage::SetPosition(const CU::Vector3f & aPosition)
{
	//myPosition = aPosition;
}

void CNetworkMessage_PlayerPositionMessage::SetID(const unsigned aClientID)
{
	myID = aClientID;
}

void CNetworkMessage_PlayerPositionMessage::DoSerialize(StreamType & aStream)
{
	serialize(myTransform, aStream);
	serialize(myID, aStream);
}

void CNetworkMessage_PlayerPositionMessage::DoDeserialize(StreamType & aStream)
{
	if (aStream.size() > 0)
	{
		myTransform = deserialize<CU::Matrix44f>(aStream);
		myID = deserialize<unsigned>(aStream);
	}
}
