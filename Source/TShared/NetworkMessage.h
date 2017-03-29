#pragma once

#include "SerializeHelper.h"
#include "PackageType.h"
#include "MessageStorage.h"

#define SERIALIZE(aValue, aStreamType) serialize(aValue, aStreamType)
#define DESERIALIZE(aValue, aStreamType) aValue = deserialize<decltype(aValue)>(aStreamType)

struct SNetworkPackageHeader
{
	SNetworkPackageHeader()
	{
		myPackageType = ePackageType::eZero;
		myTargetID = mySenderID;
		mySenderID = mySenderID;
		myTimeStamp = 0;
	}

	ePackageType myPackageType; //TODO: change to ePackageType 
	__int32 myTimeStamp;
	__int16 mySenderID;
	__int16 myTargetID;
};


class CNetworkMessage
{
public:
	friend class CMessageManager;

	CNetworkMessage();
	virtual ~CNetworkMessage();

	void PackMessage();
	void UnpackMessage();

	template <typename MESSAGE_TYPE>
	MESSAGE_TYPE* CastTo();

	void SetData(const char* someData, unsigned int dataSize);
	void SetData(StreamType aStream);
	void ClearStream();

	void SetExplicitHeader(SNetworkPackageHeader& aHeader);
	virtual void SetHeader(SNetworkPackageHeader aHeader);
	virtual bool IsImportant();

	const SNetworkPackageHeader& GetHeader() const;
	const StreamType& GetSerializedData() const;

	virtual ePackageType GetPackageType()const;
protected:

	StreamType myStream;

	virtual void DoSerialize(StreamType& aStream);
	virtual void DoDeserialize(StreamType& aStream);

	SNetworkPackageHeader myHeader;

	bool myIsPacked;
};

template <typename MESSAGE_TYPE>
MESSAGE_TYPE* CNetworkMessage::CastTo()
{
	if (CMessageStorage::GetInstance()->CheckTypeExist<MESSAGE_TYPE>() == false)
	{
		CMessageStorage::GetInstance()->AddType<MESSAGE_TYPE>(new MESSAGE_TYPE);
	}

	CNetworkMessage* message = CMessageStorage::GetInstance()->GetType<MESSAGE_TYPE>();

	message->SetData(myStream);
	message->SetHeader(myHeader);
	message->UnpackMessage();
	return static_cast<MESSAGE_TYPE*>(message);
}

inline bool CNetworkMessage::IsImportant()
{
	return false;
}
