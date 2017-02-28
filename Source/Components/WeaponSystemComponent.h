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

	void Update(float aDelta);

private:
	void HandleKeyPressed(const SComponentMessageData& aMessageData);
	void HandleKeyReleased(const SComponentMessageData& aMessageData);

	CU::GrowingArray<CWeapon*> myWeapons;
	CWeaponFactory* WeaponFactoryPointer;
	unsigned short myActiveWeaponIndex;
	bool myIsShooting;
};

