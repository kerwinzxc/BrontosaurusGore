#include "stdafx.h"
#include "ImpController.h"
#include "EnemyBlueprint.h"
#include "../Physics/PhysicsCharacterController.h"

static const float gravityAcceleration = 9.82f * 2.0f;
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
	CU::Vector3f velocity;
	velocity.y = myJumpForce;
	myElapsedWaitingToSendMessageTime += aDeltaTime;
	const CU::Vector3f closestPlayerPos = ClosestPlayerPosition();
	const CU::Vector3f myPos = GetParent()->GetWorldPosition();
	const CU::Vector3f toPlayer = closestPlayerPos - myPos;
	const float distToPlayer = toPlayer.Length2();
	UpdateTransformation();

	if(CheckIfInAir() == false)
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
		DL_PRINT("stopping jump");
		myIsJumping = false;
		myJumpForce = 0.0f;
	}
	


	if(myIsDead == false)
	{
		if (myStartAttackRange2 > distToPlayer)
		{
			myState = eImpState::eUseMeleeAttack;
		
		}
		else if (myShouldGoMeleeRadius2 > distToPlayer)
		{
			myState = eImpState::eWalkIntoMeleeRange;
		
			if(toPlayer.y > 1.0f && myIsJumping == false)
			{	
				myState = eImpState::eJump;
			}
		}
		else if (myDetectionRange2 > distToPlayer)
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
	{
		break;
	}
	case eImpState::eWalkIntoMeleeRange:
	{
		LookAtPlayer();
		velocity.z = mySpeed;
	}
		break;
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
	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
	CU::Matrix44f rotation = parentTransform.GetRotation();

	SComponentQuestionData data;
	data.myVector4f = velocity * rotation * aDeltaTime;
	data.myVector4f.w = aDeltaTime;
	if (GetParent()->AskComponents(eComponentQuestionType::eMovePhysicsController, data) == true)
	{
		parentTransform.SetPosition(data.myVector3f);
		NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
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
		myIsDead = true;
		break;
	}
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
			DL_PRINT("Not in air !!!");
			return false;
		}
	}
	return true;
}