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

CWeaponFactory::CWeaponFactory(): myPhysicsScene(nullptr)
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
	LoadWeaponsFromJsonValue(levelsArray);
	CU::CJsonValue enemyWeaponBluePrint;
	filePath = "Json/Weapons/EnemyWeaponStats.json";
	const std::string& enemyErrorString = weaponBluePrint.Parse(filePath);
	CU::CJsonValue enemyLevelsArray = weaponBluePrint.at("Weapons");
	LoadWeaponsFromJsonValue(enemyLevelsArray);
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
		newWeaponModelComponent->SetIgnoreDepth(true);
		CGameObject* newWeaponObject = myGameObjectManagerPointer->CreateGameObject();
		newWeaponObject->AddComponent(newWeaponModelComponent);
		cameraObjectQuestionData.myGameObject->AddComponent(newWeaponObject);
		aWeapon->SetWeaponObject(newWeaponObject);
		newWeaponObject->Move(CU::Vector3f(aWeapon->GetData()->modelPositionX, aWeapon->GetData()->modelPositionY, aWeapon->GetData()->modelPositionZ));
		aWeapon->SetModelVisibility(false);
	}
}

SWeaponSoundData CWeaponFactory::GetSoundData(CU::CJsonValue aSoundData)
{
	SWeaponSoundData soundData;
	soundData.fire = aSoundData.at("Fire").GetString();
	soundData.reload = aSoundData.at("Reload").GetString();

	return soundData;
}

void CWeaponFactory::LoadWeaponsFromJsonValue(const CU::CJsonValue& aJsonValue)
{
	for (int i = 0; i < aJsonValue.Size(); ++i)
	{
		SWeaponData* newWeaponData = new SWeaponData();
		SProjectileData* newProjectileData = new SProjectileData();
		SAmmoData* newAmmoData = new SAmmoData();
		newWeaponData->projectileData = newProjectileData;
		std::string weaponName = aJsonValue[i].at("WeaponName").GetString();
		newWeaponData->name = weaponName.c_str();
		std::string weaponModelFilePath = aJsonValue[i].at("WeaponModel").GetString();
		newWeaponData->modelFilePath = weaponModelFilePath.c_str();
		newWeaponData->modelPositionX = aJsonValue[i].at("ModelPositionX").GetFloat();
		newWeaponData->modelPositionY = aJsonValue[i].at("ModelPositionY").GetFloat();
		newWeaponData->modelPositionZ = aJsonValue[i].at("ModelPositionZ").GetFloat();
		newWeaponData->shootPositionX = aJsonValue[i].at("ShootPositionX").GetFloat();
		newWeaponData->shootPositionY = aJsonValue[i].at("ShootPositionY").GetFloat();
		newWeaponData->shootPositionZ = aJsonValue[i].at("ShootPositionZ").GetFloat();
		newWeaponData->fireRate = aJsonValue[i].at("FireRate").GetFloat();
		newWeaponData->randomSpreadAngle = aJsonValue[i].at("RandomSpreadAngle").GetInt();
		newWeaponData->projectilesFiredPerShot = aJsonValue[i].at("ProjectilesFiredPerShot").GetInt();

		newProjectileData->projectileModelFilePath = aJsonValue[i].at("ProjectileModel").GetString().c_str();
		newProjectileData->damage = static_cast<healthPoint>(aJsonValue[i].at("Damage").GetFloat());
		newProjectileData->movementSpeed = aJsonValue[i].at("ProjectileMovementSpeed").GetFloat();
		newProjectileData->maximumTravelRange = aJsonValue[i].at("MaximumTravelRange").GetFloat();
		newProjectileData->shouldExplodeOnImpact = aJsonValue[i].at("ShouldProjectilesExplode").GetBool();
		newProjectileData->shouldRayCast = aJsonValue[i].at("ShouldRayCast").GetBool();

		if (newProjectileData->shouldExplodeOnImpact == true)
		{
			SExplosionData* newExplosionData = new SExplosionData();
			newExplosionData->radius = aJsonValue[i].at("ExplosionRange").GetFloat();
			newExplosionData->knockBackForce = aJsonValue[i].at("ExplosionKnockBackForce").GetFloat();
			newExplosionData->damage = static_cast<healthPoint>(aJsonValue[i].at("ExplosionDamage").GetFloat());
			newProjectileData->explosionData = newExplosionData;
		}
		else
		{
			newProjectileData->explosionData = nullptr;
		}

		if (aJsonValue[i].HasKey("Sounds") == true)
		{

			newWeaponData->soundData = GetSoundData(aJsonValue[i].at("Sounds"));
		}
		else
		{
			DL_PRINT_WARNING("Weapon \"%s\" doesn't define sound feedback.", weaponName.c_str());
		}

		newAmmoData->maxAmmo = aJsonValue[i].at("MaxAmmoAmount").GetInt();
		newAmmoData->ammoForWeaponName = newWeaponData->name;

		myWeaponDataList.Add(newWeaponData);
		myAmmoDataList.Add(newAmmoData);
		if (CProjectileFactory::GetInstance() != nullptr)
		{
			CProjectileFactory::GetInstance()->CreateNewProjectileBuffer(newProjectileData->projectileModelFilePath, 500);
		}
	}
}