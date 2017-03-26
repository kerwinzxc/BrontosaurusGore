#include "stdafx.h"
#include "RevenantController.h"
#include "../Physics/PhysicsCharacterController.h"

static const float gravityAcceleration = 9.82f * 2.0f;
CRevenantController::CRevenantController(unsigned int aId, eEnemyTypes aType)
	:CEnemy(aId, aType)
{
	myFlightForce = 0.0f;
	myIsflying = false;
}


CRevenantController::~CRevenantController()
{
}

void CRevenantController::SetEnemyData(const SEnemyBlueprint* aData)
{
	const SRevenantBlueprint* revenantData = static_cast<const SRevenantBlueprint*>(aData);
	myHoverTime = revenantData->hoverTime;
	myFlightHeight = revenantData->flightHeight;
	CEnemy::SetEnemyData(aData);
}


void CRevenantController::Update(const float aDeltaTime)
{
	CU::Vector3f velocity;
	velocity.y = myFlightForce;
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
			if (myFlightForce < 0)
			{
				myFlightForce = 0.0f;
			}
		}
	}


	if (myIsDead == false && myIsflying == false)
	{
		if (myStartAttackRange2 > distToPlayer)
		{
			myState = eRevenantState::eUseMeleeAttack;
		}
		else if (myShouldGoMeleeRadius2 > distToPlayer)
		{
			myState = eRevenantState::eWalkIntoMeleeRange;
		}
		else if (myDetectionRange2 > distToPlayer)
		{
			myState = eRevenantState::eUseRangedAttack;
			if (toPlayer.y > 2.0f)
			{
				myState = eRevenantState::eFlyAscend;
				ApplyFlyForce(myFlightHeight);
			}
		}
		else
		{
			myState = eRevenantState::eIdle;
		}
	}
	switch (myState)
	{
	case eRevenantState::eIdle:
	{
		break;
	}
	case eRevenantState::eWalkIntoMeleeRange:
	{
		LookAtPlayer(); //impl. turn rate?
		velocity.z = mySpeed;
	}
	break;
	case eRevenantState::eUseMeleeAttack:
		ChangeWeapon(3);
		Attack();
		break;
	case eRevenantState::eUseRangedAttack:
		ChangeWeapon(0);
		Attack();
		break;
	case eRevenantState::eFlyAscend:
	{
		myFlightForce -= gravityAcceleration * aDeltaTime;
		if(myFlightForce <= 0.0f)
		{
			myState = eRevenantState::eFlyHover;
			myElapsedHoverTime = 0.0f;
		}
		break;
	}
	case eRevenantState::eFlyHover:
	{
		myElapsedHoverTime += aDeltaTime;
		LookAtPlayer();
		ChangeWeapon(2);
		Attack();
		if(myElapsedHoverTime >= myHoverTime)
		{
			myElapsedHoverTime = 0.0f;
			myState = eRevenantState::eFlyDescend;
		}
		break;
	}
	case eRevenantState::eFlyDescend:
	{
		myFlightForce -= gravityAcceleration * aDeltaTime;
		if(CheckIfInAir() == false)
		{
			myIsflying = false;
			DL_PRINT("Stopping flight!!!");
			myFlightForce = 0.0f;
		}
		break;
	}
	case eRevenantState::eDead:
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

void CRevenantController::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
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

void  CRevenantController::ApplyFlyForce(float aJumpHeight)
{
	myFlightForce = sqrtf((gravityAcceleration)* aJumpHeight * 2);
	myIsflying = true;

	SComponentQuestionData groundeddata;
	if (GetParent()->AskComponents(eComponentQuestionType::ePhysicsControllerConstraints, groundeddata) == true)
	{
		myControllerConstraints = groundeddata.myChar;
		if (myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_DOWN)
		{
		}
		else
		{
			myFlightForce = sqrtf((gravityAcceleration)* aJumpHeight * 2);

		}
	}
}

bool  CRevenantController::CheckIfInAir()
{
	if (myFlightForce <= -sqrtf((gravityAcceleration)* myFlightHeight * 2))
	{
		return false;
	}
	return true;
}