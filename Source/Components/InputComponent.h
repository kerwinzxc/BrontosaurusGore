#pragma once
#include "Component.h"

class CInputComponent :	public CComponent
{
public:
	CInputComponent();
	~CInputComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:

};
