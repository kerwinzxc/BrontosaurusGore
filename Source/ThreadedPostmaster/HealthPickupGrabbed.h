#pragma once
#include "Message.h"
class CHealthPickupGrabbed : public Postmaster::Message::IMessage
{
public:
	CHealthPickupGrabbed(const int aId);
	~CHealthPickupGrabbed();

	const int GetId() const;
private:
	int myID;
};

