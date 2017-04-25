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
#include "MarkerComponent.h"
#include "MarkerComponentManager.h"

CWeaponFactory::CWeaponFactory(): myPhysicsScene(nullptr)
{
	myWeaponDataList.Init(10);
	myAmmoDataList.Init(10);
	myWeaponList.Init(200);
	myGameObjectManagerPointer = nullptr;
	myModelComponentManagerPointer = nullptr;
	myHaveGivenPlayerMarkerThingie = false;
	
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
	LoadWeaponsFromJsonValue(levelsArray, true);
	CU::CJsonValue enemyWeaponBluePrint;
	filePath = "Json/Weapons/EnemyWeaponStats.json";
	const std::string& enemyErrorString = weaponBluePrint.Parse(filePath);
	CU::CJsonValue enemyLevelsArray = weaponBluePrint.at("Weapons");
	LoadWeaponsFromJsonValue(enemyLevelsArray, false);
}
unsigned short CWeaponFactory::CreateWeapon(const char* aWeaponName, CGameObject* aObjectToGiveAWeaponTo)
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
			return i;
		}
	}
	DL_PRINT_WARNING("Couldn't find what weapon to give. Check spelling and/or yell at Marcus. The weapons name was %s", aWeaponName);
	return 666;
}
void CWeaponFactory::CreateWeapon(unsigned short aWeaponIndex, CGameObject* aObjectToGiveAWeaponTo)
{
	if(myWeaponList.HasIndex(aWeaponIndex) == true)
	{
		CWeapon* newWeapon = new CWeapon(myWeaponDataList[aWeaponIndex], myPhysicsScene);
		SAmmoData* newAmmoData = myAmmoDataList[aWeaponIndex];
		SComponentMessageData newAmmoTypeMessage;
		newAmmoTypeMessage.myAmmoData = newAmmoData;
		aObjectToGiveAWeaponTo->NotifyOnlyComponents(eComponentMessageType::eAddNewAmmoType, newAmmoTypeMessage);
		SComponentMessageData newWeaponMessage;
		newWeaponMessage.myWeapon = newWeapon;
		aObjectToGiveAWeaponTo->NotifyOnlyComponents(eComponentMessageType::eWeaponFactoryGiveWeaponToWeaponSystem, newWeaponMessage);
		myWeaponList.Add(newWeapon);
		return;
	}
	DL_PRINT("Couldn't find what weapon to give. Check spelling and/or yell at Marcus. The weapons index was %u", aWeaponIndex);
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

		std::string playerHandsPath = aWeapon->GetData()->modelFilePath;
		size_t pos = playerHandsPath.find_last_of('/');
		playerHandsPath.insert(pos + 2, "_WeaponPlayer_01");

		if(myHaveGivenPlayerMarkerThingie == false)
		{
			if(CMarkerComponentManager::GetInstance() != nullptr)
			{
				CModelComponent* newMarkerModel = myModelComponentManagerPointer->CreateComponent("Models/Meshes/M_NavigationArrow.fbx");
				newMarkerModel->SetIgnoreDepth(true);

				myHaveGivenPlayerMarkerThingie = true;
				CGameObject* markerObject = myGameObjectManagerPointer->CreateGameObject();
				markerObject->AddComponent(newMarkerModel);
				cameraObjectQuestionData.myGameObject->AddComponent(markerObject);
				markerObject->Move(CU::Vector3f(0.0f, 0.9f, 2.0f));
				CMarkerComponent* markerComponent = new CMarkerComponent();
				markerObject->AddComponent(markerComponent);
				if(CComponentManager::GetInstancePtr() != nullptr)
				{				
					CComponentManager::GetInstancePtr()->RegisterComponent(markerComponent);
				}
				else
				{
					DL_PRINT_WARNING("CCOmponentMnager not created when setting marker");
				}
				CMarkerComponentManager::GetInstance()->SetMarkerObject(markerComponent);
			}
			else
			{
				DL_PRINT_WARNING("MarkerComponentManager not created when setting marker");
			}
		}
		if (CU::CJsonValue::FileExists(playerHandsPath))
		{
			CModelComponent* handModelComponent = myModelComponentManagerPointer->CreateComponent(playerHandsPath);
			handModelComponent->SetIgnoreDepth(true);

			newWeaponObject->AddComponent(handModelComponent);
		}

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

void CWeaponFactory::LoadWeaponsFromJsonValue(const CU::CJsonValue& aJsonValue, bool aIsPlayerWeapon)
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
		newWeaponData->fireParticles.Init(1);
		if(aJsonValue[i].HasKey("FireParticles"))
		{
			for (int j = 0; j < aJsonValue[i]["FireParticles"].Size(); ++j)
			{
				newWeaponData->fireParticles.Add(aJsonValue[i]["FireParticles"][j].GetString());
			}
		}
		

		newProjectileData->projectileModelFilePath = aJsonValue[i].at("ProjectileModel").GetString().c_str();
		newProjectileData->damage = static_cast<healthPoint>(aJsonValue[i].at("Damage").GetFloat());
		newProjectileData->movementSpeed = aJsonValue[i].at("ProjectileMovementSpeed").GetFloat();
		newProjectileData->maximumTravelRange = aJsonValue[i].at("MaximumTravelRange").GetFloat();
		newProjectileData->shouldExplodeOnImpact = aJsonValue[i].at("ShouldProjectilesExplode").GetBool();
		newProjectileData->shouldRayCast = aJsonValue[i].at("ShouldRayCast").GetBool();
		newProjectileData->isPlayerFriendly = aIsPlayerWeapon;

		if (newProjectileData->shouldExplodeOnImpact == true)
		{
			SExplosionData* newExplosionData = new SExplosionData();
			newExplosionData->explosionParticles.Init(1);
			newExplosionData->radius = aJsonValue[i].at("ExplosionRange").GetFloat();
			newExplosionData->radius *= newExplosionData->radius;
			newExplosionData->knockBackForce = aJsonValue[i].at("ExplosionKnockBackForce").GetFloat();
			newExplosionData->damage = static_cast<healthPoint>(aJsonValue[i].at("ExplosionDamage").GetFloat());
			newProjectileData->explosionData = newExplosionData;
			newExplosionData->isPlayerFriendly = aIsPlayerWeapon;
			if(aJsonValue[i].HasKey("ExplosionParticles"))
			{
				for (int j = 0; j < aJsonValue[i]["ExplosionParticles"].Size(); ++j)
				{
					newExplosionData->explosionParticles.Add(aJsonValue[i]["ExplosionParticles"][j].GetString());
				}
			}
			
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

		newWeaponData->isMeleeWeapon = false;
		if (newWeaponData->name == "MeleeWeapon")
		{
			newWeaponData->isMeleeWeapon = true;
			newWeaponData->soundData.fire = "Player_Chainsaw_Loop_Start";
		}

		myWeaponDataList.Add(newWeaponData);
		myAmmoDataList.Add(newAmmoData);
		if (CProjectileFactory::GetInstance() != nullptr)
		{
			CProjectileFactory::GetInstance()->CreateNewProjectileBuffer(newProjectileData->projectileModelFilePath, 1000);
		}
	}
}

short CWeaponFactory::GetWeaponFactoryWeaponIndex(const char* aWeaponName)
{
	for (unsigned short i = 0; i < myWeaponDataList.Size(); i++)
	{
		if (myWeaponDataList[i]->name == aWeaponName)
		{
			return static_cast<unsigned short>(i);
		}
	}
	DL_PRINT("Couldn't find what weaponIndex to return. Check spelling and/or yell at Marcus. The weapons name was %s", aWeaponName);
	return -1;
}

SWeaponData* CWeaponFactory::GetWeaponDataFromIndex(unsigned int aIndex)
{
	return myWeaponDataList[aIndex];
}