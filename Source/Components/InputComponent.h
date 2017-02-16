#pragma once
#include "Component.h"
#include "..\CommonUtilities\InputListener.h"

class CInputComponent :	public CComponent, public CU::IInputListener
{
public:
	CInputComponent();
	~CInputComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	CU::eInputReturn TakeInput(const CU::SInputMessage& aInputMessage) override;

	void MouseMoved(const CU::Vector2f aMouseDeltaNormalized);
};
