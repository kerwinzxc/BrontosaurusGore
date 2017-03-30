#include "StartWaveMessage.h"
#include "../PostMaster/MessageType.h"
#include "Subscriber.h"


CStartWaveMessage::CStartWaveMessage(const unsigned char aWave) : IMessage(eMessageType::eSpawnWave), myWave(aWave)
{

}

CStartWaveMessage::~CStartWaveMessage()
{
}

Postmaster::Message::IMessage * CStartWaveMessage::Copy()
{
	return new CStartWaveMessage(myWave);
}

eMessageReturn CStartWaveMessage::DoEvent(::Postmaster::ISubscriber & aSubscriber) const
{
	return  aSubscriber.DoEvent(*this);;
}

const unsigned char CStartWaveMessage::GetWave() const
{
	return myWave;
}
