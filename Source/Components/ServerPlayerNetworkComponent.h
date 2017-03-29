#pragma once
#include "Component.h"

class CServerPlayerNetworkComponent :public CComponent
{
public:
	CServerPlayerNetworkComponent();
	~CServerPlayerNetworkComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	void SetClientID(const unsigned short aClientID);

private:
	unsigned short myClientID;

};

