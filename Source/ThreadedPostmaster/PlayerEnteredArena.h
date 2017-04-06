#pragma once
#include "Message.h"
class CPlayerEnteredArena : public Postmaster::Message::IMessage
{
public:
	CPlayerEnteredArena(const short aChange, const unsigned char aAmountOfWaves , const short aKeyId);
	~CPlayerEnteredArena();

	const short GetKeyId() const;
	const unsigned char GetWaveAmount() const;
	const short GetPlayerChange() const;

	IMessage * Copy() override;
	eMessageReturn DoEvent(::Postmaster::ISubscriber & aSubscriber) const override;
private:
	short myKeyId;
	unsigned char myAmountOfWaves;
	short myPlayerChange;

	// Inherited via IMessage
};

