#include "stdafx.h"
#include "MovementComponent.h"
#include "PlayerControls.h"
#include "../CommonUtilities/JsonValue.h"
#include "../PostMaster/SendNetworkMessage.h"
#include "../TShared/NetworkMessage_Position.h"
#include "../Audio/AudioInterface.h"
#include "../BrontosaurusEngine/TextInstance.h"
#include "TumbleweedFactory.h"
#include "..\Game\PollingStation.h"

#define vodi void
static const float gravityAcceleration = 9.82f * 2.0f;
static const float timeUntilIdle = 1.0f;
static float timeUntilIdleThing = 2.0f;
static const float originalTimeUntilIdleThing = 3.0f;

CMovementComponent::CMovementComponent() : myJumpForce(0), myMovementMode(MovementMode::Default), myNoclipProssed(false), mySpeedMultiplier(1), myIncrementPressed(false), myDecrementPressed(false), myIsWalking(false)
{
	CU::CJsonValue playerControls;
	std::string errorMessage = playerControls.Parse("Json/Player/playerData.json");
	if (!errorMessage.empty())
	{
		DL_PRINT_WARNING("Could not load %s, using default values", errorMessage.c_str());

		myAcceleration = 200000.f;
		myDeceleration = 200000.f;
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
	myElapsedFallCheckTime = 0.0f;
	myFallCheckCooldown = 1.0f;

	myAudioId = Audio::CAudioInterface::GetInstance()->RegisterGameObject();

	myTextInstance = new CTextInstance();
	myTextInstance->Init();
	myTextInstance->SetPosition({ 0.5f, 0.2f });
	myIdleCountdown = timeUntilIdle;
	myIdleThingCountdown = originalTimeUntilIdleThing;

	myUseConstantVelosity = false;
	mykokoVelocity = 0.0f;
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
	myIdleCountdown -= aDeltaTime.GetSeconds();
	if (myIdleCountdown < 0.0f) 
	{
		myIdleThingCountdown -= aDeltaTime.GetSeconds();
		if (myIdleThingCountdown < 0.0f)
		{
			CU::Matrix44f tempMatrix = GetParent()->GetToWorldTransform();
			tempMatrix.Move(CU::Vector3f(0.0f, 0.0f, 20.0f));
			CU::Vector3f directionPosition = tempMatrix.GetPosition();
			tempMatrix.Move(CU::Vector3f(25.0f, 0.0f, 0.0f));
			CU::Vector3f spawnPosition = tempMatrix.GetPosition();
			CU::Vector3f direction = directionPosition - spawnPosition;
			direction.Normalize();
			spawnPosition.y += 2;

			CTumbleweedFactory::GetInstance()->CreateNewTumbleweed(spawnPosition, direction);

			timeUntilIdleThing *= 2.0f;
			myIdleThingCountdown = timeUntilIdleThing;
		}
	}
	if (myKeysDown.Any() == true) 
	{
		ResetIdle();
	}

#ifndef _RETAIL_BUILD
	SwapMovementMode();

	//Prints the player pos
	/*CU::Vector3f temp = GetParent()->GetWorldPosition();
	std::wstring message = L"Position ";
	message += L"X: ";
	message += std::to_wstring(temp.x);
	message += L"Y: ";
	message += std::to_wstring(temp.y);
	message += L"Z: ";
	message += std::to_wstring(temp.z);
	myTextInstance->SetText(message);
	myTextInstance->Render();*/


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

void CMovementComponent::SetIntroFallMode(const float aSpeed)
{
	mykokoVelocity = aSpeed;
	myUseConstantVelosity = true;
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
	if(IsWalking() == true)
	{
		const CU::Matrix44f transformation = GetParent()->GetToWorldTransform();
		Audio::CAudioInterface::GetInstance()->SetGameObjectPosition(myAudioId, transformation.GetPosition(), transformation.myForwardVector);

		Audio::CAudioInterface::GetInstance()->PostEvent("Player_Footstep", myAudioId);
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

	if ((myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_DOWN) == false)
	{
		if (myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_UP)
		{
			if(myJumpForce > 0.0f)
			{
				myJumpForce = 0.0f;
			}
		}
		myJumpForce -= gravityAcceleration * aDeltaTime.GetSeconds();
	}
	//if (GetParent()->GetLocalTransform().GetPosition().y < -100.0f)
	//{
	//	myJumpForce = 0.0f;
	//}
	if (myUseConstantVelosity == false)
	{
		myVelocity.y += myJumpForce;
	}
	else
	{
		myVelocity.y = mykokoVelocity;//-27.f;
		myIdleThingCountdown = 1.0f;
	}

	SComponentQuestionData groundeddata;
	if (GetParent()->AskComponents(eComponentQuestionType::ePhysicsControllerConstraints, groundeddata) == true)
	{
		myControllerConstraints = groundeddata.myChar;
		if (myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_DOWN)
		{
			if (myJumpForce < 0)
			{
				myLastGroundedPositionY = GetParent()->GetWorldPosition().y;
				myElapsedFallCheckTime = 0.0f;
				myCanDoubleJump = true;
				myJumpForce = 0.0f;
				if(myIsNotFalling == false)
					Audio::CAudioInterface::GetInstance()->PostEvent("Player_Land");
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

		int index = CPollingStation::GetInstance()->GetCurrentLevelIndex();
		parentTransform.SetPosition(data.myVector3f);

		bool killMe = false;
		float y = parentTransform.GetPosition().y;

		if (index == 3 || index == 5)
		{
			if (y < -1000.f)
			{
				killMe = true;
			}
		}
		else 
		{
			if (y < -100.f)
			{
				killMe = true;
			}
		}

		if (killMe)
		{
			SComponentMessageData takeDamageData;
			takeDamageData.myInt = 10000;
			GetParent()->NotifyComponents(eComponentMessageType::eTakeDamage, takeDamageData);
			myVelocity = CU::Vector3f::Zero;
		}

		NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
	}

	myElapsedFallCheckTime += aDeltaTime.GetSeconds();
	if (myFrameLastPositionY == GetParent()->GetWorldPosition().y)
	{
		myIsNotFalling = true;
	}
	else
	{
		if (myElapsedFallCheckTime >= myFallCheckCooldown)
		{
			myIsNotFalling = false;
		}
		else
		{
			if ((myLastGroundedPositionY - GetParent()->GetWorldPosition().y > -1.0f && (myLastGroundedPositionY - GetParent()->GetWorldPosition().y) < 0.0f))
			{
				myIsNotFalling = true;
			}
		}
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

	if (aPlayerControl == ePlayerControls::eJump )
	{
		float difference = myLastGroundedPositionY - GetParent()->GetWorldPosition().y;
		if (myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_DOWN || myIsNotFalling == true)
		{
			myElapsedFallCheckTime = 0.0f;
			if(myIsNotFalling == true)
			{
				myElapsedFallCheckTime = 10000.0f;
				myIsNotFalling = false;
			}
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

void CMovementComponent::ResetIdle()
{
	myIdleCountdown = timeUntilIdle;
	myIdleThingCountdown = originalTimeUntilIdleThing;
}