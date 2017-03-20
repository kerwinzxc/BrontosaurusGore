#include "stdafx.h"
#include "Enemy.h"
#include "../TServer/ServerMessageManager.h"
#include "../TShared/NetworkMessage_EnemyPosition.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"

CGameObject* Component::CEnemy::myPlayerObject = nullptr;

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
	if(myPlayerObject != nullptr)
	{
		if (myIsDead == false)
		{
			const CU::Vector3f playerPosition = myPlayerObject->GetWorldPosition();
			const CU::Vector3f position = GetParent()->GetWorldPosition();
			const CU::Vector3f toPlayer = playerPosition - position;
			const float distToPlayer = toPlayer.Length2();

			if (distToPlayer < myDetectionRange2)
			{
				//GetParent()->Face(toPlayer);
				//GetParent()->GetToWorldTransform().LookAt(playerPosition);
				//GetParent()->GetLocalTransform().Rotate(distToPlayer, CU::Axees::Y);
				GetParent()->GetLocalTransform().LookAt(playerPosition);
				GetParent()->GetLocalTransform().Rotate(distToPlayer, CU::Axees::Z);
				//GetParent()->GetToWorldTransform().CreateLookAt(playerPosition);
				//GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());

			}
			if (distToPlayer < myStartAttackRange2)
			{

			}
			else if (distToPlayer < myDetectionRange2)
			{
				float movementAmount = mySpeed * aDeltaTime.GetSeconds();
				//CU::Vector3f displacement = toPlayer.GetNormalized() * movementAmount;
				//Move(displacement);
			
				//CU::Matrix44f rot = GetParent()->GetLocalTransform().GetRotation();
				SComponentQuestionData data; // Velocity * rotation * deltaTime;							
				data.myVector4f = position; //(w also = deltaTime for some reason)
				data.myVector4f.z += movementAmount;

				data.myVector4f.w = aDeltaTime.GetSeconds(); // mebe ?
				//

				//if (GetParent()->AskComponents(eComponentQuestionType::eMovePhysicsController, data) == true)
				//{
					Move({ 0.f, 0.f, movementAmount });
				//}
			}

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
	myPlayerObject = playerObject;
}
