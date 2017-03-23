#include "stdafx.h"
#include "InputComponent.h"
#include "..\CommonUtilities\InputMessenger.h"
#include "..\CommonUtilities\InputMessage.h"
#include "..\CommonUtilities\EMouseButtons.h"
#include "..\CommonUtilities\EKeyboardKeys.h"
#include "..\CommonUtilities\EInputReturn.h"

#include "..\BrontosaurusEngine\Engine.h"

#include "../ThreadedPostmaster/SetAsNewCheckPointMessage.h"
#include "../ThreadedPostmaster/ResetToCheckPointMessage.h"
#include "../ThreadedPostmaster/Postmaster.h"
#ifdef INTIFY
#error "You are breaking windows API"
#endif // INTIFY
#define INTIFY(ENUM_TYPE) (static_cast<int>(ENUM_TYPE))

int GetInputButton(const CU::eKeys aKey);
int GetInputButton(const CU::eMouseButtons aKey);

CInputComponent::CInputComponent()
{
	CU::CInputMessenger* inputMessenger = CU::CInputMessenger::GetInstance(eInputMessengerType::ePlayState);
	assert(inputMessenger != nullptr && "maybe create a init if input messenger (playstate?) is not created yet");

	if (inputMessenger)
	{
		inputMessenger->Subscribe(*this);
	}

	myControlMap[INTIFY(ePlayerControls::eForward)] = GetInputButton(CU::eKeys::W);
	myControlMap[INTIFY(ePlayerControls::eLeft)] = GetInputButton(CU::eKeys::A);
	myControlMap[INTIFY(ePlayerControls::eBackward)] = GetInputButton(CU::eKeys::S);
	myControlMap[INTIFY(ePlayerControls::eRight)] = GetInputButton(CU::eKeys::D);
	myControlMap[INTIFY(ePlayerControls::eJump)] = GetInputButton(CU::eKeys::SPACE);
	myControlMap[INTIFY(ePlayerControls::eActivate)] = GetInputButton(CU::eKeys::E);
	myControlMap[INTIFY(ePlayerControls::ePrimaryWeapon)] = GetInputButton(CU::eMouseButtons::LBUTTON);
	myControlMap[INTIFY(ePlayerControls::eSecondaryWeapon)] = GetInputButton(CU::eMouseButtons::RBUTTON);

	myControlMap[INTIFY(ePlayerControls::eNoclip)] = GetInputButton(CU::eKeys::BACKSLASH);

	myControlMap[INTIFY(ePlayerControls::eDown)] = GetInputButton(CU::eKeys::LCONTROL);
}

CInputComponent::~CInputComponent()
{
}

void CInputComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eObjectDone:
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSetAsNewCheckPointMessage(GetParent()->GetWorldPosition()));
		break;
	case eComponentMessageType::eCheckPointReset :
		GetParent()->GetLocalTransform().SetPosition(aMessageData.myVector3f);
		GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		GetParent()->NotifyComponents(eComponentMessageType::eSetControllerPosition, aMessageData);
		break;
	default:
		break;
	}
}

CU::eInputReturn CInputComponent::TakeInput(const CU::SInputMessage& aInputMessage)
{
	switch (aInputMessage.myType)
	{
	case CU::eInputType::eMouseMoved:
		MouseMoved(aInputMessage.myMouseDelta);
		NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
		break;
	case CU::eInputType::eMousePressed:
		MousePressed(aInputMessage.myMouseButton);
		break;
	case CU::eInputType::eMouseReleased:
		MouseReleased(aInputMessage.myMouseButton);
		break;
	case CU::eInputType::eKeyboardPressed:
		KeyPressed(aInputMessage.myKey);
		break;
	case CU::eInputType::eKeyboardReleased:
		KeyReleased(aInputMessage.myKey);
		break;
	case CU::eInputType::eScrollWheelChanged:
	{
		SComponentMessageData scrollWheelData;
		scrollWheelData.myInt = static_cast<int>(aInputMessage.myMouseWheelDelta.x);
		GetParent()->NotifyComponents(eComponentMessageType::eChangeWeapon, scrollWheelData);
		break;
	}
	}
	return CU::eInputReturn::ePassOn;
}

void CInputComponent::MouseMoved(const CU::Vector2f aMouseDeltaNormalized)
{
	const float yaw = aMouseDeltaNormalized.x * (WINDOW_SIZE_F.x / WINDOW_SIZE_F.y);
	const float pitch = aMouseDeltaNormalized.y;

	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();

	parentTransform.RotateAroundAxis(yaw, CU::Axees::Y);

	SComponentMessageData data;
	data.myFloat = pitch;
	NotifyParent(eComponentMessageType::ePitch, data);
}

void CInputComponent::MousePressed(const CU::eMouseButtons aMouseButton)
{
	int index = myControlMap.Find(GetInputButton(aMouseButton));
	if (index != myControlMap.FoundNone)
	{
		SComponentMessageData data;
		data.myPlayerControl = static_cast<ePlayerControls>(index);
		NotifyParent(eComponentMessageType::eKeyPressed, data);
	}
}

void CInputComponent::MouseReleased(const CU::eMouseButtons aMouseButton)
{
	int index = myControlMap.Find(GetInputButton(aMouseButton));
	if (index != myControlMap.FoundNone)
	{
		SComponentMessageData data;
		data.myPlayerControl = static_cast<ePlayerControls>(index);
		NotifyParent(eComponentMessageType::eKeyReleased, data);
	}
}

void CInputComponent::KeyPressed(const CU::eKeys aKey)
{
	int index = myControlMap.Find(GetInputButton(aKey));
	if (index != myControlMap.FoundNone)
	{
		SComponentMessageData data;
		data.myPlayerControl = static_cast<ePlayerControls>(index);
		NotifyParent(eComponentMessageType::eKeyPressed, data);
	}
}

void CInputComponent::KeyReleased(const CU::eKeys aKey)
{
	int index = myControlMap.Find(GetInputButton(aKey));
	if (index != myControlMap.FoundNone)
	{
		SComponentMessageData data;
		data.myPlayerControl = static_cast<ePlayerControls>(index);
		NotifyParent(eComponentMessageType::eKeyReleased, data);
	}
}

int GetInputButton(const CU::eKeys aKey)
{
	return static_cast<int>(aKey);
}

int GetInputButton(const CU::eMouseButtons aMouseButton)
{
	return static_cast<int>(CU::eKeys::LENGTH) + static_cast<int>(aMouseButton);
}
