#include "stdafx.h"
#include "WeaponFactory.h"
#include "WeaponData.h"
#include "Weapon.h"
#include "ProjectileData.h"
#include "AmmoData.h"
#include "../CommonUtilities/JsonValue.h"
#include "ProjectileFactory.h"
#include "WeaponSystemComponent.h"
#include "ModelComponent.h"
#include "ModelComponentManager.h"
#include "GameObjectManager.h"
#include "ExplosionData.h"

CWeaponFactory::CWeaponFactory()
{
	myWeaponDataList.Init(10);
	myAmmoDataList.Init(10);
	myWeaponList.Init(200);
	myGameObjectManagerPointer = nullptr;
	myModelComponentManagerPointer = nullptr;
}


CWeaponFactory::~CWeaponFactory()
{
	for(unsigned int i = 0; i < myWeaponDataList.Size(); i++)
	{
		if(myWeaponDataList[i]->projectileData->explosionData != nullptr)
		{
			SAFE_DELETE(myWeaponDataList[i]->projectileData->explosionData);
		}
		SAFE_DELETE(myWeaponDataList[i]->projectileData);
	}
	myWeaponDataList.DeleteAll();
	myAmmoDataList.DeleteAll();
	myWeaponDataList.Destroy();
	myAmmoDataList.Destroy();
	myWeaponList.DeleteAll();
	myWeaponList.Destroy();
}

void CWeaponFactory::LoadWeapons()
{
	CU::CJsonValue weaponBluePrint;
	std::string filePath = "Json/Weapons/WeaponStats.json";
	const std::string& errorString = weaponBluePrint.Parse(filePath);
	CU::CJsonValue levelsArray = weaponBluePrint.at("Weapons");

	for (int i = 0; i < levelsArray.Size(); ++i)
	{
		SWeaponData* newWeaponData = new SWeaponData();
		SProjectileData* newProjectileData = new SProjectileData();
		SAmmoData* newAmmoData = new SAmmoData();
		newWeaponData->projectileData = newProjectileData;
		std::string weaponName = levelsArray[i].at("WeaponName").GetString();
		newWeaponData->name = weaponName.c_str();
		std::string weaponModelFilePath = levelsArray[i].at("WeaponModel").GetString();
		newWeaponData->modelFilePath = weaponModelFilePath.c_str();
		newWeaponData->modelPositionX = levelsArray[i].at("ModelPositionX").GetFloat();
		newWeaponData->modelPositionY = levelsArray[i].at("ModelPositionY").GetFloat();
		newWeaponData->modelPositionZ = levelsArray[i].at("ModelPositionZ").GetFloat();
		newWeaponData->shootPositionX = levelsArray[i].at("ShootPositionX").GetFloat();
		newWeaponData->shootPositionY = levelsArray[i].at("ShootPositionY").GetFloat();
		newWeaponData->shootPositionZ = levelsArray[i].at("ShootPositionZ").GetFloat();
		newWeaponData->fireRate = levelsArray[i].at("FireRate").GetFloat();
		newWeaponData->randomSpreadAngle = levelsArray[i].at("RandomSpreadAngle").GetInt();
		newWeaponData->projectilesFiredPerShot = levelsArray[i].at("ProjectilesFiredPerShot").GetInt();

		newProjectileData->projectileModelFilePath = levelsArray[i].at("ProjectileModel").GetString().c_str();
		newProjectileData->damage = static_cast<healthPoint>(levelsArray[i].at("Damage").GetFloat());
		newProjectileData->movementSpeed = levelsArray[i].at("ProjectileMovementSpeed").GetFloat();
		newProjectileData->maximumTravelRange = levelsArray[i].at("MaximumTravelRange").GetFloat();
		newProjectileData->shouldExplodeOnImpact = levelsArray[i].at("ShouldProjectilesExplode").GetBool();
		newProjectileData->shouldRayCast = levelsArray[i].at("ShouldRayCast").GetBool();

		if(newProjectileData->shouldExplodeOnImpact == true)
		{
			SExplosionData* newExplosionData = new SExplosionData();
			newExplosionData->radius = levelsArray[i].at("ExplosionRange").GetFloat();
			newExplosionData->knockBackForce = levelsArray[i].at("ExplosionKnockBackForce").GetFloat();
			newExplosionData->damage = static_cast<healthPoint>(levelsArray[i].at("ExplosionDamage").GetFloat());
			newProjectileData->explosionData = newExplosionData;
		}
		else
		{
			newProjectileData->explosionData = nullptr;
		}

		newAmmoData->maxAmmo = levelsArray[i].at("MaxAmmoAmount").GetInt();
		newAmmoData->ammoForWeaponName = newWeaponData->name;

		myWeaponDataList.Add(newWeaponData);
		myAmmoDataList.Add(newAmmoData);
		if(CProjectileFactory::GetInstance() != nullptr)
		{
			CProjectileFactory::GetInstance()->CreateNewProjectileBuffer(newProjectileData->projectileModelFilePath, 500);	
		}
	}
}
void CWeaponFactory::CreateWeapon(const char* aWeaponName, CGameObject* aObjectToGiveAWeaponTo)
{
	for(unsigned short i = 0; i < myWeaponDataList.Size(); i++)
	{
		if(myWeaponDataList[i]->name == aWeaponName)
		{
			CWeapon* newWeapon = new CWeapon(myWeaponDataList[i], myPhysicsScene);
			SAmmoData* newAmmoData = myAmmoDataList[i];
			SComponentMessageData newAmmoTypeMessage;
			newAmmoTypeMessage.myAmmoData = newAmmoData;
			aObjectToGiveAWeaponTo->NotifyOnlyComponents(eComponentMessageType::eAddNewAmmoType, newAmmoTypeMessage);
			SComponentMessageData newWeaponMessage;
			newWeaponMessage.myWeapon = newWeapon;
			aObjectToGiveAWeaponTo->NotifyOnlyComponents(eComponentMessageType::eWeaponFactoryGiveWeaponToWeaponSystem, newWeaponMessage);
			myWeaponList.Add(newWeapon);
			return;
		}
	}
	DL_PRINT_WARNING("Couldn't find what weapon to give. Check spelling and/or yell at Marcus. The weapons name was %s", aWeaponName);
	return;
}

