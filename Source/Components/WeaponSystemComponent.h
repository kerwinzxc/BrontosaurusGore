#pragma once
#include "Component.h"
#include "WeaponDefines.h"

class CWeapon;
class CWeaponFactory;
class CWeaponSystemComponent : public CComponent
{
public:
	CWeaponSystemComponent(CWeaponFactory& aWeaponFactoryThatIsGoingToBEHardToObtain);
	~CWeaponSystemComponent();
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;

	void Update(float aDelta);

private:
	CU::GrowingArray<CWeapon*> myWeapons;
	CWeaponFactory* WeaponFactoryPointer;
	unsigned short myActiveWeaponIndex;

};

