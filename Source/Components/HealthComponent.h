#pragma once
#include "HealthPoint.h"
#include "Component.h"

class CHealthComponent : public CComponent
{
public:
	CHealthComponent(unsigned int aNetworkID);
	~CHealthComponent();

	void SetMaxHealth(const healthPoint aHealthPointValue);
	void SetHealth(const healthPoint aHealthPointValue);

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;
private:
	void TakeDamage(const healthPoint aDamage);
	void Heal(const healthPoint aHealAmount);
	
private:
	unsigned int myNetworkID;

	healthPoint myMaxHeath;
	healthPoint myCurrentHealth;

	bool myIsAlive;
};

