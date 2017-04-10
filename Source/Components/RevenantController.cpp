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
	myElapsedChargeMeleeAttackTime = 0.0f;
	myElapsedHoverTime = 0.0f;
	myElapsedChargeRangedAttackTime = 0.0f,
	myElapsedChargeRangedAirBarrageAttackTime = 0.0f;
}

CRevenantController::~CRevenantController()
{
}

void CRevenantController::SetEnemyData(const SEnemyBlueprint* aData)
{
	const SRevenantBlueprint* revenantData = static_cast<const SRevenantBlueprint*>(aData);
	myHoverTime = revenantData->hoverTime;
	myFlightHeight = revenantData->flightHeight;
	myChargeMeleeAttackDuration = revenantData->chargeMeleeAttackDuration;
	myChargeRangedAttackDuration = revenantData->chargeRangedAttackAttackDuration;
	myChargeRangedAirBarrageAttackDuration = revenantData->chargeAirBarrageAttackDuration;
	CEnemy::SetEnemyData(aData);
}

void CRevenantController::Update(const float aDeltaTime)
{
	UpdateBaseMemberVars(aDeltaTime);
	myVelocity.y = myFlightForce;
	if(myIsflying == false)
	{
		myFlightForce -= gravityAcceleration * aDeltaTime;
		if (CheckIfInAir() == false)
		{
			myFlightForce = 0.0f;
		}
	}
	SendTransformationToServer();
	UpdateFlightForces(aDeltaTime);

	if (myIsDead == false && myIsflying == false && CanChangeState() == true)
	{
		if (WithinAttackRange())
		{
			myState = eRevenantState::eChargingMeleeAttack;
		}
		else if (WithinWalkToMeleeRange())
		{
			myState = eRevenantState::eWalkIntoMeleeRange;
		}
		else if (WithinShootRange())
		{
			myState = eRevenantState::eChargingRangedAttack;
			if (myToPlayer.y > 2.0f)
			{
				myState = eRevenantState::eFlyAscend;
				ApplyFlightForce();
			}
		}
		else if (WithinDetectionRange())
		{
			myState = eRevenantState::eChase;
		}
		else
		{
			myState = eRevenantState::eIdle;
			if(myIsAggressive == true)
			{
				myState = eRevenantState::eChase;
			}
		}
	}
	else
	{
		if (myIsDead == true)
		{
			myState = eRevenantState::eDead;
		}
	}


	switch (myState)
	{
	case eRevenantState::eIdle:
		break;
	case eRevenantState::eWalkIntoMeleeRange:
		myFlightForce -= gravityAcceleration * aDeltaTime;
		LookAtPlayer();
		myVelocity.z = mySpeed;
		if (myToPlayer.y > 2.0f)
		{
			ApplyFlightForce();
		}
	break;
	case eRevenantState::eUseMeleeAttack:
		ChangeWeapon(2);
		Attack();
		myState = eRevenantState::eIdle;
		break;
	case eRevenantState::eUseRangedAttack:
		ChangeWeapon(0);
		Attack();
		myState = eRevenantState::eIdle;
		break;
	case eRevenantState::eChase:
		LookAtPlayer();
		myVelocity.z = mySpeed;
		if (myToPlayer.y > 2.0f)
		{
			ApplyFlightForce();
		}
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
		myElapsedChargeRangedAirBarrageAttackTime += aDeltaTime;
		if (myElapsedChargeRangedAirBarrageAttackTime >= myChargeRangedAirBarrageAttackDuration)
		{
			myElapsedChargeRangedAirBarrageAttackTime = 0.0f;
			Attack();
		}

		myElapsedHoverTime += aDeltaTime;
		if(myElapsedHoverTime >= myHoverTime)
		{
			myElapsedChargeRangedAirBarrageAttackTime = 0.0f;
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
	case eRevenantState::eChargingMeleeAttack:
	{
		myElapsedChargeMeleeAttackTime += aDeltaTime;
		if(myElapsedChargeMeleeAttackTime >= myChargeMeleeAttackDuration)
		{
			myElapsedChargeMeleeAttackTime = 0.0f;
			myState = eRevenantState::eUseMeleeAttack;
		}
		break;
	}
	case eRevenantState::eChargingRangedAttack:
	{
		myElapsedChargeRangedAttackTime += aDeltaTime;
		if (myElapsedChargeRangedAttackTime >= myChargeRangedAttackDuration)
		{
			myElapsedChargeRangedAttackTime = 0.0f;
			myState = eRevenantState::eUseRangedAttack;
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
		myIsflying = false;
		myFlightForce = 0.0f;
		GetParent()->NotifyComponents(eComponentMessageType::eDeactivate, SComponentMessageData());
		if (myShouldNotReset == false)
		{
			CAddToCheckPointResetList* addToCheckPointMessage = new CAddToCheckPointResetList(GetParent());
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(addToCheckPointMessage);
			break;
		}
	}
	break;
	case eComponentMessageType::eNetworkDoDamage:
	{
		myIsAggressive = true;
		break;
	}
	case eComponentMessageType::eCheckPointReset:
		myIsDead = false;
		SComponentMessageData visibilityData;
		visibilityData.myBool = true;
		GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
		GetParent()->NotifyComponents(eComponentMessageType::eActivate, SComponentMessageData());
		myFlightForce = 0.0f;
		myState = eRevenantState::eIdle;
		myIsflying = false;
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

bool CRevenantController::CanChangeState()
{
	switch (myState)
	{
	case eRevenantState::eUseMeleeAttack:
		return false;
		break;
	case eRevenantState::eChargingMeleeAttack:
		return false;
		break;
	case eRevenantState::eChargingRangedAttack:
		return false;
		break;
	case eRevenantState::eUseRangedAttack:
		return false;
		break;
	default:
		break;
	}
	return true;
}

eMessageReturn CRevenantController::DoEvent(const CResetToCheckPointMessage& aResetToCheckPointMessage)
{
	myFlightForce = 0.0f;
	myState = eRevenantState::eIdle;
	return CEnemy::DoEvent(aResetToCheckPointMessage);
}