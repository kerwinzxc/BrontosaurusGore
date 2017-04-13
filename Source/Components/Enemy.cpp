#include "stdafx.h"
#include "Enemy.h"

#include "../TServer/ServerMessageManager.h"
#include "../TShared/NetworkMessage_EnemyTransformation.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"
#include "../Physics/PhysicsCharacterController.h"
#include "HighlightComponent.h"
#include "EnemyRunTowardsComponent.h"
#include "ParticleEmitterManager.h"

#define nej false
#define ja true


CU::GrowingArray<CGameObject*> CEnemy::ourPlayerObjects;
CU::GrowingArray<CEnemyRunTowardsComponent*> CEnemy::ourEnemyRunTowardsComponents;

CEnemy::CEnemy(unsigned int aId, eEnemyTypes aType): myDistToPlayer(0), mySpeed(0), myDetectionRange2(0), myWeaponRange2(0), myStartAttackRange2(0), myStopAttackRange2(0), myWalkToMeleeRange2(0), myIsAttacking(false), myControllerConstraints(0), myBloodSplatterTimer(0), myHighlightTimer(0), myDoingHighlight(false)
{
	myIsDead = false;
	myServerId = aId;
	myActiveWeaponIndex = 0;
	myNetworkPositionUpdateCoolDown = 1.0f / 60.0f;
	myElapsedWaitingToSendMessageTime = 0.0f;
	myShouldNotReset = nej;
	myType = aType;
	myRunTowardsComponentIndex = 999999;
	myIsAggressive = false;

	if (Audio::CAudioInterface::GetInstance() != nullptr)
	{
		myAudioID = Audio::CAudioInterface::GetInstance()->RegisterGameObject();
	}
}

CEnemy::~CEnemy()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

void CEnemy::UpdateBaseMemberVars(const float aDeltaTime)
{
	myVelocity = CU::Vector3f::Zero;
	myElapsedWaitingToSendMessageTime += aDeltaTime;
	myClosestPlayerPos = ClosestPlayerPosition();
	myPos = GetParent()->GetWorldPosition();
	myToPlayer = myClosestPlayerPos - myPos;
	myDistToPlayer = myToPlayer.Length2();

	if(myBloodSplatterTimer > 0)
	{
		myBloodSplatterTimer -= aDeltaTime;
	}

	DoDamageHighlight(aDeltaTime);
}

void CEnemy::SendTransformationToServer()
{
	if(myElapsedWaitingToSendMessageTime >= myNetworkPositionUpdateCoolDown)
	{
		CU::Matrix44f& transform = GetParent()->GetLocalTransform();

		CNetworkMessage_EnemyTransformation* message = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_EnemyTransformation>(ID_ALL_BUT_ME);
		message->SetId(myServerId);
		message->SetTransformation(transform);
		
		Postmaster::Threaded::CPostmaster::GetInstance().BroadcastLocal(new CSendNetworkMessageMessage(message));
		myElapsedWaitingToSendMessageTime = 0.0f;
	}
}

static const float HighlightDuration = .25f;

void CEnemy::DoDamageHighlight(const float aDeltaTime)
{
	static const float MaxIntensity = 0.75f;
	static const float MaxAlpha = 0.5f;
	static const float Periods = 1.f;
	if(myHighlightTimer > 0)
	{
		myHighlightTimer -= aDeltaTime;
		myDoingHighlight = true;
		const float t = 1.f - myHighlightTimer / HighlightDuration;
		const float angle = (2 * PI * t * Periods);
		const float curveVal = (-cos(angle) + 1.f) / 2;
		const float intersity = MaxIntensity * curveVal;
		const float alpha = MaxAlpha * curveVal;
		SetHighlight(CU::Vector4f(1, 0, 0, curveVal), intersity);
	}
	else if(myDoingHighlight == true)
	{
		myDoingHighlight = false;
		SetHighlight(CU::Vector4f(0, 0, 0, 0), 0.f);
	}
}

void CEnemy::SetHighlight(const CU::Vector4f& aColor, float aIntensity)
{
	CHighlightComponent highlight;
	highlight.SetColor(aColor);
	highlight.SetIntensity(aIntensity);
	SComponentMessageData messageData;
	messageData.myComponent = &highlight;
	GetParent()->NotifyComponents(eComponentMessageType::eSetHighlight, messageData);
}

