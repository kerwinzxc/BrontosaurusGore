#include "stdafx.h"
#include "ImpController.h"
#include "EnemyBlueprint.h"
#include "../Physics/PhysicsCharacterController.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"
#include "../ThreadedPostmaster/Postmaster.h"

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
		}
		else if (WithinDetectionRange() == true)
		{
			myState = eImpState::eChase;
		}
		else
		{
			myState = eImpState::eIdle;
		}
	}
	else
	{
		if (myIsDead == true)
		{
			myState = eImpState::eDead;
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
			myState = eImpState::eIdle;
			InitiateWander();
		}
		break;
	case eImpState::eUseRangedAttack:
		ChangeWeapon(1);
		if(GetParent()->AskComponents(eComponentQuestionType::eCanShoot, SComponentQuestionData()) == true)
		{
			Attack();
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
		myVelocity.z = -mySpeed;
		CU::Vector3f distance = myWanderToPosition - GetParent()->GetWorldPosition();
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
	CU::Matrix44f& transform = GetParent()->GetLocalTransform();
	CU::Matrix44f rotation = transform.GetRotation();
	rotation.myForwardVector.y = 0.f;


	SComponentQuestionData data;
	data.myVector4f = myVelocity * rotation * aDeltaTime;
	data.myVector4f.w = aDeltaTime;
	if (GetParent()->AskComponents(eComponentQuestionType::eMovePhysicsController, data) == true)
	{
		transform.SetPosition(data.myVector3f);
		NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
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
	case eComponentMessageType::eOnCollisionEnter:
	{
		switch (myState)
		{
		case eImpState::eWalkIntoMeleeRange:
			ApplyJumpForce(myJumpHeight);
			break;
		case eImpState::eRunAfterShooting:
			InitiateWander();
			break;
		default:
			break;
		}
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
