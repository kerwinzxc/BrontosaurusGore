#pragma once
#include "Component.h"
#include "HealthPoint.h"

struct SDamageOnCollisonCollidedObjectData;
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
	void CreateEmptySlotForPassiveBuffer();
	void DealDamage(unsigned short aIndex);
	void AddToActiveList(CGameObject* aCollidedObject);
	void RemoveFromActiveList(CGameObject* aNonCollidingObject);
	unsigned short FindCollidedObjectData(CGameObject* aGameObject);
private:
	CU::GrowingArray<SDamageOnCollisonCollidedObjectData*> myActiveCollidedWithObjectsDataList;
	CU::GrowingArray<SDamageOnCollisonCollidedObjectData*> myPassiveCollidedWithObjectsDataList;
	float myDamageCooldown;
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