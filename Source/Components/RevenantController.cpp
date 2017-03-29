#include "stdafx.h"
#include "RevenantController.h"
#include "../Physics/PhysicsCharacterController.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"
#include "../ThreadedPostmaster/Postmaster.h"

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
	UpdateBaseMemberVars(aDeltaTime);
	myVelocity.y = myFlightForce;
	SendTransformationToServer();
	UpdateFlightForces(aDeltaTime);

	if (myIsDead == false && myIsflying == false)
	{
		if (WithinAttackRange())
		{
			myState = eRevenantState::eUseMeleeAttack;
		}
		else if (WithinWalkToMeleeRange())
		{
			myState = eRevenantState::eWalkIntoMeleeRange;
		}
		else if (WithinDetectionRange())
		{
			myState = eRevenantState::eUseRangedAttack;
			if (myToPlayer.y > 2.0f)
			{
				myState = eRevenantState::eFlyAscend;
				ApplyFlightForce();
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
		break;
	case eRevenantState::eWalkIntoMeleeRange:
		LookAtPlayer();
		myVelocity.z = mySpeed;
	break;
	case eRevenantState::eUseMeleeAttack:
		ChangeWeapon(2);
		Attack();
		break;
	case eRevenantState::eUseRangedAttack:
		ChangeWeapon(0);
		Attack();
		break;
	case eRevenantState::eFlyAscend:
	{
		LookAtPlayer();
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
		LookAtPlayer();
		ChangeWeapon(1);
		Attack();

		myElapsedHoverTime += aDeltaTime;
		if(myElapsedHoverTime >= myHoverTime)
		{
			myElapsedHoverTime = 0.0f;
			myState = eRevenantState::eFlyDescend;
		}
		break;
	}
	case eRevenantState::eFlyDescend:
	{
		LookAtPlayer();

		myFlightForce -= gravityAcceleration * aDeltaTime;
		if(CheckIfInAir() == false)
		{
			myIsflying = false;
			myFlightForce = 0.0f;
		}
		break;
	}
	case eRevenantState::eDead:
		break;
	default:
		break;
	}
	CheckForNewTransformation(aDeltaTime);
}

void CRevenantController::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eDied:
	{
		myIsDead = true;
		myState = eRevenantState::eDead;
		CAddToCheckPointResetList* addToCheckPointMessage = new CAddToCheckPointResetList(GetParent());
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(addToCheckPointMessage);
		break;
	}
	case eComponentMessageType::eCheckPointReset:
		myIsDead = false;
		SComponentMessageData visibilityData;
		visibilityData.myBool = true;
		GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
		break;
	}
}

void  CRevenantController::ApplyFlightForce()
{
	if(CheckIfInAir() == false)
	{
		myIsflying = true;
		myFlightForce = sqrtf((gravityAcceleration)* myFlightHeight * 2);
	}
}

void CRevenantController::UpdateFlightForces(const float aDeltaTime)
{
	if (myIsflying == false)
	{
		myFlightForce -= gravityAcceleration * aDeltaTime;
	}

	if (CheckIfInAir() == false)
	{
		if (myFlightForce < 0)
		{
			myFlightForce = 0.0f;
		}
	}
}

bool  CRevenantController::CheckIfInAir()
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
