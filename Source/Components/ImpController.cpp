#include "stdafx.h"
#include "ImpController.h"
#include "EnemyBlueprint.h"
#include "../Physics/PhysicsCharacterController.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../TServer/ServerMessageManager.h"
#include "../TShared/NetworkMessage_AnimationStart.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"

static const float gravityAcceleration = 9.82f * 5;

CImpController::CImpController(unsigned int aId, eEnemyTypes aType)
	: CEnemy(aId, aType)
{
	myState = eImpState::eIdle;
	myJumpHeight = 0.0f;
	myWanderDistance = 5.0f;
	myIsJumping = false;
	myAttacksUntillRunningAway = 3;
	myUsedAttackSinceLastRunning = 0;
	myWanderDuration = 5.0f;
	myWanderAngle = 80;
	myElaspedWanderTime = 0.0f;
	myChargeRangedAttackDuration = 2.0f;
	myElapsedChargeMeleeAttackTime = 0.0f;
	myElapsedChargeAttackTime = 0.0f;
	myChargeMeleeAttackDuration = 1.0f;
	myJumpForce = 0.0f;
}

CImpController::~CImpController()
{
}

void CImpController::Update(const float aDeltaTime)
{
	UpdateBaseMemberVars(aDeltaTime);
	myVelocity.y = myJumpForce;
	SendTransformationToServer();
	UpdateJumpForces(aDeltaTime);
	float newPlayerDistance = CU::Vector3f(myToPlayer.x, 0.0f, myToPlayer.z).Length2();
	
	//if (myAudioID != 0)
	//{
	//	const CU::Matrix44f transform = GetParent()->GetToWorldTransform();
	//	Audio::CAudioInterface::GetInstance()->SetGameObjectPosition(myAudioID, transform.GetPosition(), {0.f,0.f,0.f});
	//}

	if(myIsDead == false && CanChangeState() == true)
	{
		if (WithinAttackRange() == true)
		{
			myState = eImpState::eChargingMeleeAttack;	
			LookAtPlayer();
			GetParent()->GetLocalTransform().Rotate(PI, CU::Axees::Y);
		}
		else if (WithinWalkToMeleeRange() == true)
		{
			myState = eImpState::eWalkIntoMeleeRange;
			if (ShouldJumpAfterPlayer() == true)
			{
				myState = eImpState::eJump;
			}

		}
		else if (WithinShootRange() == true)
		{
			myState = eImpState::eChargingRangedAttack;
			LookAtPlayer();
			GetParent()->GetLocalTransform().Rotate(PI, CU::Axees::Y);
			myIsAggressive = true;
		}
		else if (WithinDetectionRange() == true)
		{
			myState = eImpState::eChase;
			myIsAggressive = true;
		}
		else
		{
			myState = eImpState::eIdle;
			if(myIsAggressive == true)
			{
				myState = eImpState::eChase;
			}
		}
	}
	else
	{
		if (myIsDead == true)
		{
			myState = eImpState::eDead;
		//	Audio::CAudioInterface* audio = Audio::CAudioInterface::GetInstance();
			//audio->PostEvent("Imp_Death"/*, myAudioID*/);
		}
	}


	switch (myState)
	{
	case eImpState::eIdle:
		break;
	case eImpState::eWalkIntoMeleeRange:
	{
		LookAtPlayer();
		GetParent()->GetLocalTransform().Rotate(PI, CU::Axees::Y);
		myVelocity.z = -mySpeed;
		if (ShouldJumpAfterPlayer() == true)
		{
			myState = eImpState::eJump;
		}
		break;
	}
	case eImpState::eUseMeleeAttack:
		ChangeWeapon(0);
		if (GetParent()->AskComponents(eComponentQuestionType::eCanShoot, SComponentQuestionData()) == true)
		{
			Attack();
			//ChangeClientAnimation(eComponentMessageType::eImpMeleeAttack);
			myState = eImpState::eIdle;
			InitiateWander();
		}
		break;
	case eImpState::eUseRangedAttack:
		ChangeWeapon(1);
		if(GetParent()->AskComponents(eComponentQuestionType::eCanShoot, SComponentQuestionData()) == true)
		{
			Attack();
			ChangeClientAnimation(eComponentMessageType::eImpThrowAttack);
			myState = eImpState::eIdle;
			myUsedAttackSinceLastRunning++;
			if (myAttacksUntillRunningAway <= myUsedAttackSinceLastRunning)
			{
				InitiateWander();
			}
		}
		break;
	case eImpState::eJump:
		ApplyJumpForce(myJumpHeight);
		break;
	case eImpState::eChase:
	{
		LookAtPlayer();
		GetParent()->GetLocalTransform().Rotate(PI, CU::Axees::Y);
		myVelocity.z = -mySpeed;
		if (ShouldJumpAfterPlayer() == true)
		{
			myState = eImpState::eJump;
		}
	}
	break;
	case eImpState::eRunAfterShooting:
	{
		myWanderToPosition.y = GetParent()->GetLocalTransform().GetPosition().y;
		GetParent()->GetLocalTransform().LookAt(myWanderToPosition);
		GetParent()->GetLocalTransform().Rotate(PI, CU::Axees::Y);
		myVelocity.z = -mySpeed;
		CU::Vector3f distance = myWanderToPosition - GetParent()->GetLocalTransform().GetPosition();
		if(distance.Length() < 0.5f)
		{
			myState = eImpState::eIdle;
			myElaspedWanderTime = 0.0f;
			LookAtPlayer();
			GetParent()->GetLocalTransform().Rotate(PI, CU::Axees::Y);
		}
		myElaspedWanderTime += aDeltaTime;
		if(myElaspedWanderTime >= myWanderDuration)
		{
			myState = eImpState::eIdle;
			myElaspedWanderTime = 0.0f;
			LookAtPlayer();
			GetParent()->GetLocalTransform().Rotate(PI, CU::Axees::Y);
		}
		if(GetIfSidesAreColliding() == true)
		{
			if (myIsJumping == true)
			{
				InitiateWander();
			}
			ApplyJumpForce(myJumpHeight);
		}
		break;
	}
	case eImpState::eChargingRangedAttack:
	{
		myElapsedChargeAttackTime += aDeltaTime;
		if(myElapsedChargeAttackTime >= myChargeRangedAttackDuration)
		{
			myElapsedChargeAttackTime = 0.0f;
			myState = eImpState::eUseRangedAttack;
		}
		break;
	}
	case eImpState::eChargingMeleeAttack:
	{
		myElapsedChargeMeleeAttackTime += aDeltaTime;
		if (myElapsedChargeMeleeAttackTime >= myChargeMeleeAttackDuration)
		{
			myElapsedChargeMeleeAttackTime = 0.0f;
			myState = eImpState::eUseMeleeAttack;
		}
		break;
	}
	case eImpState::eDead:
		break;
	default:
		break;
	}

	CheckForNewTransformation(aDeltaTime);
}

