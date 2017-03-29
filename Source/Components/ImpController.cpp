#include "stdafx.h"
#include "ImpController.h"
#include "EnemyBlueprint.h"
#include "../Physics/PhysicsCharacterController.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"
#include "../ThreadedPostmaster/Postmaster.h"

static const float gravityAcceleration = 9.82f * 4;

CImpController::CImpController(unsigned int aId, eEnemyTypes aType)
	: CEnemy(aId, aType)
{
	myState = eImpState::eIdle;
	myJumpHeight = 0.0f;
	myIsJumping = false;
}

CImpController::~CImpController()
{
}

void CImpController::Update(const float aDeltaTime)
{
	UpdateBaseMemberVars(aDeltaTime);
	myVelocity.y = myJumpForce;
	UpdateTransformationNetworked();
	UpdateJumpForces(aDeltaTime);

	if(myIsDead == false)
	{
		if (WithinAttackRange())
		{
			myState = eImpState::eUseMeleeAttack;	
		}
		else if (WithinWalkToMeleeRange())
		{
			myState = eImpState::eWalkIntoMeleeRange;
		
			if (ShouldJumpAfterPlayer())
				myState = eImpState::eJump;

		}
		else if (WithinDetectionRange())
		{
			myState = eImpState::eUseRangedAttack;
		}
		else
		{
			myState = eImpState::eIdle;
		}
	}


	switch (myState)
	{
	case eImpState::eIdle:
		break;
	case eImpState::eWalkIntoMeleeRange:
	{
		LookAtPlayer();
		myVelocity.z = mySpeed;
		break;
	}
	case eImpState::eUseMeleeAttack:
		ChangeWeapon(0);
		Attack();
		break;
	case eImpState::eUseRangedAttack:
		ChangeWeapon(1);
		Attack();
		break;
	case eImpState::eJump:
		ApplyJumpForce(myJumpHeight);
		break;
	case eImpState::eDead:
		break;
	default:
		break;
	}

	UpdateTransformationLocal(aDeltaTime);
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
		CAddToCheckPointResetList* addToCheckPointMessage = new CAddToCheckPointResetList(GetParent());
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(addToCheckPointMessage);
		break;
	}
	case eComponentMessageType::eCheckPointReset:
		myState = eImpState::eIdle;
		myIsDead = false;
		SComponentMessageData visibilityData;
		visibilityData.myBool = true;
		GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
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

bool  CImpController::CheckIfInAir()
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
