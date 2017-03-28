#include "stdafx.h"
#include "MovementComponent.h"
#include "PlayerControls.h"
#include "../CommonUtilities/JsonValue.h"
#include "../PostMaster/SendNetworkMessage.h"
#include "../TShared/NetworkMessage_Position.h"
#include "../Audio/AudioInterface.h"

#define vodi void
static const float gravityAcceleration = 9.82f * 2.0f;
CMovementComponent::CMovementComponent() : myJumpForce(0), myMovementMode(MovementMode::Default), myNoclipProssed(false), mySpeedMultiplier(1), myIncrementPressed(false), myDecrementPressed(false), myIsWalking(false)
{
	CU::CJsonValue playerControls;
	std::string errorMessage = playerControls.Parse("Json/Player/playerData.json");
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
	myFrameLastPositionY = -110000000.0f;
	myIsNotFalling = false;

	myAudioId = Audio::CAudioInterface::GetInstance()->RegisterGameObject();
}

CMovementComponent::~CMovementComponent()
{
}

vodi CMovementComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eCheckPointReset:
		myCanDoubleJump = true;
		myJumpForce = 0.0f;
		myVelocity = CU::Vector3f::Zero;
		break;
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
#ifndef _RETAIL_BUILD
	SwapMovementMode();
	switch(myMovementMode)
	{
	case MovementMode::Default: 
		DefaultMovement(aDeltaTime);
		break;
	case MovementMode::Freecam: 
		FreecamMovement(aDeltaTime);
		break;
	default: break;
	}
#else
	DefaultMovement(aDeltaTime);
#endif

	
}

void CMovementComponent::SwapMovementMode()
{
	if(myKeysDown[static_cast<int>(ePlayerControls::eNoclip)] == true && myNoclipProssed == false)
	{
		myNoclipProssed = true;

		myMovementMode = static_cast<MovementMode>((1 + static_cast<int>(myMovementMode)
		)
		% static_cast<int>(MovementMode::Size));
	}
	else if(myKeysDown[static_cast<int>(ePlayerControls::eNoclip)] == false && myNoclipProssed == true)
	{
		myNoclipProssed = false;
	}
}

bool CMovementComponent::IsWalking() const
{
	return myKeysDown[static_cast<int>(ePlayerControls::eRight) ] && !myKeysDown[static_cast<int>(ePlayerControls::eLeft)] ||
		myKeysDown[static_cast<int>(ePlayerControls::eLeft)] && !myKeysDown[static_cast<int>(ePlayerControls::eRight)] ||
		myKeysDown[static_cast<int>(ePlayerControls::eForward)] && !myKeysDown[static_cast<int>(ePlayerControls::eBackward)] ||
		myKeysDown[static_cast<int>(ePlayerControls::eBackward)] && !myKeysDown[static_cast<int>(ePlayerControls::eForward)];
}

void CMovementComponent::UpdateSoundState()
{
	if(myIsWalking == false && IsWalking() == true)
	{
		myIsWalking = true;
		Audio::CAudioInterface::GetInstance()->PostEvent("Player_Footstep", myAudioId);
	}
	else if(myIsWalking == true && IsWalking() == false)
	{
		myIsWalking = false;
		Audio::CAudioInterface::GetInstance()->PostEvent("Player_Footstep_Stop", myAudioId);
	}

	if (myIsWalking == true && myJumpForce != 0.0f)
	{
		myIsWalking = false;
		Audio::CAudioInterface::GetInstance()->PostEvent("Player_Footstep_Stop", myAudioId);
	}

	if(myIsWalking == true)
	{
		const CU::Matrix44f transformation = GetParent()->GetToWorldTransform();
		Audio::CAudioInterface::GetInstance()->SetGameObjectPosition(myAudioId, transformation.GetPosition(), transformation.myForwardVector);
	}
}

