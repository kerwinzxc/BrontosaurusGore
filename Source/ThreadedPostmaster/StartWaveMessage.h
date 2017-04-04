#pragma once
#include "Message.h"
class CStartWaveMessage: public Postmaster::Message::IMessage
{
public:
	CStartWaveMessage(const unsigned char aWave);
	~CStartWaveMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	const unsigned char GetWave() const;

private:

	unsigned char myWave;


};
