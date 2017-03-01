#include "stdafx.h"
#include "NetworkMessage_ClientReady.h"


CNetworkMessage_ClientReady::CNetworkMessage_ClientReady()
{
}


CNetworkMessage_ClientReady::~CNetworkMessage_ClientReady()
{
}

ePackageType CNetworkMessage_ClientReady::GetPackageType() const
{
	return ePackageType::eClientReady;
}
