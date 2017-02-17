#include "stdafx.h"
#include "NetworkMessage_Ping.h"
#include "PackageType.h"


CNetworkMessage_Ping::CNetworkMessage_Ping()
{
	myHeader.myPackageType = static_cast<char>(ePackageType::PING);
}


CNetworkMessage_Ping::~CNetworkMessage_Ping()
{
}
