#include "stdafx.h"
#include "NetworkMessage.h"
#include "TShared_NetworkWrapper.h"


CNetworkMessage::CNetworkMessage()
{
	myHeader.myPackageType = 0;
	myHeader.myTargetID = ID_SERVER;
}


CNetworkMessage::~CNetworkMessage()
{

}

void CNetworkMessage::PackMessage()
{
	DoSerialize(myStream);
}

void CNetworkMessage::UnpackMessage()
{
	DoDeserialize(myStream);
}

void CNetworkMessage::SetData(const char* someData, unsigned dataSize)
{
	myStream.assign(someData, someData + dataSize);
}

void CNetworkMessage::SetData(StreamType aStream)
{
	myStream = aStream;
}

void CNetworkMessage::SetHeader(SNetworkPackageHeader aHeader)
{
	myHeader = aHeader;
}

const SNetworkPackageHeader& CNetworkMessage::GetHeader() const
{
	return myHeader;
}

const StreamType& CNetworkMessage::GetSerializedData() const
{
	return myStream;
}

void CNetworkMessage::DoSerialize(StreamType& aStream)
{
}

void CNetworkMessage::DoDeserialize(StreamType& aStream)
{
}
