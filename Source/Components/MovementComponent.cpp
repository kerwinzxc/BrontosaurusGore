#include "stdafx.h"
#include "MovementComponent.h"
#include "PlayerControls.h"
#include "../CommonUtilities/JsonValue.h"

#define vodi void

CMovementComponent::CMovementComponent()
{
	CU::CJsonValue playerControls;
	std::string errorMessage = playerControls.Parse("Json/Player/Controls.json");
	if (!errorMessage.empty())
	{
		DL_PRINT_WARNING("Could not load %s, using default values", errorMessage.c_str());

		myAcceleration = 20000.f;
		myDeceleration = 20000.f;
		myMaxSpeed = 1500.f;
		return;
	}

	myAcceleration = playerControls["Acceleration"].GetFloat();
	myDeceleration = playerControls["Deceleration"].GetFloat();
	myMaxSpeed = playerControls["MaxSpeed"].GetFloat();
}

CMovementComponent::~CMovementComponent()
{
}

vodi CMovementComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eKeyPressed:
		KeyPressed(aMessageData.myPlayerControl);
		break;
	case eComponentMessageType::eKeyReleased:
		KeyReleased(aMessageData.myPlayerControl);
		break;
	}
}

void CMovementComponent::Update(const CU::Time aDeltaTime)
{

	if (myKeysDown[static_cast<int>(ePlayerControls::eRight)] == true && myVelocity.x > -myMaxSpeed)
	{
		myVelocity.x += myAcceleration * aDeltaTime.GetSeconds();
	}
	else if (myKeysDown[static_cast<int>(ePlayerControls::eLeft)] == true && myVelocity.x < myMaxSpeed)
	{
		myVelocity.x -= myAcceleration * aDeltaTime.GetSeconds();
	}
	else
	{
		if (myVelocity.x > myDeceleration * aDeltaTime.GetSeconds())
		{
			myVelocity.x -= myDeceleration * aDeltaTime.GetSeconds();
		}
		else if (myVelocity.x < -myDeceleration * aDeltaTime.GetSeconds())
		{
			myVelocity.x += myDeceleration * aDeltaTime.GetSeconds();
		}
		else
		{
			myVelocity.x = 0;
		}
	}

	if (myKeysDown[static_cast<int>(ePlayerControls::eBackward)] == true && myVelocity.z >= -myMaxSpeed)
	{
		myVelocity.z -= myAcceleration * aDeltaTime.GetSeconds();
	}
	else if (myKeysDown[static_cast<int>(ePlayerControls::eForward)] == true && myVelocity.z <= myMaxSpeed)
	{
		myVelocity.z += myAcceleration * aDeltaTime.GetSeconds();
	}
	else
	{
		if (myVelocity.z > myDeceleration * aDeltaTime.GetSeconds())
		{
			myVelocity.z -= myDeceleration * aDeltaTime.GetSeconds();
		}
		else if (myVelocity.z < -myDeceleration * aDeltaTime.GetSeconds())
		{
			myVelocity.z += myDeceleration * aDeltaTime.GetSeconds();
		}
		else
		{
			myVelocity.z = 0;
		}
	}

	float speed = myVelocity.Length2();
	if (speed > myMaxSpeed * myMaxSpeed)
	{
		myVelocity /= sqrtf(speed);
		myVelocity *= myMaxSpeed;
	}

	myVelocity.y = 0.f;

	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
	
	CU::Matrix44f rotation = parentTransform.GetRotation();
	rotation.myForwardVector.y = 0.f;

	CU::Vector3f position = parentTransform.GetPosition();

	parentTransform.SetPosition(myVelocity * rotation * aDeltaTime.GetSeconds() + position);
	NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
}

void CMovementComponent::KeyPressed(const ePlayerControls aPlayerControl)
{
	myKeysDown[static_cast<int>(aPlayerControl)] = true;
}

void CMovementComponent::KeyReleased(const ePlayerControls aPlayerControl)
{
	myKeysDown[static_cast<int>(aPlayerControl)] = false;
}
