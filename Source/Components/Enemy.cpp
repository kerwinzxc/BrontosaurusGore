#include "stdafx.h"
#include "Enemy.h"
#include "../TServer/ServerMessageManager.h"
#include "../TShared/NetworkMessage_EnemyPosition.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "PollingStation.h"

//CU::GrowingArray<CGameObject*> Component::CEnemy::myPlayerObject;
CGameObject* Component::CEnemy::myPlayerObj;

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
	//CNetworkMessage_EnemyPosition* message = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_EnemyPosition>(ID_ALL_BUT_ME);
	//message->SetId(myServerId);
	// message->SetPosition(pos);

	///Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(message));
}

void Component::CEnemy::Update(const CU::Time& aDeltaTime)
{
	CGameObject* coolo = CPollingStation::GetInstance()->GetPlayerObject();
	if (myPlayerObj != nullptr)
	{
		CU::Vector3f playerPosition = myPlayerObj->GetWorldPosition();

		GetParent()->GetLocalTransform().LookAt(playerPosition);
		GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());


		//if (myIsDead == false)
			//{

			//	const CU::Vector3f playerPosition = CPollingStation::GetInstance()->GetPlayerObject()->GetWorldPosition();
			//	const CU::Vector3f position = GetParent()->GetWorldPosition();
			//	const CU::Vector3f toPlayer = playerPosition - position;
			//	const float distToPlayer = toPlayer.Length2();

			//	if (distToPlayer < myDetectionRange2)
			//	{
			//		//GetParent()->Face(toPlayer);
			//		GetParent()->GetLocalTransform().LookAt(playerPosition);
			//		

			//	}
			//	if (distToPlayer < myStartAttackRange2)
			//	{

			//	}
			//	else if (distToPlayer < myDetectionRange2)
			//	{
			//		float movementAmount = mySpeed * aDeltaTime.GetSeconds();
			//		//CU::Vector3f displacement = toPlayer.GetNormalized() * movementAmount;
			//		//Move(displacement);

			//		//if (GetParent()->AskComponents(eComponentQuestionType::eMovePhysicsController, data) == true)
			//		//{
			//			Move({ 0.f, 0.f, movementAmount });
			//		//}
			//	}

			//}
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

void Component::CEnemy::SetPlayer(CGameObject* aPlayerObject)
{
	myPlayerObj = aPlayerObject; //s�tts genom server ocks� tydligen ???
	DL_PRINT("PlayerObj set in enemy.");
	//if (myPlayerObject.IsInitialized() == false)
	//{
	//	myPlayerObject.Init(2);
	//}
	//myPlayerObject.Add(playerObject);
}

CU::Vector3f Component::CEnemy::ClosestPlayerPosition()
{
	//const CU::Vector3f position = GetParent()->GetWorldPosition();
	//
	//if(myPlayerObject.IsInitialized() == false || myPlayerObject.Size() == 0)
	//{
	//	return position;
	//}
	//
	//CU::Vector3f playerPos = myPlayerObject[0]->GetWorldPosition();
	//for(int i = 0; i < myPlayerObject.Size(); ++i)
	//{
	//	CGameObject*const player = myPlayerObject[i];
	//	const CU::Vector3f newPlayerPos = player->GetWorldPosition();
	//
	//	if((position - playerPos).Length2() > (position - newPlayerPos).Length2())
	//	{
	//		playerPos = newPlayerPos;
	//	}
	//}
	//
	//return playerPos;
	return CU::Vector3f( 0,0,0 );
}
