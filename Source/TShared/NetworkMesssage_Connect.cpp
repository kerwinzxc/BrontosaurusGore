#include "stdafx.h"
#include "NetworkMesssage_Connect.h"
#include "PackageType.h"


CNetworkMessage_Connect::CNetworkMessage_Connect()
{
	myHeader.myPackageType = static_cast<char>(ePackageType::CONNECT);
}


CNetworkMessage_Connect::~CNetworkMessage_Connect()
{
}

void CNetworkMessage_Connect::DoSerialize(StreamType& aStream)
{
	serialize(myClientName, aStream);
}

void CNetworkMessage_Connect::DoDeserialize(StreamType& aStream)
{
	if (aStream.size() > 0)
	{
		myClientName = deserialize<std::string>(aStream);
	}
}
