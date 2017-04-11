#pragma once
#include "Component.h"

class CLutComponent : public CComponent
{
public:
	CLutComponent();
	~CLutComponent();

	virtual void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	void SendHurtLutMessage();
};
