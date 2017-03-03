#pragma once
struct SWeaponData;
struct SAmmoData;
class CWeapon;
class CGameObject;
class CWeaponSystemComponent;
class CWeaponFactory
{
public:
	CWeaponFactory();
	~CWeaponFactory();

	void LoadWeapons();
	void CreateWeapon(const char* aWeaponName, CGameObject* aObjectToGiveAWeaponTo);
	void CreateWeapon(const char* aWeaponName, CWeaponSystemComponent* aWeaponSystemToGiveAWeaponTo);
private:
	CU::GrowingArray<SWeaponData*> myWeaponDataList;
	CU::GrowingArray<SAmmoData*> myAmmoDataList;
};

