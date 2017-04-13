#include "stdafx.h"
#include "PinkyClientDamageHandler.h"


CPinkyClientDamageHandler::CPinkyClientDamageHandler()
{
	myCountDown = 0.0f;
}


CPinkyClientDamageHandler::~CPinkyClientDamageHandler()
{
}

void CPinkyClientDamageHandler::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnCollisionEnter:
	{
		if(myCountDown <= 0.0f)
		{
			myCountDown = 1.5f;
			CGameObject* hitObject = aMessageData.myComponent->GetParent();
			SComponentMessageData damageData;
			damageData.myInt = 50.0f;
			hitObject->NotifyComponents(eComponentMessageType::eTakeDamage, damageData);
		}
		break;
	}
	case eComponentMessageType::eUpdatePinky:
	{
		myCountDown -= aMessageData.myFloat;
		break;
	}
	default:
		break;
	}
}