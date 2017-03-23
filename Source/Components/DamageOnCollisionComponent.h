#pragma once
#include "Component.h"
#include "HealthPoint.h"
class CDamageOnCollisionComponent : public CComponent
{
public:
	CDamageOnCollisionComponent();
	~CDamageOnCollisionComponent();

	void Update(float aDeltaTime);
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	inline void SetDamage(healthPoint aDamageAmount);
	inline void SetDamageCooldown(float aDamageCooldownDuration);
private:
	float myDamageCooldown;
	float myElapsedCooldownTime;
	healthPoint myDamage;
	
};

inline void CDamageOnCollisionComponent::SetDamage(healthPoint aDamageAmount)
{
	myDamage = aDamageAmount;
}
inline void CDamageOnCollisionComponent::SetDamageCooldown(float aDamageCooldownDuration)
{
	myDamageCooldown = aDamageCooldownDuration;
}