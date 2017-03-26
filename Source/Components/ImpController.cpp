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

	SComponentQuestionData groundeddata;
	if (GetParent()->AskComponents(eComponentQuestionType::ePhysicsControllerConstraints, groundeddata) == true)
	{
		myControllerConstraints = groundeddata.myChar;
		if (myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_DOWN)
		{
			if (myJumpForce < 0)
			{
				myJumpForce = 0.0f;
			}
		}
	}
	if(myIsJumping == true)
	{
		myJumpForce -= gravityAcceleration * aDeltaTime;
		if (CheckIfInAir() == false)
		{
			DL_PRINT("stopping jump");
			myIsJumping = false;
			myJumpForce = 0.0f;
		}
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
		CU::Vector3f lookatPosition = closestPlayerPos;
		lookatPosition.y = GetParent()->GetLocalTransform().GetPosition().y;
		GetParent()->LookAt(lookatPosition); //impl. turn rate?
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
	rotation.myForwardVector.y = 0.f;

	SComponentQuestionData data;
	data.myVector4f = velocity * rotation * aDeltaTime;
	data.myVector4f.w = aDeltaTime;

	if (GetParent()->AskComponents(eComponentQuestionType::eMovePhysicsController, data) == true)
	{
		//parentTransform.SetPosition(data.myVector3f);
		NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
	}
	GetParent()->Move(velocity * aDeltaTime);  // Remove this when character cotroll
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
	myJumpForce = sqrtf((gravityAcceleration)* aJumpHeight * 2);
	myIsJumping = true;
	DL_PRINT("Jump!! %f", myJumpForce);

	SComponentQuestionData groundeddata;
	if (GetParent()->AskComponents(eComponentQuestionType::ePhysicsControllerConstraints, groundeddata) == true)
	{
		myControllerConstraints = groundeddata.myChar;
		if (myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_DOWN)
		{
		}
		else
		{
			myJumpForce = sqrtf((gravityAcceleration)* aJumpHeight * 2);
			
		}
	}
}

bool  CImpController::CheckIfInAir()
{
	if (myJumpForce <= -sqrtf((gravityAcceleration)* myJumpHeight * 2))
	{
		return false;
	}
	return true;
}