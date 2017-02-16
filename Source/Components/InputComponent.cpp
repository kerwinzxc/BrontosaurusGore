#include "stdafx.h"
#include "InputComponent.h"
#include "..\CommonUtilities\InputMessenger.h"
#include "..\CommonUtilities\InputMessage.h"
#include "..\CommonUtilities\EMouseButtons.h"
#include "..\CommonUtilities\EKeyboardKeys.h"
#include "..\CommonUtilities\EInputReturn.h"


CInputComponent::CInputComponent()
{
	int br = 0;
	CU::CInputMessenger* inputMessenger = CU::CInputMessenger::GetInstance(eInputMessengerType::ePlayState);
	assert(inputMessenger != nullptr && "maybe create a init if input messenger (playstate?) is not created yet");

	if (inputMessenger)
	{
		inputMessenger->Subscribe(*this);
	}
}

CInputComponent::~CInputComponent()
{
	// if (CommonUtilities::InputWrapper::GetKeyDown(DIK_D) == true && myVelocity.x > -myMaxSpeed)
	// {
		// myVelocity.x += myAcceleration * aDeltaTime.GetSeconds();
	// }
	// else if (CommonUtilities::InputWrapper::GetKeyDown(DIK_A) == true && myVelocity.x < myMaxSpeed)
	// {
		// myVelocity.x -= myAcceleration * aDeltaTime.GetSeconds();
	// }
	// else
	// {
		// if (myVelocity.x > myDeceleration * aDeltaTime.GetSeconds())
		// {
			// myVelocity.x -= myDeceleration * aDeltaTime.GetSeconds();
		// }
		// else if (myVelocity.x < -myDeceleration * aDeltaTime.GetSeconds())
		// {
			// myVelocity.x += myDeceleration * aDeltaTime.GetSeconds();
		// }
		// else
		// {
			// myVelocity.x = 0;
		// }
	// }

	// if (CommonUtilities::InputWrapper::GetKeyDown(DIK_S) == true && myVelocity.y > -myMaxSpeed)
	// {
		// myVelocity.y += myAcceleration * aDeltaTime.GetSeconds();
	// }
	// else if (CommonUtilities::InputWrapper::GetKeyDown(DIK_W) == true && myVelocity.y < myMaxSpeed)
	// {
		// myVelocity.y -= myAcceleration * aDeltaTime.GetSeconds();
	// }
	// else
	// {
		// if (myVelocity.y > myDeceleration * aDeltaTime.GetSeconds())
		// {
			// myVelocity.y -= myDeceleration * aDeltaTime.GetSeconds();
		// }
		// else if (myVelocity.y < -myDeceleration * aDeltaTime.GetSeconds())
		// {
			// myVelocity.y += myDeceleration * aDeltaTime.GetSeconds();
		// }
		// else
		// {
			// myVelocity.y = 0;
		// }
	// }
}

void CInputComponent::Receive(const eComponentMessageType /*aMessageType*/, const SComponentMessageData& /*aMessageData*/)
{
}

CU::eInputReturn CInputComponent::TakeInput(const CU::SInputMessage& aInputMessage)
{
	switch (aInputMessage.myType)
	{
	case CU::eInputType::eMouseMoved:
		DL_PRINT("mouse delta: %f, %f", aInputMessage.myMouseDelta.x, aInputMessage.myMouseDelta.y);
		MouseMoved(aInputMessage.myMouseDelta);
		NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
		break;
	case CU::eInputType::eMousePressed:
		DL_PRINT("mouse pressed, pos: %f, %f, button: %s", aInputMessage.myMousePosition.x, aInputMessage.myMousePosition.y, (aInputMessage.myMouseButton == CU::eMouseButtons::LBUTTON ? "left" : "right"));
		aInputMessage.myMouseButton;
		aInputMessage.myMousePosition;
		break;
	case CU::eInputType::eMouseReleased:
		aInputMessage.myMouseButton;
		aInputMessage.myMousePosition;
		break;
	case CU::eInputType::eKeyboardPressed:
		aInputMessage.myKey;
		break;
	case CU::eInputType::eKeyboardReleased:
		aInputMessage.myKey;
		break;
	}

	return CU::eInputReturn::ePassOn;
}

void CInputComponent::MouseMoved(const CU::Vector2f aMouseDeltaNormalized)
{
	const float yaw = aMouseDeltaNormalized.x;
	const float pitch = aMouseDeltaNormalized.y;

	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();

	parentTransform.RotateAroundAxis(yaw, CU::Axees::Y);

	const float BegränsadVertikalVy = 0.999f;

	float lookAtHeight = parentTransform.myForwardVector.y;
	if (lookAtHeight < BegränsadVertikalVy && lookAtHeight > -BegränsadVertikalVy)
	{
		parentTransform.Rotate(pitch, CU::Axees::X);
	}
	else if (lookAtHeight > BegränsadVertikalVy && pitch > 0.f)
	{
		parentTransform.Rotate(pitch, CU::Axees::X);
	}
	else if (lookAtHeight < -BegränsadVertikalVy && pitch < 0.f)
	{
		parentTransform.Rotate(pitch, CU::Axees::X);
	}
}
