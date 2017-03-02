#pragma once
#include "NetworkMessage.h"
#include "../CommonUtilities/vector3.h"

class CNetworkMessage_Position: public CNetworkMessage
{
public:
	CNetworkMessage_Position();
	~CNetworkMessage_Position();

	const CU::Vector3f& GetPosition();
	const unsigned GetID();
	void SetPosition(const CU::Vector3f& aPosition);
	void SetID(const unsigned aNetworkComponentID);
	ePackageType GetPackageType()const override;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;

	CU::Vector3f myPosition;
	unsigned myNetworkComponentID;

};

