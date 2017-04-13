#include "HealthPickupGrabbed.h"
#include "..\PostMaster\Message.h"


CHealthPickupGrabbed::CHealthPickupGrabbed(const int aId) : IMessage(eMessageType::eNetworkMessage), myID(aId)
{
}


CHealthPickupGrabbed::~CHealthPickupGrabbed()
{
}

const int CHealthPickupGrabbed::GetId() const
{
	return myID;
}
