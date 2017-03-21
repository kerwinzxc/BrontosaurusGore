#include "stdafx.h"
#include "Enemy.h"
#include "../TServer/ServerMessageManager.h"
#include "../TShared/NetworkMessage_EnemyPosition.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"

CU::GrowingArray<CGameObject*> Component::CEnemy::myPlayerObject;

Component::CEnemy::CEnemy(unsigned int anId): myHealth(0), mySpeed(0), myDetectionRange2(0), myStartAttackRange2(0), myStopAttackRange2(0), myServerId(anId)
{
	myIsDead = false;
}


Component::CEnemy::~CEnemy()
{
}

void Component::CEnemy::Move(const CU::Vector3f& aDispl)
{

	GetParent()->GetLocalTransform().Move(aDispl);
	const CU::Vector3f pos = GetParent()->GetWorldPosition();
	CNetworkMessage_EnemyPosition* message = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_EnemyPosition>(ID_ALL_BUT_ME);
	message->SetId(myServerId);
	message->SetPosition(pos);

	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(message));
}

void Component::CEnemy::Update(const CU::Time& aDeltaTime)
{

		if (myIsDead == false)
		{

			const CU::Vector3f playerPosition = ClosestPlayerPosition();
			const CU::Vector3f position = GetParent()->GetWorldPosition();
			const CU::Vector3f dif = playerPosition - position;
			const CU::Vector3f difNorm = dif.GetNormalized();
			const float dist2 = dif.Length2();

			if (dist2 < myStartAttackRange2)
			{

			}
			else if (dist2 < myDetectionRange2)
			{
				float movementAmount = mySpeed * aDeltaTime.GetSeconds();
				//const CU::Vector3f displ = difNorm * mySpeed * aDeltaTime.GetSeconds();
				
				Move(CU::Vector3f(0.0f, 0.0f, movementAmount));
				
			}
			if (dist2 < myDetectionRange2)
			{
				GetParent()->Face(difNorm);
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

void Component::CEnemy::SetPlayer(CGameObject* playerObject)
{
	if (myPlayerObject.IsInitialized() == false)
	{
		myPlayerObject.Init(2);
	}
	myPlayerObject.Add(playerObject);
}

CU::Vector3f Component::CEnemy::ClosestPlayerPosition()
{
	const CU::Vector3f position = GetParent()->GetWorldPosition();

	if(myPlayerObject.IsInitialized() == false || myPlayerObject.Size() == 0)
	{
		return position;
	}

	CU::Vector3f playerPos = myPlayerObject[0]->GetWorldPosition();
	for(int i = 0; i < myPlayerObject.Size(); ++i)
	{
		CGameObject*const player = myPlayerObject[i];
		const CU::Vector3f newPlayerPos = player->GetWorldPosition();

		if((position - playerPos).Length2() > (position - newPlayerPos).Length2())
		{
			playerPos = newPlayerPos;
		}
	}

	return playerPos;
}
