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

private:
	CU::eInputReturn TakeInput(const CU::SInputMessage& aInputMessage) override;

	void MouseMoved(const CU::Vector2f aMouseDeltaNormalized);

	void MousePressed(const CU::eMouseButtons aMouseButton);
	void MouseReleased(const CU::eMouseButtons aMouseButton);

	void KeyPressed(const CU::eKeys aKey);
	void KeyReleased(const CU::eKeys aKey);

private:
	CU::StaticArray</*CU::eKeys*/int, static_cast<int>(ePlayerControls::eLength)> myControlMap;
	//bool myIsLeftMouseButtonPressed;
};
