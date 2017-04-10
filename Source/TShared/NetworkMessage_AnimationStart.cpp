#include "stdafx.h"
#include "NetworkMessage_AnimationStart.h"
#include "../Components/ComponentMessage.h"


CNetworkMessage_AnimationStart::CNetworkMessage_AnimationStart()
{
	myEnemyID = static_cast<unsigned int>(-1);
	myAnimation = eComponentMessageType::eLength;
}

CNetworkMessage_AnimationStart::~CNetworkMessage_AnimationStart()
{
}

void CNetworkMessage_AnimationStart::Init(const unsigned int aEnemyID, const eComponentMessageType aAnimation)
{
	myEnemyID = aEnemyID;
	myAnimation = aAnimation;
}

unsigned int CNetworkMessage_AnimationStart::GetEnemyID() const
{
	return myEnemyID;
}

eComponentMessageType CNetworkMessage_AnimationStart::GetAnimation() const
{
	return myAnimation;
}

ePackageType CNetworkMessage_AnimationStart::GetPackageType() const
{
	return ePackageType::eAnimationStart;
}

void CNetworkMessage_AnimationStart::DoSerialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	SERIALIZE(myEnemyID, aStream);
	SERIALIZE(myAnimation, aStream);
}

void CNetworkMessage_AnimationStart::DoDeserialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	DESERIALIZE(myEnemyID, aStream);
	DESERIALIZE(myAnimation, aStream);
}
