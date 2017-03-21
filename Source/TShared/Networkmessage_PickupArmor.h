#pragma once
#include "ImportantNetworkMessage.h"
class CNetworkmessage_PickupArmor : public CImportantNetworkMessage
{
public:
	CNetworkmessage_PickupArmor();
	~CNetworkmessage_PickupArmor();

	ePackageType GetPackageType()const override;

	const int GetID();
	void SetID(const int aId);
private:
	int myNetworkID;

	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

