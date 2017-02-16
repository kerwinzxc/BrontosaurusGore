#include "stdafx.h"
#include "WeaponFactory.h"
#include "WeaponData.h"
#include "Weapon.h"

WeaponFactory::WeaponFactory()
{
	myWeaponDataList.Init(10);
}


WeaponFactory::~WeaponFactory()
{
}

void WeaponFactory::LoadWeapons()
{
	//Do inläsning här plz;
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