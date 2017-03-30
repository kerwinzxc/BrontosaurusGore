#include "stdafx.h"
#include "NetworkMessage_DoorMessage.h"


CNetworkMessage_DoorMessage::CNetworkMessage_DoorMessage()
{
	myKeyID = -1;
}


CNetworkMessage_DoorMessage::~CNetworkMessage_DoorMessage()
{
}

ePackageType CNetworkMessage_DoorMessage::GetPackageType() const
{
	return ePackageType::eDoorMessage;
}

void CNetworkMessage_DoorMessage::SetID(const unsigned char aNetworkID)
{
	myNetworkID = aNetworkID;
}

void CNetworkMessage_DoorMessage::SetDoorAction(const eDoorAction aDoorAction)
{
	myDoorAction = aDoorAction;
}

void CNetworkMessage_DoorMessage::SetKeyID(const short aKeyID)
{
	myKeyID = aKeyID;
}

const short CNetworkMessage_DoorMessage::GetKeyID() const
{
	return myKeyID;
}

const unsigned char CNetworkMessage_DoorMessage::GetNetworkID() const
{
	return myNetworkID;
}

const eDoorAction CNetworkMessage_DoorMessage::GetDoorAction() const
{
	return myDoorAction;
}

void CNetworkMessage_DoorMessage::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myDoorAction, aStream);
	serialize(myNetworkID, aStream);
}

void CNetworkMessage_DoorMessage::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myDoorAction = deserialize<eDoorAction>(aStream);
	myNetworkID = deserialize<unsigned char>(aStream);
}
