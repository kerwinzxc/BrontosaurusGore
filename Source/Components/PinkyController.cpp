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
	CEnemy::SetEnemyData(aData);
}

void CPinkyController::Update(const float aDeltaTime)
{
	CU::Vector3f velocity;
	velocity.y = -gravityAcceleration * aDeltaTime;
	myElapsedWaitingToSendMessageTime += aDeltaTime;
	const CU::Vector3f closestPlayerPos = ClosestPlayerPosition();
	const CU::Vector3f myPos = GetParent()->GetWorldPosition();
	const CU::Vector3f toPlayer = closestPlayerPos - myPos;
	const float distToPlayer = toPlayer.Length2();
	UpdateTransformationNetworked();

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


	if (myIsDead == false && myIsCharging == false)
	{
		if (myStartAttackRange2 > distToPlayer)
		{
			myState = ePinkyState::eUseMeleeAttack;
		}
		else if (myWalkToMeleeRange2 > distToPlayer)
		{
			myState = ePinkyState::eWalkIntoMeleeRange;
		}
		else if (myDetectionRange2 > distToPlayer)
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
	{
		break;
	}
	case ePinkyState::eWalkIntoMeleeRange:
	{
		LookAtPlayer(); //impl. turn rate?
		velocity.z = mySpeed;
	}
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
			myState = ePinkyState::eCharge;
			myIsCharging = true;
		}
		break;
	case ePinkyState::eCharge:
	{
		velocity.z = myChargeSpeed;
		break;
	}
	case ePinkyState::eChargeCooldown:
	{
		myElapsedChargeCooldownTime += aDeltaTime;
		LookAtPlayer();
		if(myElapsedChargeCooldownTime >= myChargeCooldown)
		{
			myElapsedChargeCooldownTime = 0.0f;
		}
		break;
	}
	case ePinkyState::eDead:
		break;
	default:
		break;
	}

	UpdateTransformationLocal(velocity, aDeltaTime);
	//CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
	//CU::Matrix44f rotation = parentTransform.GetRotation();
	//rotation.myForwardVector.y = 0.f;

	//SComponentQuestionData data;
	//data.myVector4f = velocity * rotation * aDeltaTime;
	//data.myVector4f.w = aDeltaTime;

	//if (GetParent()->AskComponents(eComponentQuestionType::eMovePhysicsController, data) == true)
	//{
	//	parentTransform.SetPosition(data.myVector3f);
	//	NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
	//}
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
