#include "stdafx.h"
#include "ImpController.h"
#include "EnemyBlueprint.h"


CImpController::CImpController(unsigned int aId, eEnemyTypes aType)
	: CEnemy(aId, aType)
{
	myState = eImpState::eIdle;
	myJumpHeight = 0.0f;
	myShouldGoMeleeRadius2 = 0.0f;
}


CImpController::~CImpController()
{
}

void CImpController::Update(const float aDeltaTime)
{
	myElapsedWaitingToSendMessageTime += aDeltaTime;
	const CU::Vector3f closestPlayerPos = ClosestPlayerPosition();
	const CU::Vector3f myPos = GetParent()->GetWorldPosition();
	const CU::Vector3f toPlayer = closestPlayerPos - myPos;
	const float distToPlayer = toPlayer.Length2();
	DL_PRINT("distPlayer %f", distToPlayer);
	if(myIsDead == false)
	{
		if (myStartAttackRange2 > distToPlayer)
		{
			DL_PRINT("attacking?");
			myState = eImpState::eUseMeleeAttack;
		}
		else if (myShouldGoMeleeRadius2 > distToPlayer)
		{
			DL_PRINT("Walking forward");
			myState = eImpState::eWalkIntoMeleeRange;
		}
		else if (myDetectionRange2 > distToPlayer)
		{
			DL_PRINT("using ranged attack");
			myState = eImpState::eUseRangedAttack;
		}
		else
		{
			DL_PRINT("idling");
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
		CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
		CU::Matrix44f rotation = parentTransform.GetRotation();
		rotation.myForwardVector.y = 0.f;

		SComponentQuestionData data;
		data.myVector4f = CU::Vector3f(0.0f, 0.0f, mySpeed) * rotation * aDeltaTime;
		data.myVector4f.w = aDeltaTime;

		if (GetParent()->AskComponents(eComponentQuestionType::eMovePhysicsController, data) == true)
		{
			//parentTransform.SetPosition(data.myVector3f);
			NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
		}
		GetParent()->Move(CU::Vector3f(0.0f, 0.0f, mySpeed) * aDeltaTime);  // Remove this when character cotroll

		UpdateTransformation();
	}
		break;
	case eImpState::eUseMeleeAttack:
		break;
	case eImpState::eUseRangedAttack:
		break;
	case eImpState::eJump:
		break;
	case eImpState::eDead:
		break;
	default:
		break;
	}
}

void CImpController::SetEnemyData(const SEnemyBlueprint* aData)
{
	const SImpBlueprint* impData = static_cast<const SImpBlueprint*>(aData);
	myJumpHeight = impData->jumpHeight;
	myShouldGoMeleeRadius2 = impData->goMeleeRadius * impData->goMeleeRadius;
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