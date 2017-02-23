#pragma once
struct WeaponData;
struct AmmoData;
class Weapon;
class WeaponFactory
{
public:
	WeaponFactory();
	~WeaponFactory();

	void LoadWeapons();
	Weapon* CreateWeapon(const char* aWeaponName);
	
private:
	CU::GrowingArray<WeaponData*> myWeaponDataList;
	CU::GrowingArray<AmmoData*> myAmmoDataList;
};

