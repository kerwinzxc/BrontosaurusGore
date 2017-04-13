#pragma once
#include "ImportantNetworkMessage.h"
class CNetworkMessage_PickupHealth : public CImportantNetworkMessage
{
public:
	CNetworkMessage_PickupHealth();
	~CNetworkMessage_PickupHealth();

	ePackageType GetPackageType()const override;

	const int GetID();
	void SetID(const int aId);
private:
	int myNetworkID;

	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

