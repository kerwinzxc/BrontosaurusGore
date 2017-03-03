#pragma once
#include "Component.h"
#include "WeaponDefines.h"

class CWeapon;
class CWeaponFactory;
struct SAmmoData;
class CWeaponSystemComponent : public CComponent
{
public:
	CWeaponSystemComponent(CWeaponFactory& aWeaponFactoryThatIsGoingToBEHardToObtain);
	~CWeaponSystemComponent();
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	void Update(float aDelta);
	void GiveWepon(const char* aWeaponName);
	void AddWeapon(CWeapon* aWeapon, SAmmoData* aTemporaryAmmoData);
private:
	void HandleKeyPressed(const SComponentMessageData& aMessageData);
	void HandleKeyReleased(const SComponentMessageData& aMessageData);

	CU::GrowingArray<CWeapon*> myWeapons;
	CU::GrowingArray<SAmmoData*> myTemporaryAmmoDataList;
	CWeaponFactory* WeaponFactoryPointer;
	unsigned short myActiveWeaponIndex;
	bool myIsShooting;
};

