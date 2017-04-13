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
	myAttacksUntilChangingStates = 3;
	myUsedAttacksSinceLastStateChange = 0;
	myIsAtJumpPoint = false;
	myChillAtJumpPointCountDown = 0.0f;
	myWaitBeforeChangingStateCountdown = 0.0f;
	myShouldntExist = false;
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
	myChargeMeleeAttackDuration = 0.5f;
	myChargeRangedAttackDuration = 0.1f;
	myChargeRangedAirBarrageAttackDuration = 0.1;
	CEnemy::SetEnemyData(aData);
}

void CRevenantController::Update(const float aDeltaTime)
{
	if(myShouldntExist == true)
	{
		return;
	}
	if(myChillAtJumpPointCountDown > 0)
	{
		myChillAtJumpPointCountDown -= aDeltaTime;
		if(myChillAtJumpPointCountDown <= 0)
		{
			myIsAtJumpPoint = false;
		}
	}
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
			if(myIsAtJumpPoint == true)
			{
				StartCharginRangedAttack();
			}
		}
		else if (WithinShootRange())
		{
			StartCharginRangedAttack();
		}
		else if (WithinDetectionRange())
		{
			myIsAggressive = true;
			myState = eRevenantState::eChase;
			if (myIsAtJumpPoint == true)
			{
				StartCharginRangedAttack();
			}
		}
		else
		{
			myState = eRevenantState::eIdle;
			if(myIsAggressive == true)
			{
				myState = eRevenantState::eChase;
				if (myIsAtJumpPoint == true)
				{
					StartCharginRangedAttack();
				}
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
		if (GetParent()->AskComponents(eComponentQuestionType::eCanShoot, SComponentQuestionData()) == true)
		{
			Attack();
			myState = eRevenantState::eIdle;
		}
		break;
	case eRevenantState::eUseRangedAttack:
		ChangeWeapon(0);
		if (GetParent()->AskComponents(eComponentQuestionType::eCanShoot, SComponentQuestionData()) == true)
		{
			Attack();
			myUsedAttacksSinceLastStateChange++;
			myState = eRevenantState::eChargingRangedAttack;
			if (myUsedAttacksSinceLastStateChange >= myAttacksUntilChangingStates)
			{
				myElapsedChargeRangedAttackTime = 0.0f;
				myUsedAttacksSinceLastStateChange = 0;
				myState = eRevenantState::eIdle;
				if(myIsAtJumpPoint == true)
				{
					myState = eRevenantState::eWaitBeforeChangingState;
					myWaitBeforeChangingStateCountdown = 1.0f;
				}
			}
		
		}
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
		myFlightForce = 0.0f;
		LookAtPlayer();
		ChangeWeapon(1);
		if (GetParent()->AskComponents(eComponentQuestionType::eCanShoot, SComponentQuestionData()) == true)
		{
			myElapsedChargeRangedAirBarrageAttackTime += aDeltaTime;
			if (myElapsedChargeRangedAirBarrageAttackTime >= myChargeRangedAirBarrageAttackDuration)
			{
				myElapsedChargeRangedAirBarrageAttackTime = 0.0f;
				Attack();
				myUsedAttacksSinceLastStateChange++;
			}

			if(myUsedAttacksSinceLastStateChange >= myAttacksUntilChangingStates)
			{
				myElapsedChargeRangedAirBarrageAttackTime = 0.0f;
				myElapsedHoverTime = 0.0f;
				myState = eRevenantState::eFlyDescend;
				myUsedAttacksSinceLastStateChange = 0;
			}
		/*	myElapsedHoverTime += aDeltaTime;
			if(myElapsedHoverTime >= myHoverTime)
			{
				myElapsedChargeRangedAirBarrageAttackTime = 0.0f;
				myElapsedHoverTime = 0.0f;
				myState = eRevenantState::eFlyDescend;
			}*/	
		}
		break;
	}
	case eRevenantState::eFlyDescend:
	{
		if (myIsAtJumpPoint == true)
		{
			LookAtPlayer();

			myFlightForce -= gravityAcceleration * aDeltaTime;
			if (CheckIfInAir() == false)
			{
				myIsflying = false;
				myFlightForce = 0.0f;
			}
		}
		else
		{
			if (myJumpPointPosition == CU::Vector3f::Zero)
			{
				myJumpPointPosition = GetNearestJumpPosition();
			}
			CU::Vector3f position = GetParent()->GetLocalTransform().GetPosition();
			CU::Vector3f targetPosition = myJumpPointPosition;
			if (position.y < targetPosition.y + 2.0f)
			{
				myFlightForce = sqrtf((gravityAcceleration)* myFlightHeight * 2);
			}
			else
			{
				myFlightForce = 0.0f;
			}
			myVelocity.z = mySpeed;
			targetPosition.y = position.y;
			GetParent()->GetLocalTransform().LookAt(targetPosition);
			float distance = CU::Vector3f(targetPosition - position).Length();
			if (distance < 0.5f)
			{
				LookAtPlayer();
				myIsAtJumpPoint = true;
				myChillAtJumpPointCountDown = rand() % 7 + 3;
			}

		}
		break;
	}
	case eRevenantState::eChargingMeleeAttack:
	{
			LookAtPlayer();
			myElapsedChargeMeleeAttackTime += aDeltaTime;
			if (myElapsedChargeMeleeAttackTime >= myChargeMeleeAttackDuration)
			{
				myElapsedChargeMeleeAttackTime = 0.0f;
				myState = eRevenantState::eUseMeleeAttack;
			}
		break;
	}
	case eRevenantState::eChargingRangedAttack:
	{
		LookAtPlayer();
		myElapsedChargeRangedAttackTime += aDeltaTime;
		if (myElapsedChargeRangedAttackTime >= myChargeRangedAttackDuration)
		{
			myElapsedChargeRangedAttackTime = 0.0f;
			myState = eRevenantState::eUseRangedAttack;
		}
		break;
	}
	case eRevenantState::eWaitBeforeChangingState:
	{
		myWaitBeforeChangingStateCountdown -= aDeltaTime;
		if(myWaitBeforeChangingStateCountdown <= 0)
		{
			myState = eRevenantState::eIdle;
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
		if (myShouldNotReset == false)
		{
			myIsDead = false;
			SComponentMessageData visibilityData;
			visibilityData.myBool = true;
			GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
			GetParent()->NotifyComponents(eComponentMessageType::eActivate, SComponentMessageData());
			myFlightForce = 0.0f;
			myState = eRevenantState::eIdle;
			myIsflying = false;
		}
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
	case eRevenantState::eWaitBeforeChangingState:
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
	myIsAtJumpPoint = false;
	myIsflying = false;
	return CEnemy::DoEvent(aResetToCheckPointMessage);
}

void CRevenantController::StartCharginRangedAttack()
{
	myState = eRevenantState::eChargingRangedAttack;
	unsigned short randomNumber = rand() % 5;
	if (randomNumber == 0)
	{
		myState = eRevenantState::eFlyAscend;
		ApplyFlightForce();
	}
	if (myToPlayer.y > 2.0f)
	{
		myState = eRevenantState::eFlyAscend;
		ApplyFlightForce();
	}
}