void CEnemy::CheckForNewTransformation(const float aDeltaTime)
{
	CU::Matrix44f& transform = GetParent()->GetLocalTransform();
	CU::Matrix44f& rotation = transform.GetRotation();
	rotation.myForwardVector.y = 0.f;

	SComponentQuestionData data;
	if (myVelocity.z > 20)
		myVelocity.z = 20;
	if (myVelocity.y > 900)
		myVelocity.y = 900;
	data.myVector4f = myVelocity * rotation * aDeltaTime;
	data.myVector4f.w = aDeltaTime;
	if (GetParent()->AskComponents(eComponentQuestionType::eMovePhysicsController, data) == true)
	{
		transform.SetPosition(data.myVector3f);
		GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
	}
}

void CEnemy::MoveForward(const float aMovAmount)
{
	GetParent()->GetLocalTransform().Move({0.f, 0.f, aMovAmount});
}

void CEnemy::Attack()
{
	if(myIsDead == false)
	{
		SComponentMessageData messageData;
		CU::Vector3f direction = ClosestPlayerPosition() - GetParent()->GetWorldPosition();
		direction.Normalize();
		messageData.myVector3f = ClosestPlayerPosition();
		messageData.myVector4f.w = myServerId;
		GetParent()->NotifyComponents(eComponentMessageType::eServerShoot, messageData);
	}
}


void CEnemy::StartHighlight()
{
	myHighlightTimer = HighlightDuration;
}

void CEnemy::DoSplatter()
{
	if(myBloodSplatterTimer <= 0.f)
	{
		myBloodSplatterTimer = .25f;
		SComponentQuestionData questionData;
		GetParent()->AskComponents(eComponentQuestionType::eLastHitNormal, questionData);
		const CU::Vector3f normal = -questionData.myVector3f;
		GetParent()->AskComponents(eComponentQuestionType::eLastHitPosition, questionData);
		const CU::Vector3f position = questionData.myVector3f;

		CU::Matrix44f transformation;
		transformation.SetPosition(position);
		transformation.LookAt(position + normal);

		Particles::ParticleEmitterID id = CParticleEmitterManager::GetInstance().GetEmitterInstance("Blood");
		CParticleEmitterManager::GetInstance().SetTransformation(id, transformation);
		CParticleEmitterManager::GetInstance().Activate(id);

		CParticleEmitterManager::GetInstance().Release(id);
	}
	
}

void CEnemy::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eDied:
	{
		myIsDead = true;
		GetParent()->NotifyComponents(eComponentMessageType::eDeactivate, SComponentMessageData());
		if (myShouldNotReset == false)
		{
			CAddToCheckPointResetList* addToCheckPointMessage = new CAddToCheckPointResetList(GetParent());
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(addToCheckPointMessage);
			break;
		}
	}
	break;
	case eComponentMessageType::eObjectDone:
		break;
	case eComponentMessageType::eTakeDamage:
	{
		
	}
		break;
	case eComponentMessageType::eCheckPointReset:
	{
		if (myShouldNotReset == false)
		{
			myIsDead = false;
			SComponentMessageData visibilityData;
			visibilityData.myBool = true;
			GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
			GetParent()->NotifyComponents(eComponentMessageType::eActivate, SComponentMessageData());
			break;
		}
	}
	break;
	case eComponentMessageType::eDeactivate:
		myIsDead = ja;
		break;
	case eComponentMessageType::eActivate:
		myIsDead = nej;
	}
}

bool CEnemy::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData & aQuestionData)
{
	switch (aQuestionType)
	{
	case eComponentQuestionType::eEnemyNetworkID:
		aQuestionData.myInt = myServerId;
		return true;
		break;
	}
	return false;
}

void CEnemy::KillEverythingThenResetItAgain(const bool aFlag)
{
	myShouldNotReset = aFlag;
}

void CEnemy::SetPlayerObject(CGameObject* aPlayerObj)
{
	if (ourPlayerObjects.IsInitialized() == false)
	{
		ourPlayerObjects.Init(4);
	}
	ourPlayerObjects.Add(aPlayerObj);
}

CU::Vector3f CEnemy::ClosestPlayerPosition()
{
	const CU::Vector3f position = GetParent()->GetWorldPosition();
	
	if(ourPlayerObjects.IsInitialized() == false || ourPlayerObjects.Size() == 0 || ourPlayerObjects[0] == nullptr)
	{
		return position;
	}

	CU::Vector3f playerPos = ourPlayerObjects[0]->GetWorldPosition();
	for(unsigned int i = 0; i < ourPlayerObjects.Size(); ++i)
	{
		CGameObject*const player = ourPlayerObjects[i];
		const CU::Vector3f newPlayerPos = player->GetWorldPosition();
	
		if((position - playerPos).Length2() > (position - newPlayerPos).Length2())
		{
			playerPos = newPlayerPos;
		}
	}
	
	return playerPos;
}

