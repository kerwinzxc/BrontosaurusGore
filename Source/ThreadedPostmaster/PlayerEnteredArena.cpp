#include "PlayerEnteredArena.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/Subscriber.h"

CPlayerEnteredArena::CPlayerEnteredArena(const unsigned char aChange, const unsigned char aAmountOfWaves , const short aKeyId) : IMessage(eMessageType::ePlayerEnterArena)
{
	myAmountOfWaves = aAmountOfWaves;
	myKeyId = aKeyId;
	myPlayerChange = aChange;
}


CPlayerEnteredArena::~CPlayerEnteredArena()
{
}

const short CPlayerEnteredArena::GetKeyId() const
{
	return myKeyId;
}

const unsigned char CPlayerEnteredArena::GetWaveAmount() const
{
	return myAmountOfWaves;
}

const unsigned char CPlayerEnteredArena::GetPlayerChange() const
{
	return myPlayerChange;
}

Postmaster::Message::IMessage * CPlayerEnteredArena::Copy()
{
	return new CPlayerEnteredArena(myPlayerChange,myAmountOfWaves,myKeyId);
}

eMessageReturn CPlayerEnteredArena::DoEvent(::Postmaster::ISubscriber & aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
