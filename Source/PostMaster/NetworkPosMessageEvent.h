#pragma once
#include "Event.h"
#include "../CommonUtilities/vector3.h"

class CNetworkPosMessageEvent: public Event
{
public:
	CNetworkPosMessageEvent();
	~CNetworkPosMessageEvent();

private:
	const CU::Vector3f myPosition;
};