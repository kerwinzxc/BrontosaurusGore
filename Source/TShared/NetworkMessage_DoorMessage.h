#pragma once
#include "ImportantNetworkMessage.h"

enum class eDoorAction
{
	eOpen,
	eClose,
	eUnlock
};

class CNetworkMessage_DoorMessage : public CImportantNetworkMessage
{
public:
	CNetworkMessage_DoorMessage();
	~CNetworkMessage_DoorMessage();


	ePackageType GetPackageType()const override;

	void SetID(const unsigned char aNetworkID);
	void SetDoorAction(const eDoorAction aDoorAction);

	const unsigned char GetNetworkID() const;
	const eDoorAction GetDoorAction() const;

private:
	eDoorAction myDoorAction;

	unsigned char myNetworkID;


	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

