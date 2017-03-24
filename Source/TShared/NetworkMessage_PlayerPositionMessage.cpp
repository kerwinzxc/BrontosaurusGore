#include "stdafx.h"
#include "NetworkMessage_PlayerPositionMessage.h"
#include "../Physics/Physics.h"
#include "../Physics/PhysXHelper.h"


CNetworkMessage_PlayerPositionMessage::CNetworkMessage_PlayerPositionMessage()
{
}


CNetworkMessage_PlayerPositionMessage::~CNetworkMessage_PlayerPositionMessage()
{
}

ePackageType CNetworkMessage_PlayerPositionMessage::GetPackageType() const
{
	return ePackageType::ePlayerPosition;
}


const CU::Matrix44f& CNetworkMessage_PlayerPositionMessage::GetTransformation()
{
	CU::Matrix44f transform = Physics::QuatToMatrix(myRotation);
	transform.SetPosition(myPosition);
	return transform;
}

const unsigned CNetworkMessage_PlayerPositionMessage::GetID()
{
	return myID;
}

void CNetworkMessage_PlayerPositionMessage::SetTransformation(const CU::Matrix44f& aTransform)
{
	myRotation = Physics::MatrixToQuat(aTransform);
	myPosition = aTransform.GetPosition();
}


void CNetworkMessage_PlayerPositionMessage::SetID(const unsigned aClientID)
{
	myID = aClientID;
}

void CNetworkMessage_PlayerPositionMessage::DoSerialize(StreamType & aStream)
{
	serialize(myRotation, aStream);
	serialize(myPosition, aStream);
	serialize(myID, aStream);
}

void CNetworkMessage_PlayerPositionMessage::DoDeserialize(StreamType & aStream)
{
	if (aStream.size() > 0)
	{
		myRotation = deserialize<physx::PxQuat>(aStream);
		myPosition = deserialize < CU::Vector3f > (aStream);
		myID = deserialize<unsigned>(aStream);
	}
}
