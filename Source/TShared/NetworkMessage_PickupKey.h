#pragma once
#include "ImportantNetworkMessage.h"
class CNetworkMessage_PickupKey : public CImportantNetworkMessage
{
public:
	CNetworkMessage_PickupKey();
	~CNetworkMessage_PickupKey();

	ePackageType GetPackageType()const override;

	void SetID(const int aId);
	const int GetNetWorkID() const;
	void SetLockID(const short aLockID);
	const short GetLockID() const;
private:
	int myNetWorkId;
	short myLockID;


	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

