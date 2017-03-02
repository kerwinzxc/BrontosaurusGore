#pragma once
#include "NetworkMessage.h"

class CNetworkMessage_PingResponse: public CNetworkMessage
{
public:
	friend class CMessageStorage;
	
	CNetworkMessage_PingResponse();
	~CNetworkMessage_PingResponse();

	ePackageType GetPackageType() const override;

private:

};

inline ePackageType CNetworkMessage_PingResponse::GetPackageType() const
{
	return ePackageType::ePingResponse;
}
