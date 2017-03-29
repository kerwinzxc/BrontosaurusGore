#include "stdafx.h"
#include "HealthComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../TShared/NetworkMessage_TakeDamage.h"
#include "../TClient/ClientMessageManager.h"

CHealthComponent::CHealthComponent(unsigned int aNetworkID) : myNetworkID(aNetworkID)
{
	myMaxHeath = 0;
	myCurrentHealth = 0;
	myArmor = 0;
	myMaxArmor = 0;
	myIsAlive = true;
}


CHealthComponent::~CHealthComponent()
{
}

void CHealthComponent::SetMaxHealth(const healthPoint aHealthPointValue)
{
	myMaxHeath = aHealthPointValue;
}

void CHealthComponent::SetHealth(const healthPoint aHealthPointValue)
{
	myCurrentHealth = aHealthPointValue;
}

void CHealthComponent::SetMaxArmor(const armorPoint aArmorValue)
{
	myMaxArmor = aArmorValue;
}

void CHealthComponent::SetArmor(const armorPoint aArmorValue)
{
	myArmor = aArmorValue;
}

void CHealthComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eTakeDamage:
		if(myIsAlive == true)
		{
			TakeDamage(aMessageData.myInt);
			
			CNetworkMessage_TakeDamage* message = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_TakeDamage>(ID_ALL_BUT_ME);

			message->SetDamageTaken(aMessageData.myInt);
			message->SetID(myNetworkID);

			Postmaster::Threaded::CPostmaster::GetInstance().BroadcastLocal(new CSendNetworkMessageMessage(message));
		}
		break;
	case eComponentMessageType::eHeal:
		if (myIsAlive == true)
		{
			Heal(aMessageData.myInt);
		}
		break;
	case eComponentMessageType::eCheckPointReset:
	{
		myIsAlive = true;
		myCurrentHealth = myMaxHeath;
		break;	
	}
	case eComponentMessageType::eNetworkDoDamage:
		if (myIsAlive == true)
		{
			TakeDamage(aMessageData.myInt);
		}
		break;
	case eComponentMessageType::eAddArmor:
		if (myIsAlive == true)
		{
			AddArmor(aMessageData.myInt);
		}
		break;
	default:
		break;
	}
}
bool CHealthComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	switch (aQuestionType)
	{
	case eComponentQuestionType::eGetHealth:
		aQuestionData.myInt = myCurrentHealth;
		break;
	case eComponentQuestionType::eGetMaxHealth:
		aQuestionData.myInt = myMaxHeath;
		break;
	case eComponentQuestionType::eGetArmor:
		aQuestionData.myInt = myArmor;
		break;
	case eComponentQuestionType::eGetMaxArmor:
		aQuestionData.myInt = myMaxArmor;
		break;
	default:
		break;
	}
	return false;
}
void CHealthComponent::Destroy()
{

}

void CHealthComponent::TakeDamage(const healthPoint aDamage)
{
	if(aDamage < 0)
	{
		Heal(-aDamage);
		return;
	}
	GetParent()->NotifyComponents(eComponentMessageType::eTookDamage, SComponentMessageData());
	if((myCurrentHealth + myArmor) - aDamage <= 0)
	{
		myCurrentHealth = 0;
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CAddToCheckPointResetList(GetParent()));
		GetParent()->NotifyComponents(eComponentMessageType::eDied, SComponentMessageData());
		//kom du hit sätt en break point i model componets recive eDied!

	}
	else
	{
		healthPoint damage = aDamage;
		if (myArmor > 0)
		{
			if (myArmor - damage < 0)
			{
				damage -= myArmor;
				myArmor = 0;
			}
			else
			{
				myArmor -= damage;
				damage = 0;
			}
		}
		myCurrentHealth -= damage;
	}
}
void CHealthComponent::Heal(const healthPoint aHealAmount)
{
	if (aHealAmount < 0)
	{
		TakeDamage(-aHealAmount);
		return;
	}

	if (myCurrentHealth + aHealAmount > myMaxHeath)
	{
		myCurrentHealth = myMaxHeath;
	}
	else
	{
		myCurrentHealth += aHealAmount;
	}
}
void CHealthComponent::AddArmor(const armorPoint aArmorValue)
{
	myArmor += aArmorValue;
	if (myArmor > myMaxArmor)
	{
		myArmor = myMaxArmor;
	}
}