#pragma once
#include "ImportantNetworkMessage.h"

enum class eComponentMessageType;

class CNetworkMessage_AnimationStart : public CImportantNetworkMessage
{
public:
	CNetworkMessage_AnimationStart();
	~CNetworkMessage_AnimationStart();

	void Init(const unsigned int aEnemyID, const eComponentMessageType aAnimation);
	unsigned int GetEnemyID() const;
	eComponentMessageType GetAnimation() const;

	virtual ePackageType GetPackageType() const override;

private:
	virtual void DoSerialize(StreamType& aStream) override;
	virtual void DoDeserialize(StreamType& aStream) override;

	unsigned int myEnemyID;
	eComponentMessageType myAnimation;
};