void CImpController::UpdateJumpForces(const float aDeltaTime)
{
	if (CheckIfInAir() == false)
	{
		if (myJumpForce < 0)
		{
			myJumpForce = 0.0f;
			myIsJumping = false;
		}
	}
	myJumpForce -= gravityAcceleration * aDeltaTime;

	if (CheckIfInAir() == false)
	{
		myJumpForce = 0.0f;
		myIsJumping = false;
	}
}

void CImpController::SetEnemyData(const SEnemyBlueprint* aData)
{
	const SImpBlueprint* impData = static_cast<const SImpBlueprint*>(aData);
	myJumpHeight = impData->jumpHeight;
	myWanderAngle = impData->wanderAngle;
	myWanderDistance = impData->wanderDistance;
	myWanderDuration = impData->wanderDuration;
	myAttacksUntillRunningAway = impData->attacksUntillRunningAway;
	myChargeMeleeAttackDuration = impData->chargeMeleeAttackDuration;
	myChargeRangedAttackDuration = impData->chargeAttackDuration;
	CEnemy::SetEnemyData(aData);
}

void CImpController::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	CEnemy::Receive(aMessageType, aMessageData);
	switch (aMessageType)
	{
	case eComponentMessageType::eDied:
	{
		myState = eImpState::eDead;
		myIsDead = true;
		GetParent()->NotifyComponents(eComponentMessageType::eDeactivate, SComponentMessageData());
		if (myShouldNotReset == false)
		{
			CAddToCheckPointResetList* addToCheckPointMessage = new CAddToCheckPointResetList(GetParent());
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(addToCheckPointMessage);
			break;
		}
	}
	break;
	case eComponentMessageType::eCheckPointReset:
	{
		if (myShouldNotReset == false)
		{
		myState = eImpState::eIdle;
		myIsDead = false;
		SComponentMessageData visibilityData;
		visibilityData.myBool = true;
		GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
		GetParent()->NotifyComponents(eComponentMessageType::eActivate, SComponentMessageData());
		break;
		}
	}
	case eComponentMessageType::eNetworkDoDamage:
	{
		myIsAggressive = true;
		break;
	}
	case eComponentMessageType::eDeactivate:
		myIsDead = true;
		break;
	case eComponentMessageType::eActivate:
		myIsDead = false;
		break;
	}
}