void CWeaponFactory::CreateWeapon(const char* aWeaponName, CWeaponSystemComponent* aWeaponSystemToGiveAWeaponTo)
{
	for (unsigned short i = 0; i < myWeaponDataList.Size(); i++)
	{
		if (myWeaponDataList[i]->name == aWeaponName)
		{
			CWeapon* newWeapon = new CWeapon(myWeaponDataList[i], myPhysicsScene);
			SAmmoData* newAmmoData = myAmmoDataList[i];
			aWeaponSystemToGiveAWeaponTo->AddWeapon(newWeapon, newAmmoData);
			myWeaponList.Add(newWeapon);
			return;
		}
	}
	DL_PRINT("Couldn't find what weapon to give. Check spelling and/or yell at Marcus. The weapons name was %s", aWeaponName);
	return;
}

void CWeaponFactory::Init(CGameObjectManager* aGameObjectPointer, CModelComponentManager* aModelComponentManager, Physics::CPhysicsScene* aPhysicsScene)
{
	myGameObjectManagerPointer = aGameObjectPointer; 
	myModelComponentManagerPointer = aModelComponentManager;
	myPhysicsScene = aPhysicsScene;
}

void CWeaponFactory::MakeWeaponModel(CGameObject* aOwner, CWeapon* aWeapon)
{
	SComponentQuestionData cameraObjectQuestionData;
	if(aOwner->AskComponents(eComponentQuestionType::eGetCameraObject, cameraObjectQuestionData))
	{
		CModelComponent* newWeaponModelComponent = myModelComponentManagerPointer->CreateComponent(aWeapon->GetData()->modelFilePath.c_str());
		CGameObject* newWeaponObject = myGameObjectManagerPointer->CreateGameObject();
		newWeaponObject->AddComponent(newWeaponModelComponent);
		cameraObjectQuestionData.myGameObject->AddComponent(newWeaponObject);
		aWeapon->SetWeaponObject(newWeaponObject);
		newWeaponObject->Move(CU::Vector3f(aWeapon->GetData()->modelPositionX, aWeapon->GetData()->modelPositionY, aWeapon->GetData()->modelPositionZ));
		aWeapon->SetModelVisibility(false);
	}
}