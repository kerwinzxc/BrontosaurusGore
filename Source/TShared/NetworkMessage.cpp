#include "stdafx.h"
#include "NetworkMessage.h"
#include "TShared_NetworkWrapper.h"
#include "../CommonUtilities/picojson.h"


CNetworkMessage::CNetworkMessage()
{
	myHeader.myPackageType = static_cast<ePackageType>(0);
	myHeader.myTargetID = ID_SERVER;
	myIsPacked = false;
}


CNetworkMessage::~CNetworkMessage()
{

}

void CNetworkMessage::PackMessage()
{
	if (myIsPacked == false)
	{
		myStream.clear();
		DoSerialize(myStream);
		myIsPacked = true;
	}
}

void CNetworkMessage::UnpackMessage()
{
	if (myIsPacked == true)
	{
		if (myStream.empty() == false)
		{
			DoDeserialize(myStream);
			myIsPacked = false;
		}
		return;
	}
	DL_ASSERT("Message was not packed");
}

void CNetworkMessage::SetData(const char* someData, unsigned dataSize)
{
	assert(dataSize > 0);
	myStream.assign(someData, someData + dataSize);
}

void CNetworkMessage::SetData(StreamType aStream)
{
	myStream = aStream;
	myIsPacked = true;
}
void CNetworkMessage::ClearStream()
{
	myStream.clear();
	myIsPacked = false;
}

void CNetworkMessage::SetExplicitHeader(SNetworkPackageHeader& aHeader)
{
	myHeader = aHeader;
}

void CNetworkMessage::SetHeader(SNetworkPackageHeader aHeader)
{
	myHeader = aHeader;
	myHeader.myPackageType = (GetPackageType());
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