void CImpController::ApplyJumpForce(float aJumpHeight)
{
	if(myIsJumping == false) 
	{
		myJumpForce = sqrtf((gravityAcceleration)* aJumpHeight * 2);
		myIsJumping = true;

		ChangeClientAnimation(eComponentMessageType::eImpStartToJump);
		//CNetworkMessage_AnimationStart* jumpMessage = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_AnimationStart>(ID_ALL);
		//jumpMessage->Init(GetNetworkID(), eComponentMessageType::eImpStartToJump);
		//Postmaster::Threaded::CPostmaster::GetInstance().BroadcastLocal(new CSendNetworkMessageMessage(jumpMessage));
	}
}

bool CImpController::CheckIfInAir()
{
	SComponentQuestionData groundeddata;
	if (GetParent()->AskComponents(eComponentQuestionType::ePhysicsControllerConstraints, groundeddata) == true)
	{
		myControllerConstraints = groundeddata.myChar;
		if (myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_DOWN)
		{
			return false;
		}
	}
	return true;
}

void CImpController::InitiateWander()
{
	//CU::Matrix44f impMatrix = GetParent()->GetLocalTransform(); //Change this later to something less taxing
	//if (myWanderAngle > 0)
	//{
	//	myUsedAttackSinceLastRunning = 0;
	//	myState = eImpState::eRunAfterShooting;
	//	float randomAngles = rand() % (myWanderAngle);
	//	randomAngles -= myWanderAngle * 0.5f;;
	//	float randomRadians = randomAngles * (PI / 180.0f);
	//	impMatrix.Rotate(randomRadians, CU::Axees::Y);
	//	impMatrix.Rotate(PI, CU::Axees::Y);
	//}
	//	impMatrix.Move(CU::Vector3f(0.0f, 0.0f, myWanderDistance));
	//	myWanderToPosition = impMatrix.GetPosition();
	myState = eImpState::eRunAfterShooting;
	myUsedAttackSinceLastRunning = 0;
	myWanderToPosition = GetNearestJumpPosition();
}

bool CImpController::CanChangeState()
{
	switch (myState)
	{
	case eImpState::eUseMeleeAttack:
		return false;
		break;
	case eImpState::eChargingMeleeAttack:
		return false;
		break;
	case eImpState::eChargingRangedAttack:
		return false;
		break;
	case eImpState::eUseRangedAttack:
		return false;
		break;
	case eImpState::eRunAfterShooting:
		return false;
		break;
	default:
		break;
	}
	return true;
}

eMessageReturn CImpController::DoEvent(const CResetToCheckPointMessage& aResetToCheckPointMessage)
{
	myJumpForce = 0.0f;
	myState = eImpState::eIdle;
	myIsJumping = false;
	return CEnemy::DoEvent(aResetToCheckPointMessage);
}

void CImpController::ChangeClientAnimation(const eComponentMessageType aMessageType) const
{
	CNetworkMessage_AnimationStart* animationMessage = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_AnimationStart>(ID_ALL);
	animationMessage->Init(GetNetworkID(), aMessageType);
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(animationMessage));
}
