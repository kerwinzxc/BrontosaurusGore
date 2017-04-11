#include "stdafx.h"
#include "DoorComponent.h"
#include "PollingStation.h"
#include "..\TShared\NetworkMessage_DoorMessage.h"
#include "..\ThreadedPostmaster\Postmaster.h"
#include "..\ThreadedPostMaster\SendNetowrkMessageMessage.h"
#include "..\TClient\ClientMessageManager.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"
#include "../Game/PollingStation.h"


CDoorComponent::CDoorComponent()
{
	myIsClosed = true;
	myShouldReset = true;
	myIsLocked = false;
	myAddToCheckpoint = false;
	myLockId = -1;
	myNetworkID = 0;
	myMoveDistance = 15;
	myMoveSpeed = 0.25f;
	myOpenDirection = CU::Vector2f::Zero;
	myOriginPosition = CU::Vector2f::Zero;
	myType = eComponentType::eDoor;
	myResetToPosition = CU::Matrix44f::Identity;
}


CDoorComponent::~CDoorComponent()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

void CDoorComponent::SetOpenDirection(const CU::Vector2f & aDirection)
{
	myOpenDirection = aDirection;
}

void CDoorComponent::SetMoveSpeed(const float aMoveSpeed)
{
	myMoveSpeed = aMoveSpeed;
}

void CDoorComponent::SetMoveDistance(const float aDistance)
{
	myMoveDistance = aDistance;
}

void CDoorComponent::SetIsClosed(const bool aIsClosed)
{
	myIsClosed = aIsClosed;
}

void CDoorComponent::SetShouldReset(const bool aShouldReset)
{
	myShouldReset = aShouldReset;
}

void CDoorComponent::SetIsLocked(const bool aIsLocked)
{
	myIsLocked = aIsLocked;
}

void CDoorComponent::SetLockId(const lockID aLockId)
{
	myLockId = aLockId;
}

void CDoorComponent::SnapShotDoorState()
{
	myResetToIsClosed = myIsClosed;
	myResetToIsLocked = myIsLocked;
	myResetToPosition = GetParent()->GetToWorldTransform();
}

void CDoorComponent::ResetToSnapShot()
{
	myIsClosed = myResetToIsClosed;
	myIsLocked = myResetToIsLocked;
	GetParent()->SetWorldTransformation(myResetToPosition);
	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}

void CDoorComponent::SetNetworkID(const unsigned char aNetworkID)
{
	myNetworkID = aNetworkID;
}

void CDoorComponent::Update(const CU::Time & aDeltaTime)
{
	if (myIsClosed == false)
	{
		float distance = abs(GetParent()->GetLocalTransform().GetPosition().y - myOriginPosition.y);
		//DL_PRINT("%f", lol);
		if (distance < myMoveDistance)
		{
			CU::Vector3f move;
			move = GetParent()->GetLocalTransform().GetPosition();
			move.y -= myMoveSpeed;
			GetParent()->GetLocalTransform().SetPosition(move);
		}
		//GetParent()->SetWorldPosition(CU::Vector3f(9999, 9999, 9999));
		SComponentMessageData data;
		GetParent()->NotifyComponents(eComponentMessageType::eMoving, data);
		return;
	}
	else
	{
		if (GetParent()->GetLocalTransform().GetPosition().y < myOriginPosition.y)
		{
			CU::Vector3f move;
			move = GetParent()->GetLocalTransform().GetPosition();
			move.y += myMoveSpeed;
			GetParent()->GetLocalTransform().SetPosition(move);
			//GetParent()->SetWorldPosition(myResetToPosition.GetPosition());
			GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		}
	}

	if (myAddToCheckpoint == true)
	{
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CAddToCheckPointResetList(GetParent()));
		myAddToCheckpoint = false;
	}
}

const CU::Vector2f CDoorComponent::GetOpenDirection() const
{
	return myOpenDirection;
}

const float CDoorComponent::GetMoveSpeed() const
{
	return myMoveSpeed;
}

const float CDoorComponent::GetMoveDistance() const
{
	return myMoveDistance;
}

const lockID CDoorComponent::GetLockId() const
{
	return myLockId;
}

const bool CDoorComponent::GetIsClosed() const
{
	return myIsClosed;
}

const bool CDoorComponent::GetIsLocked() const
{
	return myIsLocked;
}

const unsigned char CDoorComponent::GetNetworkID() const
{
	return myNetworkID;
}

eMessageReturn CDoorComponent::DoEvent(const CSetAsNewCheckPointMessage & aSetAsNewCheckPointMessage)
{
	SnapShotDoorState();
	myAddToCheckpoint = true;
	return eMessageReturn::eContinue;
}

void CDoorComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eCheckPointReset:
		ResetToSnapShot();
		DL_PRINT("Resetto the doórretto");
		break;
	case eComponentMessageType::eObjectDone:
			myOriginPosition = GetParent()->GetWorldPosition();
			//SnapShotDoorState();
			Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eSetNewCheckPoint);
			Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eResetToCheckPointMessage);
		break;
	case eComponentMessageType::eOnTriggerEnter:
		if (aMessageData.myGameObject != GetParent() && aMessageData.myComponent->GetParent() == CPollingStation::GetInstance()->GetPlayerObject())
		{
			CU::Vector3f pas = GetParent()->GetWorldPosition();
			if (myIsLocked == false)
			{
				SetIsClosed(false);
				CNetworkMessage_DoorMessage* doorMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_DoorMessage>(ID_ALL);
				doorMessage->SetDoorAction(eDoorAction::eOpen);
				doorMessage->SetID(myNetworkID);
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(doorMessage));
				return;
			}
			for (unsigned int i = 0; i < CPollingStation::GetInstance()->GetKeys().Size(); ++i)
			{
				if (CPollingStation::GetInstance()->GetKeys()[i] == myLockId)
				{
					myIsLocked = false;
					SetIsClosed(false);
					CNetworkMessage_DoorMessage* lockdoorMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_DoorMessage>(ID_ALL);
					lockdoorMessage->SetDoorAction(eDoorAction::eUnlock);
					lockdoorMessage->SetID(myNetworkID);
					Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(lockdoorMessage));

					CNetworkMessage_DoorMessage* opendoorMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_DoorMessage>(ID_ALL);
					opendoorMessage->SetDoorAction(eDoorAction::eOpen);
					opendoorMessage->SetID(myNetworkID);
					Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(opendoorMessage));
					if (myShouldReset == true)
					{
						Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CAddToCheckPointResetList(GetParent()));
					}
					break;
				}
			}
		}
		break;
	default:
		break;
	}
}

eMessageReturn CDoorComponent::DoEvent(const CResetToCheckPointMessage& aResetToCheckPointMessage)
{
	if (myShouldReset == true)
	{
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CAddToCheckPointResetList(GetParent()));
		DL_PRINT("Add door to checkpoint %u", GetParent()->GetId());
		//SnapShotDoorState();
	}
	return eMessageReturn::eContinue;
}
