#pragma once
#include "Message.h"

class CEnemy;

class CAddEnemyToWave : public Postmaster::Message::IMessage
{
public:
	CAddEnemyToWave(CEnemy* aEnemy, const short aArenaID);
	~CAddEnemyToWave();

	virtual IMessage* Copy() override;

	virtual eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	CEnemy* GetEnemy() const;
	const short GetArenaID() const;

private:
	CEnemy* myEnemyInWave;
	short myArenaID;
};

