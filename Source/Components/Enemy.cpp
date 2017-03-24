#include "stdafx.h"
#include "Enemy.h"

#include "../TServer/ServerMessageManager.h"
#include "../TShared/NetworkMessage_EnemyTransformation.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"

CU::GrowingArray<CGameObject*> Component::CEnemy::ourPlayerObjects;

Component::CEnemy::CEnemy(unsigned int aId): mySpeed(0), myDetectionRange2(0), myStartAttackRange2(0), myStopAttackRange2(0), myIsAttacking(false)
{
	myIsDead = false;
	myServerId = aId;
	myActiveWeaponIndex = 0;
}

Component::CEnemy::~CEnemy()
{
}

void Component::CEnemy::UpdateTransformation()
{
	const CU::Matrix44f tranformation = GetParent()->GetLocalTransform();
	CNetworkMessage_EnemyTransformation* message = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_EnemyTransformation>(ID_ALL_BUT_ME);
	message->SetId(myServerId);
	message->SetTransformation(tranformation);

	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(message));
}

void Component::CEnemy::MoveForward(const float aMovAmount)
{
	GetParent()->GetLocalTransform().Move({0.f, 0.f, aMovAmount});
}

void Component::CEnemy::Attack()
{
	SComponentMessageData messageData;
	messageData.myVector3f = GetParent()->GetWorldPosition();
	messageData.myVector4f.w = myServerId;
	GetParent()->NotifyComponents(eComponentMessageType::eServerShoot, messageData);
}

void Component::CEnemy::Update(const CU::Time& aDeltaTime)
{
	if (myIsDead == false)
	{
		bool hasChanged = false;
		const CU::Vector3f closestPlayerPos = ClosestPlayerPosition();
		const CU::Vector3f myPos = GetParent()->GetWorldPosition();
		const CU::Vector3f toPlayer = closestPlayerPos - myPos;
		const float distToPlayer = toPlayer.Length2();


		if (WithinDetectionRange(distToPlayer))
		{
			hasChanged = true;
			GetParent()->Face(toPlayer); //impl. turn rate?
		}


		if (myIsAttacking == false)
		{
			if (WithinAttackRange(distToPlayer))
				myIsAttacking = true;

			else if (WithinDetectionRange(distToPlayer))
			{
				float movementAmount = mySpeed * aDeltaTime.GetSeconds();
				//SComponentQuestionData data;
				//data.myVector4f = CU::Vector4f(0.f, 0.f, movementAmount, aDeltaTime.GetSeconds());
				//if(GetParent()->AskComponents(eComponentQuestionType::eMovePhysicsController, data) == true)
					MoveForward(movementAmount);
			}
		}

		if(myIsAttacking == true)
		{
			if (OutsideAttackRange(distToPlayer))
				myIsAttacking = false;

			Attack();
		}

		if(hasChanged == true)
		{
			UpdateTransformation();
		}
	}
}

void Component::CEnemy::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eDied:
		myIsDead = true;
		break;
	}
}

void Component::CEnemy::SetPlayerObject(CGameObject* aPlayerObj)
{
	if (ourPlayerObjects.IsInitialized() == false)
		ourPlayerObjects.Init(4);
	ourPlayerObjects.Add(aPlayerObj);
}

CU::Vector3f Component::CEnemy::ClosestPlayerPosition()
{
	const CU::Vector3f position = GetParent()->GetWorldPosition();
	
	if(ourPlayerObjects.IsInitialized() == false || ourPlayerObjects.Size() == 0)
	{
		return position;
	}
	
	CU::Vector3f playerPos = ourPlayerObjects[0]->GetWorldPosition();
	for(int i = 0; i < ourPlayerObjects.Size(); ++i)
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

void Component::CEnemy::ChangeWeapon(const unsigned int aIndex)
{
	myActiveWeaponIndex = aIndex;
	SComponentMessageData changeWeaponData;
	changeWeaponData.myInt = myActiveWeaponIndex;
	GetParent()->NotifyComponents(eComponentMessageType::eSelectWeapon, changeWeaponData);
}