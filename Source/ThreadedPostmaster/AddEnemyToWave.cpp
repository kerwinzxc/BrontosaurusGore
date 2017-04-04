#include "AddEnemyToWave.h"
#include "../PostMaster/MessageType.h"
#include "Subscriber.h"
#include "Message.h"


CAddEnemyToWave::CAddEnemyToWave(CEnemy * aEnemy): IMessage(eMessageType::eAddEnemyToWave), myEnemyInWave(aEnemy)
{
}

CAddEnemyToWave::~CAddEnemyToWave()
{
}

Postmaster::Message::IMessage * CAddEnemyToWave::Copy()
{
	return new CAddEnemyToWave(myEnemyInWave);
}

eMessageReturn CAddEnemyToWave::DoEvent(::Postmaster::ISubscriber & aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

CEnemy * CAddEnemyToWave::GetEnemy() const
{
	return myEnemyInWave;
}

