#pragma once
#include "Message.h"
class CStartWaveMessage: public Postmaster::Message::IMessage
{
public:
	CStartWaveMessage(const unsigned char aWave, const short aArenaID);
	~CStartWaveMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	const unsigned short GetWave() const;
	const short GetArenaID() const;

private:
	short myArenaID;
	unsigned short myWave;


};

