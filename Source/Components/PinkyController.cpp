#include "stdafx.h"
#include "PinkyController.h"
#include "../Physics/PhysicsCharacterController.h"

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
	CEnemy::SetEnemyData(aData);
}

void CPinkyController::Update(const float aDeltaTime)
{
	CU::Vector3f velocity;
	myGravityForce -= gravityAcceleration * aDeltaTime;
	velocity.y = myGravityForce;
	myElapsedWaitingToSendMessageTime += aDeltaTime;
	const CU::Vector3f closestPlayerPos = ClosestPlayerPosition();
	const CU::Vector3f myPos = GetParent()->GetWorldPosition();
	const CU::Vector3f toPlayer = closestPlayerPos - myPos;
	const float distToPlayer = toPlayer.Length2();

	UpdateTransformationNetworked();
	HandleGrounded();

	if (myIsDead == false && myIsCharging == false)
	{
		if (WithinAttackRange(distToPlayer))
		{
			myState = ePinkyState::eUseMeleeAttack;
		}
		else if (WithinWalkToMeleeRange(distToPlayer))
		{
			myState = ePinkyState::eWalkIntoMeleeRange;
		}
		else if (WithinDetectionRange(distToPlayer))
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
		velocity.z = mySpeed;
	break;
	case ePinkyState::eUseMeleeAttack:
		ChangeWeapon(0);
		Attack();
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
		velocity.z = myChargeSpeed;
		KeepWithinChargeDist();
		break;
	case ePinkyState::eChargeCooldown:
		LookAtPlayer();
		UpdateChargeCooldown(aDeltaTime);
		break;
	case ePinkyState::eDead:
		break;
	default:
		break;
	}

	UpdateTransformationLocal(velocity, aDeltaTime);
}

void CPinkyController::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eDied:
	{
		myIsDead = true;
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
