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

	bool WeaponIndexValid() const;
	void Update(float aDelta);
	void GiveWeapon(const char* aWeaponName);
	void AddWeapon(CWeapon* aWeapon, SAmmoData* aTemporaryAmmoData);
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
	void ChangeWeaponCallback(unsigned aIndex);
private:
	void HandleKeyPressed(const SComponentMessageData& aMessageData);
	void HandleKeyReleased(const SComponentMessageData& aMessageData);

	void ChangeWeapon(unsigned int aIndex);
	void ChangeWeapon2(unsigned int aIndex);
	void ChangeWeaponLocal(unsigned int aIndex);
	bool CanChangeWeapon(unsigned int aIndex);
private:

	bool CheckIfAlreadyHaveWeapon(const char* aWeaponName);
	CU::Vector3f myShouldShootThisDirectionForCosmeticShotNextFrame;
	CU::GrowingArray<CWeapon*> myWeapons;
	CU::GrowingArray<SAmmoData*> myTemporaryAmmoDataList;
	CWeaponFactory* WeaponFactoryPointer;
	unsigned short myActiveWeaponIndex;
	bool myIsShooting;
	bool myIsActive;
	bool myIsChanginWeapon;
	bool myShouldShootACosmeticShotNextFrame;
};

