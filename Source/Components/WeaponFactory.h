#pragma once
struct SWeaponData;
struct SAmmoData;
class CWeapon;
class CGameObject;
class CWeaponFactory
{
public:
	CWeaponFactory();
	~CWeaponFactory();

	void LoadWeapons();
	void CreateWeapon(const char* aWeaponName, CGameObject* aObjectToGiveAWeaponTo);
	
private:
	CU::GrowingArray<SWeaponData*> myWeaponDataList;
	CU::GrowingArray<SAmmoData*> myAmmoDataList;
};

