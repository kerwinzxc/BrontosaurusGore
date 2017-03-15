#include "stdafx.h"
#include "MovementComponent.h"
#include "PlayerControls.h"
#include "../CommonUtilities/JsonValue.h"
#include "../PostMaster/SendNetworkMessage.h"
#include "../TShared/NetworkMessage_Position.h"

#define vodi void
static const float gravityAcceleration = 9.82f * 2.0f;
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

	myControllerConstraints = Physics::EControllerConstraintsFlag::eCOLLISION_DOWN;
	myCanDoubleJump = true;

	myAcceleration = playerControls["Acceleration"].GetFloat();
	myDeceleration = playerControls["Deceleration"].GetFloat();
	myMaxSpeed = playerControls["MaxSpeed"].GetFloat();
	myJumpHeight = playerControls["JumpHeight"].GetFloat();
	myDoubleJumpHeight = playerControls["SecondJumpHeight"].GetFloat();
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
	case eComponentMessageType::eStopJumping:
		break;
	}
}

void CMovementComponent::Update(const CU::Time aDeltaTime)
{
	myVelocity.y = 0.0f;
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

	if ((myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_DOWN) == 0)
	{
		if (myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_UP)
		{
			myJumpForce = 0.0f;
		}
		myJumpForce -= gravityAcceleration * aDeltaTime.GetSeconds();
	}
	myVelocity.y += myJumpForce;

	SComponentQuestionData groundeddata;
	if (GetParent()->AskComponents(eComponentQuestionType::ePhysicsControllerConstraints, groundeddata) == true)
	{
		myControllerConstraints = groundeddata.myChar;
		if (myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_DOWN)
		{
			myCanDoubleJump = true;
			myJumpForce = 0.0f;
		}
	}


	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
	CU::Matrix44f rotation = parentTransform.GetRotation();
	rotation.myForwardVector.y = 0.f;

	SComponentQuestionData data;
	data.myVector4f = myVelocity * rotation * aDeltaTime.GetSeconds();
	data.myVector4f.w = aDeltaTime.GetSeconds();

	if (GetParent()->AskComponents(eComponentQuestionType::eMovePhysicsController, data) == true)
	{
		CU::Vector3f position = parentTransform.GetPosition();
		parentTransform.SetPosition(data.myVector3f);
		if (parentTransform.GetPosition().y < -100.0f)
		{
			SComponentMessageData takeDamageData;
			takeDamageData.myInt = 10000;
			GetParent()->NotifyComponents(eComponentMessageType::eTakeDamage, takeDamageData);

			//Teleport stuff back code
			//CU::Vector3f teleportPosition(parentTransform.GetPosition().x, parentTransform.GetPosition().y * -1, parentTransform.GetPosition().z);
			////parentTransform.SetPosition(parentTransform.GetPosition().x, parentTransform.GetPosition().y * -1, parentTransform.GetPosition().z);
			//SComponentQuestionData data;
			//data.myVector4f = (teleportPosition - parentTransform.GetPosition()) ;
			//data.myVector4f.w = aDeltaTime.GetSeconds();
			//if (GetParent()->AskComponents(eComponentQuestionType::eMovePhysicsController, data) == true)
			//{
			//	parentTransform.SetPosition(data.myVector3f);
			//}
		}
		NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
	}

	
}

void CMovementComponent::KeyPressed(const ePlayerControls aPlayerControl)
{
	myKeysDown[static_cast<int>(aPlayerControl)] = true;

	if (myKeysDown[static_cast<int>(ePlayerControls::eJump)] == true )
	{
		if (myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_DOWN)
		{
			ApplyJumpForce(myJumpHeight);
		}
		else
		{
			if (myCanDoubleJump == true)
			{
				ApplyJumpForce(myDoubleJumpHeight);
				myCanDoubleJump = false;
			}
		}
	
	}
}

void CMovementComponent::KeyReleased(const ePlayerControls aPlayerControl)
{
	myKeysDown[static_cast<int>(aPlayerControl)] = false;
}

void CMovementComponent::ApplyJumpForce(float aJumpHeight)
{
	myJumpForce = sqrtf(gravityAcceleration * aJumpHeight * 2);
}