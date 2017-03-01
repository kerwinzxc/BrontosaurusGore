#pragma once
#include "HealthPoint.h"
#include "Component.h"
class HealthComponent : public CComponent
{
public:
	HealthComponent(healthPoint aMaxHealth);
	~HealthComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;
private:
	void TakeDamage(const healthPoint aDamage);
	void Heal(const healthPoint aHealAmount);
	
private:
	healthPoint myMaxHeath;
	healthPoint myCurrentHealth;
	bool myIsAlive;
};

