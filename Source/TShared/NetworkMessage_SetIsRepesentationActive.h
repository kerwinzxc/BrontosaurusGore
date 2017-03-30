#pragma once
#include "ImportantNetworkMessage.h"

#define Ja true
#define Nej false
#define Kanske static_cast<bool>(rand() % 2)
#define Mojligen static_cast<bool>(rand() % 1000)
#define TypInte static_cast<bool>(rand() % 1000000)

class CNetworkMessage_SetIsRepesentationActive: public CImportantNetworkMessage
{
public:
	CNetworkMessage_SetIsRepesentationActive();
	~CNetworkMessage_SetIsRepesentationActive();

	ePackageType GetPackageType()const override;

	void SetActive(const bool aFlag);
	void SetNetworkID(const unsigned short aID);

	const bool GetActive() const;
	const unsigned short GetNetworkID() const;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;

	unsigned short myNetworkID;
	bool myIsActive;
};

