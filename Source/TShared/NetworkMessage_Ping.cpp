#include "stdafx.h"
#include "NetworkMessage_Ping.h"
#include "PackageType.h"


CNetworkMessage_Ping::CNetworkMessage_Ping()
{
	myHeader.myPackageType = static_cast<char>(ePackageType::ePing);
}


CNetworkMessage_Ping::~CNetworkMessage_Ping()
{
}

ePackageType CNetworkMessage_Ping::GetPackageType() const
{
	return ePackageType::ePing;
}
