#include "AddEnemyToWave.h"
#include "../PostMaster/MessageType.h"
#include "Subscriber.h"
#include "Message.h"


CAddEnemyToWave::CAddEnemyToWave(CEnemy * aEnemy, const short aArenaID): IMessage(eMessageType::eAddEnemyToWave), myEnemyInWave(aEnemy), myArenaID(aArenaID)
{
}

CAddEnemyToWave::~CAddEnemyToWave()
{
}

Postmaster::Message::IMessage * CAddEnemyToWave::Copy()
{
	return new CAddEnemyToWave(myEnemyInWave, myArenaID);
}

eMessageReturn CAddEnemyToWave::DoEvent(::Postmaster::ISubscriber & aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

CEnemy * CAddEnemyToWave::GetEnemy() const
{
	return myEnemyInWave;
}

const short CAddEnemyToWave::GetArenaID() const
{
	return myArenaID;
}

