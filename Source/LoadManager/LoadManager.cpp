#include "stdafx.h"
#include "LoadManager.h"
#include "DL_Debug.h"
#include "LoadObject.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadModel.h"
#include "LoadCamera.h"
#include "LoadColliderComponent.h"
#include "LoadSoundComponent.h"
#include "LoadScriptComponent.h"
#include "LoadFireComponent.h"
#include "LoadHealthComponent.h"
#include "LoadArmorComponent.h"
#include "LoadAmmoComponent.h"
#include "LoadPointLightComponent.h"
#include "LoadEnemy.h"
#include "LoadNetworkComponent.h"
#include "LoadWeaponSystemComponent.h"
#include "LoadMovementComponent.h"
#include "LoadInputComponent.h"
#include "LoadCollider.h"
#include "LoadDoorComponent.h"
#include "LoadKeyPickupComponent.h"
#include "LoadWeaponPickupComponent.h"

#include "LoadAmmoPickupComponent.h"
#include "LoadHealthPackComponent.h"
#include "LoadParticleEmitterComponent.h"
#include "LoadCheckPointComponent.h"
#include "LoadDamageOnCollisionComponent.h"
#include "LoadHighlightComponent.h"
#include "LoadTumbleweed.h"

LoadManager* LoadManager::ourInstance = nullptr;

void LoadManager::CreateInstance(CPlayState& aPlayState, CScene& aScene)
{
	assert(ourInstance == nullptr && "Load manager already created");
	ourInstance = new LoadManager(aPlayState, aScene);
}

LoadManager* LoadManager::GetInstance()
{
	return ourInstance;
}

void LoadManager::DestroyInstance()
{
	assert(ourInstance != nullptr && "Load manager not created (is NULL)");
	SAFE_DELETE(ourInstance);
}

CScene& LoadManager::GetCurrentScene() const
{
	return myCurrentScene;
}

CPlayState& LoadManager::GetCurrentPLaystate() const
{
	return myCurrentPlaystate;
}

void LoadManager::RegisterFunctions()
{
	KLoader::CKevinLoader &loader = KLoader::CKevinLoader::GetInstance();

	loader.RegisterObjectLoadFunction(LoadObject);
	loader.RegisterObjectLinkFunction(LinkObject);

	loader.RegisterComponentLoadFunction("MeshFilter", LoadMeshFilter);
	loader.RegisterComponentLoadFunction("Enemy", ClientLoadEnemy);
	loader.RegisterComponentLoadFunction("Camera", LoadCamera);
	loader.RegisterComponentLoadFunction("NetworkComponent", LoadClientNetworkComponent);
	loader.RegisterComponentLoadFunction("HealthComponent", LoadHealthComponent);
	loader.RegisterComponentLoadFunction("WeaponSystemComponent", LoadWeaponSystemComponent);
	loader.RegisterComponentLoadFunction("AmmoComponent", LoadAmmoComponent);
	loader.RegisterComponentLoadFunction("MovementComponent", LoadMovementComponent);
	loader.RegisterComponentLoadFunction("InputComponent", LoadInputComponent);
	loader.RegisterComponentLoadFunction("AmmoPickupComponent", LoadAmmoPickupComponent);
	loader.RegisterComponentLoadFunction("HealthPackComponent", LoadHealthPackComponent);
	loader.RegisterComponentLoadFunction("ArmorPickupComponent", LoadArmorPackComponent);
	loader.RegisterComponentLoadFunction("KeyPickupComponent", LoadKeyPickupComponent);
	loader.RegisterComponentLoadFunction("WeaponPickupComponent",LoadWeaponPickupkComponent);
	loader.RegisterComponentLoadFunction("ParticleEmitterComponent", LoadParticleEmitterComponent);
	loader.RegisterComponentLoadFunction("CheckpointComponent", LoadCheckpointComponent);
	loader.RegisterComponentLoadFunction("BoxCollider", LoadBoxCollider);
	loader.RegisterComponentLoadFunction("SphereCollider", LoadSphereCollider);
	loader.RegisterComponentLoadFunction("CapsuleCollider", LoadCapsuleCollider);
	loader.RegisterComponentLoadFunction("MeshCollider", LoadMeshCollider);
	loader.RegisterComponentLoadFunction("Rigidbody", LoadRigidBody);
	loader.RegisterComponentLoadFunction("CharacterController", LoadCharacterController);
	loader.RegisterComponentLoadFunction("ScriptComponent", LoadScriptComponent);
	loader.RegisterComponentLoadFunction("DamageOnCollisionComponent", LoadDamageOnCollisionComponent);
	loader.RegisterComponentLoadFunction("DoorComponent", LoadDoorComponent);
	loader.RegisterComponentLoadFunction("HighlightComponent",LoadHighlightComponent);
	loader.RegisterComponentLoadFunction("ImpController", ClientLoadImp);
	loader.RegisterComponentLoadFunction("RevenantController", ClientLoadRevenant);
	loader.RegisterComponentLoadFunction("PinkyController", ClientLoadPinky);
	loader.RegisterComponentLoadFunction("Tumbleweed", LoadTumbleweed);

	//loader.RegisterComponentLoadFunction("CircleCollider", LoadCircleCollider);
	//loader.RegisterComponentLoadFunction("SoundComponent", LoadSoundComponent);
	//loader.RegisterComponentLoadFunction("FireComponent", LoadFireComponent);
	//loader.RegisterComponentLoadFunction("PointLightComponent", LoadPointLightComponent);
}

LoadManager::LoadManager(CPlayState& aPlayState, CScene& aScene)
	: myCurrentPlaystate(aPlayState)
	, myCurrentScene(aScene)
{
	RegisterFunctions();
}


LoadManager::~LoadManager()
{
}
