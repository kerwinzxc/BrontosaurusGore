#include "stdafx.h"
#include "WeaponFactory.h"
#include "WeaponData.h"
#include "Weapon.h"
#include "ProjectileData.h"
#include "AmmoData.h"
#include "../CommonUtilities/JsonValue.h"

WeaponFactory::WeaponFactory()
{
	myWeaponDataList.Init(10);
	myAmmoDataList.Init(10);
}


WeaponFactory::~WeaponFactory()
{
}

void WeaponFactory::LoadWeapons()
{
	CU::CJsonValue weaponBluePrint;
	std::string filePath = "Json/Weapons/WeaponStats.json";
	const std::string& errorString = weaponBluePrint.Parse(filePath);
	CU::CJsonValue levelsArray = weaponBluePrint.at("Weapons");

	for (int i = 0; i < levelsArray.Size(); ++i)
	{
		WeaponData* newWeaponData = new WeaponData();
		ProjectileData* newProjectileData = new ProjectileData();
		AmmoData* newAmmoData = new AmmoData();
		newWeaponData->projectileData = newProjectileData;
		newWeaponData->name = levelsArray[i].at("WeaponName").GetString().c_str();
		newWeaponData->fireRate = levelsArray[i].at("FireRate").GetFloat();
		newProjectileData->damage = static_cast<healthPoint>(levelsArray[i].at("Damage").GetFloat());
		newProjectileData->movementSpeed = levelsArray[i].at("ProjectileMovementSpeed").GetFloat();
		newAmmoData->maxAmmo = levelsArray[i].at("ProjectileMovementSpeed").GetInt();
		newAmmoData->ammoForWeaponName = newWeaponData->name;
		myWeaponDataList.Add(newWeaponData);
		myAmmoDataList.Add(newAmmoData);
	}
}
Weapon* WeaponFactory::CreateWeapon(const char* aWeaponName)
{
	for(unsigned short i = 0; i < myWeaponDataList.Size(); i++)
	{
		if(myWeaponDataList[i]->name == aWeaponName)
		{
			Weapon* newWeapon = new Weapon(myWeaponDataList[i]);
			return newWeapon;
		}
	}
	DL_PRINT("Couldn't find what weapon to give. Check spelling and/or yell at Marcus. The weapons name was %s", aWeaponName);
	return nullptr;
}