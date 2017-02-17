#pragma once
#include "Component.h"
#include "Defines.h"

class Weapon;
class WeaponFactory;
class WeaponSystemComponent : public CComponent
{
public:
	WeaponSystemComponent(WeaponFactory& aWeaponFactoryThatIsGoingToBEHardToObtain);
	~WeaponSystemComponent();
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;

	void Update(float aDelta);

private:
	CU::GrowingArray<Weapon*> myWeapons;
	WeaponFactory* WeaponFactoryPointer;
	unsigned short myActiveWeaponIndex;

};

