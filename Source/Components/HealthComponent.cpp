#include "stdafx.h"
#include "HealthComponent.h"


HealthComponent::HealthComponent(healthPoint aMaxHealth)
{
	myMaxHeath = aMaxHealth;
	myCurrentHealth = myMaxHeath;
	myIsAlive = true;
}


HealthComponent::~HealthComponent()
{
}

void HealthComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
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
	default:
		break;
	}
}
void HealthComponent::Destroy()
{

}

void HealthComponent::TakeDamage(const healthPoint aDamage)
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
		GetParent()->NotifyComponents(eComponentMessageType::eDied, SComponentMessageData());
	}
	else
	{
		myCurrentHealth -= aDamage;
	}
}
void HealthComponent::Heal(const healthPoint aHealAmount)
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