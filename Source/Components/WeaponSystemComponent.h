#pragma once
#include "Component.h"
#include "WeaponDefines.h"

class CWeapon;
class CWeaponFactory;
struct SAmmoData;
class CTextInstance;
class CWeaponSystemComponent : public CComponent
{
public:
	CWeaponSystemComponent(CWeaponFactory& aWeaponFactoryThatIsGoingToBEHardToObtain);
	~CWeaponSystemComponent();
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	void Update(float aDelta);
	void GiveWeapon(const char* aWeaponName);
	void AddWeapon(CWeapon* aWeapon, SAmmoData* aTemporaryAmmoData);
private:
	void HandleKeyPressed(const SComponentMessageData& aMessageData);
	void HandleKeyReleased(const SComponentMessageData& aMessageData);

	CU::GrowingArray<CWeapon*> myWeapons;
	CU::GrowingArray<SAmmoData*> myTemporaryAmmoDataList;
	CWeaponFactory* WeaponFactoryPointer;
	CTextInstance* myActiveWeaponAmmoLeftText;
	unsigned short myActiveWeaponIndex;
	bool myIsShooting;
};

