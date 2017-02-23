#pragma once
struct WeaponData;
struct AmmoData;
class Weapon;
class CGameObject;
class WeaponFactory
{
public:
	WeaponFactory();
	~WeaponFactory();

	void LoadWeapons();
	void CreateWeapon(const char* aWeaponName, CGameObject* aObjectToGiveAWeaponTo);
	
private:
	CU::GrowingArray<WeaponData*> myWeaponDataList;
	CU::GrowingArray<AmmoData*> myAmmoDataList;
};

