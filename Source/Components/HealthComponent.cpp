#include "stdafx.h"
#include "HealthComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"

CHealthComponent::CHealthComponent()
{
	myMaxHeath = 0;
	myCurrentHealth = 0;
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

void CHealthComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eTakeDamage:
		if(myIsAlive == true)
		{
			TakeDamage(aMessageData.myInt);
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
	default:
		break;
	}
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
	if(myCurrentHealth - aDamage <= 0)
	{
		myCurrentHealth = 0;
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CAddToCheckPointResetList(GetParent()));
		GetParent()->NotifyComponents(eComponentMessageType::eDied, SComponentMessageData());

	}
	else
	{
		myCurrentHealth -= aDamage;
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