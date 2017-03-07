#pragma once
#include "NetworkMessage.h"
#include "../CommonUtilities/vector3.h"
class CNetworkMessage_PlayerPositionMessage : public CNetworkMessage
{
public:
	CNetworkMessage_PlayerPositionMessage();
	~CNetworkMessage_PlayerPositionMessage();
	ePackageType GetPackageType()const override;

	const CU::Vector3f& GetPosition();
	const unsigned GetID();
	void SetPosition(const CU::Vector3f& aPosition);
	void SetID(const unsigned aClientID);

private:

	virtual void DoSerialize(StreamType& aStream);
	virtual void DoDeserialize(StreamType& aStream);

	unsigned myID;
	CU::Vector3f myPosition;

};

