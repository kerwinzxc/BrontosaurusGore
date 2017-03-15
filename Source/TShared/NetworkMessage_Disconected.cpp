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

std::string CNetworkMessage_Disconected::GetClientName() const
{
	return myClientName;
}

void CNetworkMessage_Disconected::SetClientName(const std::string& aClientsName)
{
	myClientName = aClientsName;
}

void CNetworkMessage_Disconected::DoSerialize(StreamType& aStream)
{
	SERIALIZE(myClientName, aStream);
}

void CNetworkMessage_Disconected::DoDeserialize(StreamType& aStream)
{
	DESERIALIZE(myClientName, aStream);
}
