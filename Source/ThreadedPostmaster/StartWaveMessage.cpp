#include "StartWaveMessage.h"
#include "../PostMaster/MessageType.h"
#include "Subscriber.h"
#include "../CommonUtilities/DL_Debug.h"


CStartWaveMessage::CStartWaveMessage(const unsigned char aWave, const short aArenaID) : IMessage(eMessageType::eSpawnWave), myWave(aWave), myArenaID(aArenaID)
{
	DL_PRINT("StartWaveMessage");
}

CStartWaveMessage::~CStartWaveMessage()
{
}

Postmaster::Message::IMessage * CStartWaveMessage::Copy()
{
	return new CStartWaveMessage(myWave, myArenaID);
}

eMessageReturn CStartWaveMessage::DoEvent(::Postmaster::ISubscriber & aSubscriber) const
{
	return  aSubscriber.DoEvent(*this);;
}

const unsigned short CStartWaveMessage::GetWave() const
{
	return myWave;
}

const short CStartWaveMessage::GetArenaID() const
{
	return myArenaID;
}
