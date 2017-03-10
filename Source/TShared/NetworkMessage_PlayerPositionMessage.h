#pragma once
#include "NetworkMessage.h"
#include "../CommonUtilities/vector3.h"
#include "../CommonUtilities/matrix44.h"
class CNetworkMessage_PlayerPositionMessage : public CNetworkMessage
{
public:
	CNetworkMessage_PlayerPositionMessage();
	~CNetworkMessage_PlayerPositionMessage();
	ePackageType GetPackageType()const override;

	const CU::Matrix44f& GetTransformation();
	//const CU::Vector3f& GetPosition();
	const unsigned GetID();
	void SetTransformation(const CU::Matrix44f& aTransform);
	void SetPosition(const CU::Vector3f& aPosition);
	void SetID(const unsigned aClientID);

private:

	virtual void DoSerialize(StreamType& aStream);
	virtual void DoDeserialize(StreamType& aStream);

	unsigned myID;
	CU::Matrix44f myTransform;

};

