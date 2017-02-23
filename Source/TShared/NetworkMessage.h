#pragma once

#include "SerializeHelper.h"
#include "PackageType.h"


struct SNetworkPackageHeader
{
	__int8 myPackageType;
	__int32 myTimeStamp;
	__int16 mySenderID;
	__int16 myTargetID;
};


class CNetworkMessage
{
public:
	CNetworkMessage();
	virtual ~CNetworkMessage();

	void PackMessage();
	void UnpackMessage();

	template <typename TYPE>
	TYPE* CastTo();

	void SetData(const char* someData, unsigned int dataSize);
	void SetData(StreamType aStream);

	void SetExplicitHeader(SNetworkPackageHeader& aHeader);
	virtual void SetHeader(SNetworkPackageHeader aHeader);

	const SNetworkPackageHeader& GetHeader() const;
	const StreamType& GetSerializedData() const;

	virtual ePackageType GetPackageType()const;
protected:
	StreamType myStream;

	virtual void DoSerialize(StreamType& aStream);
	virtual void DoDeserialize(StreamType& aStream);

	SNetworkPackageHeader myHeader;
};

template <typename TYPE>
TYPE* CNetworkMessage::CastTo()
{
	CNetworkMessage* tempMessage = static_cast<CNetworkMessage*>(new TYPE);
	tempMessage->SetData(myStream);
	tempMessage->SetHeader(myHeader);
	tempMessage->UnpackMessage();
	return static_cast<TYPE*>(tempMessage);
}
