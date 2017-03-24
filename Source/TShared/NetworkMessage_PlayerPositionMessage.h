#pragma once
#include "NetworkMessage.h"
#include "../CommonUtilities/vector3.h"
#include "../CommonUtilities/matrix44.h"
#include "../Physics/Include/pxShared/foundation/PxQuat.h"

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
	void SetID(const unsigned aClientID);
private:

	virtual void DoSerialize(StreamType& aStream);
	virtual void DoDeserialize(StreamType& aStream);

	unsigned myID;
	physx::PxQuat myRotation;
	CU::Vector3f myPosition;
};

