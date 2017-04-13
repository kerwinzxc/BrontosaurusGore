#include "stdafx.h"
#include "PinkyClientDamageHandler.h"


CPinkyClientDamageHandler::CPinkyClientDamageHandler()
{
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
		CGameObject* hitObject = aMessageData.myComponent->GetParent();
		SComponentMessageData damageData;
		damageData.myInt = 75.0f;
		hitObject->NotifyComponents(eComponentMessageType::eTakeDamage, damageData);
		break;
	}
	default:
		break;
	}
}