#include "stdafx.h"
#include "InputComponent.h"
#include "..\CommonUtilities\InputMessenger.h"
#include "..\CommonUtilities\InputMessage.h"
#include "..\CommonUtilities\EMouseButtons.h"
#include "..\CommonUtilities\EKeyboardKeys.h"
#include "..\CommonUtilities\EInputReturn.h"

#ifdef INTIFY
#error "You are breaking windows API"
#endif // INTIFY
#define INTIFY(ENUM_TYPE) (static_cast<int>(ENUM_TYPE))

CInputComponent::CInputComponent()
{
	CU::CInputMessenger* inputMessenger = CU::CInputMessenger::GetInstance(eInputMessengerType::ePlayState);
	assert(inputMessenger != nullptr && "maybe create a init if input messenger (playstate?) is not created yet");

	if (inputMessenger)
	{
		inputMessenger->Subscribe(*this);
	}

	myControlMap[INTIFY(ePlayerControls::eForward)] = CU::eKeys::W;
	myControlMap[INTIFY(ePlayerControls::eLeft)] = CU::eKeys::A;
	myControlMap[INTIFY(ePlayerControls::eBackward)] = CU::eKeys::S;
	myControlMap[INTIFY(ePlayerControls::eRight)] = CU::eKeys::D;
	myControlMap[INTIFY(ePlayerControls::eJump)] = CU::eKeys::SPACE;
	myControlMap[INTIFY(ePlayerControls::eActivate)] = CU::eKeys::E;

	myIsLeftMouseButtonPressed = false;
}

CInputComponent::~CInputComponent()
{
}

void CInputComponent::Receive(const eComponentMessageType /*aMessageType*/, const SComponentMessageData& /*aMessageData*/)
{
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
	{
		aInputMessage.myMouseButton;
		aInputMessage.myMousePosition;

		myIsLeftMouseButtonPressed = true;
		break;
	}
	case CU::eInputType::eMouseReleased:
		aInputMessage.myMouseButton;
		aInputMessage.myMousePosition;

		myIsLeftMouseButtonPressed = false;
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
		scrollWheelData.myInt = aInputMessage.myMouseWheelDelta.x;
		GetParent()->NotifyComponents(eComponentMessageType::eChangeWeapon, scrollWheelData);
		break;
	}
	}
	return CU::eInputReturn::ePassOn;
}

void CInputComponent::MouseMoved(const CU::Vector2f aMouseDeltaNormalized)
{
	const float yaw = aMouseDeltaNormalized.x;
	const float pitch = aMouseDeltaNormalized.y;

	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();

	parentTransform.RotateAroundAxis(yaw, CU::Axees::Y);

	SComponentMessageData data;
	data.myFloat = pitch;
	NotifyParent(eComponentMessageType::ePitch, data);
}

void CInputComponent::KeyPressed(const CU::eKeys aKey)
{
	int index = myControlMap.Find(aKey);
	if (index != myControlMap.FoundNone)
	{
		SComponentMessageData data;
		data.myPlayerControl = static_cast<ePlayerControls>(index);
		NotifyParent(eComponentMessageType::eKeyPressed, data);
	}
}

void CInputComponent::KeyReleased(const CU::eKeys aKey)
{
	int index = myControlMap.Find(aKey);
	if (index != myControlMap.FoundNone)
	{
		SComponentMessageData data;
		data.myPlayerControl = static_cast<ePlayerControls>(index);
		NotifyParent(eComponentMessageType::eKeyReleased, data);
	}
}

void CInputComponent::Update()
{
	if(myIsLeftMouseButtonPressed == true)
	{
		GetParent()->NotifyComponents(eComponentMessageType::eSetDirectionForShooting, SComponentMessageData());
	}
}