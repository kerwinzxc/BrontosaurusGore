#include "stdafx.h"
#include "LoadWeaponSystemComponent.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "WeaponSystemComponent.h"
#include "WeaponSystemManager.h"
#include "../Game/PlayState.h"
#include "ComponentMessage.h"

int LoadWeaponSystemComponent(KLoader::SLoadedComponentData someData)
{
	GET_LOADMANAGER(loadManager);
	CWeaponSystemManager* weaponSystemManager = loadManager.GetCurrentPLaystate().GetCWeaponSystemManager();
	if(weaponSystemManager != nullptr)
	{
		CWeaponSystemComponent* newWeaponSystemComponent = weaponSystemManager->CreateAndRegisterComponent();
		if (someData.myData.HasKey("Weapons") == false)
		{
			DL_PRINT("WeaponsSystemComponent empty got no weapons added");
			return 0;
		}
		else
		{

			CU::CJsonValue levelsArray = someData.myData.at("Weapons");

			if (!(levelsArray.IsArray() && levelsArray.Size() > 0))
			{
				DL_PRINT("WeaponsSystemComponent missing values");
			}

			for (unsigned short i = 0; i < levelsArray.Size(); i++)
			{
				std::string newWeaponName = levelsArray[i].GetString().c_str();
				newWeaponSystemComponent->GiveWeapon(newWeaponName.c_str());
			}
		}
		return newWeaponSystemComponent->GetId();
	}
	DL_PRINT("Tried loading a weapon system component without having the manager created.");
	return NULL_COMPONENT;
}

int LoadWeaponSystemServerComponent(KLoader::SLoadedComponentData someData)
{
	GET_SERVERLOADMANAGER(loadManager);
	CWeaponSystemManager* weaponSystemManager = loadManager.GetCurrentGameServer().GetCWeaponSystemManager();
	if (weaponSystemManager != nullptr)
	{
		CWeaponSystemComponent* newWeaponSystemComponent = weaponSystemManager->CreateAndRegisterComponent();
		if (someData.myData.HasKey("Weapons") == false)
		{
			DL_PRINT("WeaponsSystemComponent empty got no weapons added");
			return 0;
		}
		else
		{

			CU::CJsonValue levelsArray = someData.myData.at("Weapons");

			if (!(levelsArray.IsArray() && levelsArray.Size() > 0))
			{
				DL_PRINT("WeaponsSystemComponent missing values");
			}

			for (unsigned short i = 0; i < levelsArray.Size(); i++)
			{
				std::string newWeaponName = levelsArray[i].GetString().c_str();
				newWeaponSystemComponent->GiveWeapon(newWeaponName.c_str());
			}
		}
		return newWeaponSystemComponent->GetId();
	}
	DL_PRINT("Tried loading a weapon system component without having the manager created.");
	return NULL_COMPONENT;
}