void CEnemy::ChangeWeapon(const unsigned int aIndex)
{
	if(myActiveWeaponIndex != aIndex)
	{
		myActiveWeaponIndex = aIndex;
		SComponentMessageData changeWeaponData;
		changeWeaponData.myInt = myActiveWeaponIndex;
		GetParent()->NotifyComponents(eComponentMessageType::eSelectWeapon, changeWeaponData);
	}
}

void CEnemy::LookAtPlayer()
{
	CU::Vector3f lookatPosition = ClosestPlayerPosition();
	lookatPosition.y = GetParent()->GetLocalTransform().GetPosition().y;
	GetParent()->LookAt(lookatPosition);
}

eMessageReturn CEnemy::DoEvent(const CResetToCheckPointMessage& aResetToCheckPointMessage)
{
	GetParent()->GetLocalTransform().SetPosition(mySpawnPosition);
	SComponentMessageData controllerPositionData;
	controllerPositionData.myVector3f = mySpawnPosition;
	GetParent()->NotifyComponents(eComponentMessageType::eSetControllerPosition, controllerPositionData);
	myIsAggressive = false;
	return eMessageReturn::eContinue;
}

void CEnemy::Init()
{
	mySpawnPosition = GetParent()->GetLocalTransform().GetPosition();
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eResetToCheckPointMessage);
	SComponentMessageData healData;
	healData.myInt = 1000000;
	GetParent()->NotifyComponents(eComponentMessageType::eHeal, healData);
}

bool CEnemy::GetIfSidesAreColliding()
{
	SComponentQuestionData groundeddata;
	if (GetParent()->AskComponents(eComponentQuestionType::ePhysicsControllerConstraints, groundeddata) == true)
	{
		myControllerConstraints = groundeddata.myChar;
		if (myControllerConstraints & Physics::EControllerConstraintsFlag::eCOLLISION_SIDES)
		{
			return true;
		}
	}
	return false;
}

void CEnemy::AddEnemyRunTowardsComponent(CEnemyRunTowardsComponent* aEnemyRunTowardsComponent)
{
	if (ourEnemyRunTowardsComponents.IsInitialized() == false)
	{
		ourEnemyRunTowardsComponents.Init(4);
	}
	ourEnemyRunTowardsComponents.Add(aEnemyRunTowardsComponent);
}

const CU::Vector3f CEnemy::GetNearestJumpPosition()
{
	const CU::Vector3f position = GetParent()->GetWorldPosition();
	if (ourEnemyRunTowardsComponents.HasIndex(myRunTowardsComponentIndex) == true)
	{
		ourEnemyRunTowardsComponents[myRunTowardsComponentIndex]->SetIsOccupied(false);
	}

	if (ourEnemyRunTowardsComponents.IsInitialized() == false || ourEnemyRunTowardsComponents.Size() == 0)
	{
		return position;
	}

	CU::Vector3f playerPos;
	unsigned int startIndex = 0;
	for (unsigned int startIndex = 0; startIndex < ourEnemyRunTowardsComponents.Size(); ++startIndex)
	{
		if (ourEnemyRunTowardsComponents[startIndex]->GetIsOccupied() == false)
		{
			playerPos = ourEnemyRunTowardsComponents[startIndex]->GetParent()->GetWorldPosition();
			break;
		}
	}
	myRunTowardsComponentIndex = startIndex;
	for (unsigned int i = startIndex; i < ourEnemyRunTowardsComponents.Size(); ++i)
	{
		if (ourEnemyRunTowardsComponents[i]->GetIsOccupied() == false)
		{
			CEnemyRunTowardsComponent*const enemyRunTowardsComponent = ourEnemyRunTowardsComponents[i];
			const CU::Vector3f newPlayerPos = enemyRunTowardsComponent->GetParent()->GetWorldPosition();

			if ((position - playerPos).Length2() > (position - newPlayerPos).Length2())
			{
				playerPos = newPlayerPos;
				myRunTowardsComponentIndex = i;
			}
		}
	}

	if (ourEnemyRunTowardsComponents.HasIndex(myRunTowardsComponentIndex) == true)
	{
		ourEnemyRunTowardsComponents[myRunTowardsComponentIndex]->SetIsOccupied(true);
	}

	return playerPos;
}

void CEnemy::playerDontFuckTHisUp()
{
	if(ourPlayerObjects.IsInitialized() == true)
	{
		ourPlayerObjects.RemoveAll();
		ourPlayerObjects.Destroy();
	}

	if (ourEnemyRunTowardsComponents.IsInitialized() == true)
	{
		ourEnemyRunTowardsComponents.RemoveAll();
		ourEnemyRunTowardsComponents.Destroy();
	}
}