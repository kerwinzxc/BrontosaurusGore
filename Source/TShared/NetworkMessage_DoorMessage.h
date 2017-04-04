#pragma once
#include "ImportantNetworkMessage.h"

enum class eDoorAction
{
	eOpen,
	eClose,
	eLock,
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

	void SetKeyID(const short aKeyID);

	const short GetKeyID() const;
	const unsigned char GetNetworkID() const;
	const eDoorAction GetDoorAction() const;

private:
	eDoorAction myDoorAction;

	short myKeyID;
	unsigned char myNetworkID;

	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

