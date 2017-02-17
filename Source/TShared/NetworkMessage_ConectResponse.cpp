#include "stdafx.h"
#include "NetworkMessage_ConectResponse.h"


CNetworkMessage_ConectResponse::CNetworkMessage_ConectResponse()
{
}


CNetworkMessage_ConectResponse::~CNetworkMessage_ConectResponse()
{
}

void CNetworkMessage_ConectResponse::DoSerialize(StreamType& aStream)
{
	serialize(myClientId, aStream);
}

void CNetworkMessage_ConectResponse::DoDeserialize(StreamType& aStream)
{
	myClientId = deserialize<short>(aStream);
}
