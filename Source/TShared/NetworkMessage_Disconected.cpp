#include "stdafx.h"
#include "NetworkMessage_Disconected.h"


CNetworkMessage_Disconected::CNetworkMessage_Disconected()
{
}


CNetworkMessage_Disconected::~CNetworkMessage_Disconected()
{
}

ePackageType CNetworkMessage_Disconected::GetPackageType() const
{
	return ePackageType::eDisconected;
}

void CNetworkMessage_Disconected::DoSerialize(StreamType& aStream)
{
	SERIALIZE(myClientName, aStream);
}

void CNetworkMessage_Disconected::DoDeserialize(StreamType& aStream)
{
	DESERIALIZE(myClientName, aStream);
}
