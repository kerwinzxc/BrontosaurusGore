#pragma once
#include "ImportantNetworkMessage.h"

class CNetworkMessage_TakeDamage : public CImportantNetworkMessage
{
public:
	CNetworkMessage_TakeDamage();
	~CNetworkMessage_TakeDamage();

	ePackageType GetPackageType()const override;

	const unsigned int GetID();
	void SetID(const int aId);

	const unsigned int GetDamageTaken();
	void SetDamageTaken(const unsigned int aDamageTaken);

private:
	unsigned int myNetworkID;
	unsigned int myDamageTaken;

	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;

};

