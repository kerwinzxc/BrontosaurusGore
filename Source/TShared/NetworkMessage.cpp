#include "stdafx.h"
#include "NetworkMessage.h"
#include "TShared_NetworkWrapper.h"
#include "../CommonUtilities/picojson.h"


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

void CNetworkMessage::SetExplicitHeader(SNetworkPackageHeader& aHeader)
{
	myHeader = aHeader;
}

void CNetworkMessage::SetHeader(SNetworkPackageHeader aHeader)
{
	myHeader = aHeader;
	myHeader.myPackageType = static_cast<char>(GetPackageType());
}

const SNetworkPackageHeader& CNetworkMessage::GetHeader() const
{
	return myHeader;
}

const StreamType& CNetworkMessage::GetSerializedData() const
{
	return myStream;
}

ePackageType CNetworkMessage::GetPackageType() const
{
	return ePackageType::eZero;
}

void CNetworkMessage::DoSerialize(StreamType& aStream)
{
}

void CNetworkMessage::DoDeserialize(StreamType& aStream)
{
}
