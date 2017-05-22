#include "stdafx.h"
#include "PinkyController.h"
#include "../Physics/PhysicsCharacterController.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"
#include "../ThreadedPostmaster/Postmaster.h"

static const float gravityAcceleration = 9.82f * 2.0f;

CPinkyController::CPinkyController(unsigned int aId, eEnemyTypes aType)
	:CEnemy(aId, aType)
{
	myWindupChargeTime = 0.0f;
	myChargeDamage = 0.0f;
	myChargeSpeed = 0.0f;
	myGravityForce = 0.0f;
	myChargeCooldown = 0.0f;
	myElapsedChargeCooldownTime = 0.0f;
	myElapsedWindupTime = 0.0f;
	myIsCharging = false;
	myGravityForce = 0.0f;
	myElapsedChargeMeleeAttackTime = 0.0f;
	mySHouldntExist = false;
}

CPinkyController::~CPinkyController()
{
}

void CPinkyController::SetEnemyData(const SEnemyBlueprint* aData)
{
	const SPinkyBlueprint* pinkyData = static_cast<const SPinkyBlueprint*>(aData);
	myWindupChargeTime = pinkyData->windupChargeTime;
	myChargeDamage = pinkyData->chargeDamage;
	myChargeSpeed = pinkyData->chargeSpeed;
	myChargeDistance2 = pinkyData->chargeDistance * pinkyData->chargeDistance;
	myMeleeAttackChargeDuration = pinkyData->chargeMeleeAttackDuration;
	myMeleeAttackChargeDuration = 0.5f;
	CEnemy::SetEnemyData(aData);
}

void CPinkyController::Update(const float aDeltaTime)
{
	if(mySHouldntExist == true)
	{
		return;
	}
	UpdateBaseMemberVars(aDeltaTime);
	myGravityForce -= gravityAcceleration * aDeltaTime;
	myVelocity.y = myGravityForce;
	SendTransformationToServer();
	HandleGrounded();

	if (myIsDead == false && myIsCharging == false && CanChangeState() == true)
	{
		/*if (WithinAttackRange())
		{
			myState = ePinkyState::eChargingMeleeAttack;
		}
		else if (WithinWalkToMeleeRange())
		{
			myState = ePinkyState::eWalkIntoMeleeRange;
		}
		else */if (WithinShootRange())
		{
			myState = ePinkyState::eWindupCharge;
			myWindupChargeTime = 0.0f;
		}
		else if(WithinDetectionRange() == true)
		{
			myState = ePinkyState::eChase;
		}
		else
		{
			myState = ePinkyState::eIdle;
			if(myIsAggressive == true)
			{
				myState = ePinkyState::eChase;
			}
		}
	}


	switch (myState)
	{
	case ePinkyState::eIdle:
		break;
	case ePinkyState::eWalkIntoMeleeRange:
		LookAtPlayer(); //impl. turn rate?
		myVelocity.z = mySpeed;
	break;
	case ePinkyState::eUseMeleeAttack:
		ChangeWeapon(0);
		Attack();
		myState = ePinkyState::eIdle;
		break;
	case ePinkyState::eChase:
		LookAtPlayer(); //impl. turn rate?
		myVelocity.z = mySpeed;
		break;
	case ePinkyState::eWindupCharge:
		ChangeClientAnimation(eComponentMessageType::ePinkyLoadChange);
		myElapsedWindupTime += aDeltaTime;
		LookAtPlayer();
		if(myElapsedWindupTime >= myWindupChargeTime)
		{
			myElapsedWindupTime = 0.0f;
			myState = ePinkyState::eStartCharge;
			myIsCharging = true;

		}
		break;
	case ePinkyState::eStartCharge:
		myStartChargeLocation = myPos;
		myState = ePinkyState::eCharge;
	case ePinkyState::eCharge:
	{
		ChangeClientAnimation(eComponentMessageType::ePinkyCharge);
		myVelocity.z = myChargeSpeed;
		//if(GetIfSidesAreColliding() == true)
		myLastFramePostion.Print();
		GetParent()->GetWorldPosition().Print();
		float distance = CU::Vector3f(myLastFramePostion - GetParent()->GetWorldPosition()).Length();
		float minmoveChargesak = myChargeSpeed * aDeltaTime * 0.1f;
		if(distance < minmoveChargesak)
		{
			myIsCharging = false;
			myElapsedChargeCooldownTime = 0.0f;
			myState = ePinkyState::eChargeCooldown;
		}
		KeepWithinChargeDist();
		myLastFramePostion = GetParent()->GetWorldPosition();
		break;
	}
	case ePinkyState::eChargeCooldown:
		LookAtPlayer();
		UpdateChargeCooldown(aDeltaTime);
		break;
	case ePinkyState::eChargingMeleeAttack:
		myElapsedChargeMeleeAttackTime += aDeltaTime;
		if(myElapsedChargeMeleeAttackTime >= myMeleeAttackChargeDuration)
		{
			myElapsedChargeMeleeAttackTime = 0.0f;
			myState = ePinkyState::eUseMeleeAttack;
		}
		break;
	case ePinkyState::eDead:
		break;
	default:
		break;
	}

	CheckForNewTransformation(aDeltaTime);
}

void CPinkyController::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	CEnemy::Receive(aMessageType, aMessageData);
	switch (aMessageType)
	{
	case eComponentMessageType::eDied:
	{
		myIsDead = true;
		myState = ePinkyState::eDead;
		myIsCharging = false;
		//GetParent()->NotifyComponents(eComponentMessageType::eDeactivate, SComponentMessageData());
		if (myShouldNotReset == false)
		{
			CAddToCheckPointResetList* addToCheckPointMessage = new CAddToCheckPointResetList(GetParent());
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(addToCheckPointMessage);
			break;
		}
	}
	break;
	case eComponentMessageType::eOnCollisionEnter:
	{
		/*if(myIsCharging == true)
		{
			myIsCharging = false;
			myElapsedChargeCooldownTime = 0.0f;
			myState = ePinkyState::eChargeCooldown;
		}*/
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
			myState = ePinkyState::eIdle;
			break;
		}
	case eComponentMessageType::eNetworkDoDamage:
	{
		myIsAggressive = true;
		break;
	}
	break;
	}
}

void CPinkyController::HandleGrounded()
{
	SComponentQuestionData groundeddata;
	if (GetParent()->AskComponents(eComponentQuestionType::ePhysicsControllerConstraints, groundeddata) == true)
	{
		myControllerConstraints = groundeddata.myChar;
		if (myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_DOWN)
		{
			if (myGravityForce < 0)
			{
				myGravityForce = 0.0f;
			}
		}
	}
}

void CPinkyController::UpdateChargeCooldown(const float aDeltaTime)
{
	myElapsedChargeCooldownTime += aDeltaTime;
	if (myElapsedChargeCooldownTime >= myChargeCooldown)
	{
		myElapsedChargeCooldownTime = 0.0f;
	}
}

void CPinkyController::KeepWithinChargeDist()
{
	float distFromStart = (myStartChargeLocation - GetParent()->GetWorldPosition()).Length2();
	if (distFromStart > myChargeDistance2)
	{
		myStartChargeLocation = CU::Vector3f::Zero;
		myIsCharging = false;
	}
}

bool CPinkyController::CanChangeState()
{
	switch (myState)
	{
	case ePinkyState::eUseMeleeAttack:
		return false;
		break;
	case ePinkyState::eChargingMeleeAttack:
		return false;
		break;
	default:
		break;
	}
	return true;
}
