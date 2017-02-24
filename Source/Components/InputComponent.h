#pragma once
#include "Component.h"
#include "PlayerControls.h"
#include "..\CommonUtilities\InputListener.h"

namespace CU
{
	enum class eKeys;
	enum class eMouseButtons;
}

class CInputComponent :	public CComponent, public CU::IInputListener
{
public:
	CInputComponent();
	~CInputComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Update();

private:
	CU::eInputReturn TakeInput(const CU::SInputMessage& aInputMessage) override;

	void MouseMoved(const CU::Vector2f aMouseDeltaNormalized);

	void KeyPressed(const CU::eKeys aKey);
	void KeyReleased(const CU::eKeys aKey);
private:
	CU::StaticArray<CU::eKeys, static_cast<int>(ePlayerControls::eLength)> myControlMap;
	bool myIsLeftMouseButtonPressed;
};
