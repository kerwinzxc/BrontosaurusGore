#include "stdafx.h"
#include "MovementComponent.h"
#include "PlayerControls.h"

#define vodi void

CMovementComponent::CMovementComponent()
{
	myAcceleration = 50000.f;
	myDeceleration = 40000.f;
	myMaxSpeed = 1000.f;
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

	if (myKeysDown[static_cast<int>(ePlayerControls::eBackward)] == true && myVelocity.z > -myMaxSpeed)
	{
		myVelocity.z -= myAcceleration * aDeltaTime.GetSeconds();
	}
	else if (myKeysDown[static_cast<int>(ePlayerControls::eForward)] == true && myVelocity.z < myMaxSpeed)
	{
		myVelocity.z += myAcceleration * aDeltaTime.GetSeconds();
	}
	else
	{
		//if (myVelocity.z > myDeceleration * aDeltaTime.GetSeconds())
		//{
		//	myVelocity.z -= myDeceleration * aDeltaTime.GetSeconds();
		//}
		//else if (myVelocity.z < -myDeceleration * aDeltaTime.GetSeconds())
		//{
		//	myVelocity.z += myDeceleration * aDeltaTime.GetSeconds();
		//}
		//else
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
	DL_PRINT("%f, %f, %f", myVelocity.x, myVelocity.y, myVelocity.z);


	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
	
	CU::Matrix44f rotation = parentTransform.GetRotation();
	rotation.myForwardVector.y = 0.f;

	CU::Vector3f position = parentTransform.GetPosition();

	parentTransform.SetPosition(myVelocity * rotation * aDeltaTime.GetSeconds() + position);
	NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
	//GetParent()->GetLocalTransform().Move(myVelocity);
}

void CMovementComponent::KeyPressed(const ePlayerControls aPlayerControl)
{
	myKeysDown[static_cast<int>(aPlayerControl)] = true;
}

void CMovementComponent::KeyReleased(const ePlayerControls aPlayerControl)
{
	myKeysDown[static_cast<int>(aPlayerControl)] = false;
}
