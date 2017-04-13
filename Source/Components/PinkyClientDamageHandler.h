#pragma once
#include "Component.h"
class CPinkyClientDamageHandler : public CComponent
{
public:
	CPinkyClientDamageHandler();
	~CPinkyClientDamageHandler();

private:
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	float myCountDown;
};

