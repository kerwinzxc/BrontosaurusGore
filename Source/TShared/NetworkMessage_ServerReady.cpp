#include "stdafx.h"
#include "NetworkMessage_ServerReady.h"


CNetworkMessage_ServerReady::CNetworkMessage_ServerReady()
{
}


CNetworkMessage_ServerReady::~CNetworkMessage_ServerReady()
{
}

ePackageType CNetworkMessage_ServerReady::GetPackageType() const
{
	return ePackageType::eServerReady;
}