void CMovementComponent::DefaultMovement(const CU::Time& aDeltaTime)
{
	UpdateSoundState();

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
	if (GetParent()->GetLocalTransform().GetPosition().y < -100.0f)
	{
		myJumpForce = 0.0f;
	}
	myVelocity.y += myJumpForce;

	SComponentQuestionData groundeddata;
	if (GetParent()->AskComponents(eComponentQuestionType::ePhysicsControllerConstraints, groundeddata) == true)
	{
		myControllerConstraints = groundeddata.myChar;
		if (myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_DOWN)
		{
			if (myJumpForce < 0)
			{
				myCanDoubleJump = true;
				myJumpForce = 0.0f;
			}
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
		parentTransform.SetPosition(data.myVector3f);
		if (parentTransform.GetPosition().y < -100.0f)
		{
			SComponentMessageData takeDamageData;
			takeDamageData.myInt = 10000;
			GetParent()->NotifyComponents(eComponentMessageType::eTakeDamage, takeDamageData);
			myVelocity = CU::Vector3f::Zero;
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

	if (myFrameLastPositionY == GetParent()->GetWorldPosition().y)
	{
		myIsNotFalling = true;
	}
	else
	{
		myIsNotFalling = false;
	}

	myFrameLastPositionY = GetParent()->GetWorldPosition().y;
}

void CMovementComponent::FreecamMovement(const CU::Time& aDeltaTime)
{
	CU::Vector3f velocity;

	if (myKeysDown[static_cast<int>(ePlayerControls::eForward)])
	{
		velocity += CU::Vector3f(0,0,1);
	}

	if(myKeysDown[static_cast<int>(ePlayerControls::eBackward)])
	{
		velocity -= CU::Vector3f(0, 0, 1);
	}

	if (myKeysDown[static_cast<int>(ePlayerControls::eRight)])
	{
		velocity += CU::Vector3f(1, 0, 0);
	}

	if (myKeysDown[static_cast<int>(ePlayerControls::eLeft)])
	{
		velocity -= CU::Vector3f(1, 0, 0);
	}

	if (myKeysDown[static_cast<int>(ePlayerControls::eJump)])
	{
		velocity += CU::Vector3f(0, 1, 0);
	}
	if (myKeysDown[static_cast<int>(ePlayerControls::eDown)])
	{
		velocity -= CU::Vector3f(0, 1, 0);
	}
	if (myKeysDown[static_cast<int>(ePlayerControls::eIncreaseSpeed)] && myIncrementPressed == false)
	{
		myIncrementPressed = true;
		mySpeedMultiplier += 1;
		CLAMP(mySpeedMultiplier, 1, 10);
	}
	else if(myKeysDown[static_cast<int>(ePlayerControls::eIncreaseSpeed)] == false && myIncrementPressed == true)
	{
		myIncrementPressed = false;
	}
	if (myKeysDown[static_cast<int>(ePlayerControls::eDecreseSpeed)] && myDecrementPressed == false)
	{
		myDecrementPressed = true;
		mySpeedMultiplier -= 1;

		CLAMP(mySpeedMultiplier, 1, 10);
	}
	else if(myKeysDown[static_cast<int>(ePlayerControls::eDecreseSpeed)] == false && myDecrementPressed == true)
	{
		myDecrementPressed = false;
	}

	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
	CU::Matrix44f rotation = parentTransform.GetRotation();
	rotation.myForwardVector.y = 0.f;

	SComponentQuestionData data;
	data.myVector4f = velocity.Normalize() * rotation * aDeltaTime.GetSeconds() * myMaxSpeed * mySpeedMultiplier;
	data.myVector4f.w = aDeltaTime.GetSeconds();

	if (GetParent()->AskComponents(eComponentQuestionType::eMovePhysicsController, data) == true)
	{
		CU::Vector3f position = parentTransform.GetPosition();
		parentTransform.SetPosition(data.myVector3f);
	}

	NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
}

void CMovementComponent::KeyPressed(const ePlayerControls aPlayerControl)
{
	myKeysDown[static_cast<int>(aPlayerControl)] = true;

	if (myKeysDown[static_cast<int>(ePlayerControls::eJump)] == true )
	{
		if (myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_DOWN || myIsNotFalling == true)
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
	myJumpForce = sqrtf((gravityAcceleration) * aJumpHeight * 2);
}