#pragma once
#include "NetworkMessage.h"

class CNetworkMessage_PingResponse: public CNetworkMessage
{
public:
	CNetworkMessage_PingResponse();
	~CNetworkMessage_PingResponse();
	ePackageType GetPackageType() const override;
};

inline ePackageType CNetworkMessage_PingResponse::GetPackageType() const
{
	return ePackageType::ePingResponse;
}
