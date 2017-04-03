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
	CEnemy::SetEnemyData(aData);
}

void CPinkyController::Update(const float aDeltaTime)
{
	UpdateBaseMemberVars(aDeltaTime);
	myGravityForce -= gravityAcceleration * aDeltaTime;
	myVelocity.y = myGravityForce;
	SendTransformationToServer();
	HandleGrounded();

	if (myIsDead == false && myIsCharging == false && myState != ePinkyState::eChargingMeleeAttack && myState != ePinkyState::eUseMeleeAttack)
	{
		if (WithinAttackRange())
		{
			myState = ePinkyState::eChargingMeleeAttack;
		}
		else if (WithinWalkToMeleeRange())
		{
			myState = ePinkyState::eWalkIntoMeleeRange;
		}
		else if (WithinDetectionRange())
		{
			myState = ePinkyState::eWindupCharge;
			myWindupChargeTime = 0.0f;
		}
		else
		{
			myState = ePinkyState::eIdle;
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
	case ePinkyState::eWindupCharge:
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
		myVelocity.z = myChargeSpeed;
		KeepWithinChargeDist();
		break;
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
	switch (aMessageType)
	{
	case eComponentMessageType::eDied:
	{
		myIsDead = true;
		CAddToCheckPointResetList* addToCheckPointMessage = new CAddToCheckPointResetList(GetParent());
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(addToCheckPointMessage);
		myState = ePinkyState::eDead;
		break;
	}
	case eComponentMessageType::eOnCollisionEnter:
	{
		if(myIsCharging == true)
		{
			myIsCharging = false;
			myElapsedChargeCooldownTime = 0.0f;
			myState = ePinkyState::eChargeCooldown;
		}
		break;
	}
	case eComponentMessageType::eCheckPointReset:
		myIsDead = false;
		SComponentMessageData visibilityData;
		visibilityData.myBool = true;
		GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
		myState = ePinkyState::eIdle;
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
