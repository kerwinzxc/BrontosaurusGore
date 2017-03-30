#pragma once
#include "Message.h"
class CPlayerEnteredArena : public Postmaster::Message::IMessage
{
public:
	CPlayerEnteredArena(const unsigned char aChange, const unsigned char aAmountOfWaves , const short aKeyId);
	~CPlayerEnteredArena();

	const short GetKeyId() const;
	const unsigned char GetWaveAmount() const;
	const unsigned char GetPlayerChange() const;

	IMessage * Copy() override;
	eMessageReturn DoEvent(::Postmaster::ISubscriber & aSubscriber) const override;
private:
	short myKeyId;
	unsigned char myAmountOfWaves;
	unsigned char myPlayerChange;

	// Inherited via IMessage
};

