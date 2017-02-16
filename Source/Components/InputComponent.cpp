#include "stdafx.h"
#include "InputComponent.h"


CInputComponent::CInputComponent()
{
	
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
