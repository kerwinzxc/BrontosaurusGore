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
	for(unsigned int i = 0; i < myWeaponDataList.Size(); i++)
	{
		SAFE_DELETE(myWeaponDataList[i]->projectileData);
	}
	myWeaponDataList.DeleteAll();
	myAmmoDataList.DeleteAll();
	myWeaponDataList.Destroy();
	myAmmoDataList.Destroy();
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
		std::string weaponName = levelsArray[i].at("WeaponName").GetString();
		newWeaponData->name = weaponName.c_str();
		newWeaponData->fireRate = levelsArray[i].at("FireRate").GetFloat();
		newProjectileData->projectileModelFilePath = levelsArray[i].at("ProjectileModel").GetString().c_str();
		newProjectileData->damage = static_cast<healthPoint>(levelsArray[i].at("Damage").GetFloat());
		newProjectileData->movementSpeed = levelsArray[i].at("ProjectileMovementSpeed").GetFloat();
		newAmmoData->maxAmmo = levelsArray[i].at("MaxAmmoAmount").GetInt();
		newAmmoData->ammoForWeaponName = newWeaponData->name;
		myWeaponDataList.Add(newWeaponData);
		myAmmoDataList.Add(newAmmoData);
	}
}
void WeaponFactory::CreateWeapon(const char* aWeaponName, CGameObject* aObjectToGiveAWeaponTo)
{
	for(unsigned short i = 0; i < myWeaponDataList.Size(); i++)
	{
		if(myWeaponDataList[i]->name == aWeaponName)
		{
			Weapon* newWeapon = new Weapon(myWeaponDataList[i]);
			AmmoData* newAmmoData = myAmmoDataList[i];
			SComponentMessageData newAmmoTypeMessage;
			newAmmoTypeMessage.myAmmoData = newAmmoData;
			aObjectToGiveAWeaponTo->NotifyOnlyComponents(eComponentMessageType::eAddNewAmmoType, newAmmoTypeMessage);
			SComponentMessageData newWeaponMessage;
			newWeaponMessage.myWeapon = newWeapon;
			aObjectToGiveAWeaponTo->NotifyOnlyComponents(eComponentMessageType::eWeaponFactoryGiveWeaponToWeaponSystem, newWeaponMessage);
		}
	}
	DL_PRINT("Couldn't find what weapon to give. Check spelling and/or yell at Marcus. The weapons name was %s", aWeaponName);
	return;
}