#include "stdafx.h"
#include "PinkyClientDamageHandler.h"
#include "PollingStation.h"

CPinkyClientDamageHandler::CPinkyClientDamageHandler()
{
	myCountDown = 0.0f;
	myCollidedObjects.Init(10);
	myIsActive = true;
}


CPinkyClientDamageHandler::~CPinkyClientDamageHandler()
{
}

void CPinkyClientDamageHandler::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eUpdatePinky:
	{
		myCountDown -= aMessageData.myFloat;
		if (myIsActive == true)
		{
			float playerDistance = CU::Vector3f(GetParent()->GetWorldPosition() - CPollingStation::GetInstance()->GetPlayerObject()->GetWorldPosition()).Length();
			if(playerDistance < 2.0f)
			{
				if (myCountDown <= 0.0f)
				{
					myCountDown = 1.5f;
					SComponentMessageData damageData;
					damageData.myInt = 50.0f;
					CPollingStation::GetInstance()->GetPlayerObject()->NotifyComponents(eComponentMessageType::eTakeDamage, damageData);
				}
			}
		}
		break;
	}
	case eComponentMessageType::eDied:
	{
		myIsActive = false;
	}
	break;
	case eComponentMessageType::eCheckPointReset:
	{
		myIsActive = true;
	}
	break;
	default:
		break;
	}
}