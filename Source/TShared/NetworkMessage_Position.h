#pragma once
#include "NetworkMessage.h"
#include "../CommonUtilities/vector3.h"

class CNetworkMessage_Position: public CNetworkMessage
{
public:
	friend class CMessageManager;

	CNetworkMessage_Position();
	~CNetworkMessage_Position();

	const CU::Vector3f& GetPosition();
	void SetPosition(const CU::Vector3f& aPosition);
	ePackageType GetPackageType()const override;
private:


	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;

	CU::Vector3f myPosition;
};

