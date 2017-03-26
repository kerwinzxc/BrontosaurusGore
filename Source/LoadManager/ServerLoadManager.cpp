#include "stdafx.h"
#include "ServerLoadManager.h"
#include "LoadObject.h"
#include "LoadNetworkComponent.h"
#include "LoadMovementComponent.h"
#include "LoadEnemy.h"
#include "LoadWeaponSystemComponent.h"
#include "LoadDamageOnCollisionComponent.h"
#include "LoadImp.h"
#include "LoadRevenant.h"
#include "LoadPinky.h"
#include "LoadCollider.h"

CServerLoadManager* CServerLoadManager::ourInstance = nullptr;

void CServerLoadManager::CreateInstance(CGameServer& aGameServer)
{
	assert(ourInstance == nullptr && "Load manager already created");
	ourInstance = new CServerLoadManager(aGameServer);
}

CServerLoadManager* CServerLoadManager::GetInstance()
{
	return ourInstance;
}

void CServerLoadManager::DestroyInstance()
{
	assert(ourInstance != nullptr && "Load manager not created (is NULL)");
	SAFE_DELETE(ourInstance);
}

//void LoadManager::SetCurrentScene(CScene* aScene)
//{
//	myCurrentScene = aScene;
//}

//void LoadManager::SetCurrentPlayState(CPlayState* aPlaystate)
//{
//	myCurrentPlaystate = aPlaystate;
//}

CGameServer& CServerLoadManager::GetCurrentGameServer() const
{
	return myCurrentGameServer;
}

void CServerLoadManager::RegisterFunctions()
{
	KLoader::CKevinLoader &loader = KLoader::CKevinLoader::GetInstance();

	loader.RegisterObjectLoadFunction(LoadServerObject);
	loader.RegisterObjectLinkFunction(LinkObject);

	loader.RegisterComponentLoadFunction("NetworkComponent", LoadServerNetworkComponent);
	loader.RegisterComponentLoadFunction("MovementComponent", ServerLoadMovementComponent);
	loader.RegisterComponentLoadFunction("Enemy", LoadEnemy); 
	loader.RegisterComponentLoadFunction("WeaponSystemComponent", LoadWeaponSystemServerComponent);
	loader.RegisterComponentLoadFunction("DamageOnCollisionComponent", LoadDamageOnCollisionComponentForServer);
	loader.RegisterComponentLoadFunction("ImpController", LoadImp);
	loader.RegisterComponentLoadFunction("RevenantController", LoadRevenant);
	loader.RegisterComponentLoadFunction("PinkyController", LoadPinky);
	loader.RegisterComponentLoadFunction("BoxCollider", LoadBoxColliderServer);
	//loader.RegisterComponentLoadFunction("SphereCollider", LoadSphereColliderServer);
	loader.RegisterComponentLoadFunction("CapsuleCollider", LoadCapsuleColliderServer);
	loader.RegisterComponentLoadFunction("MeshCollider", LoadMeshColliderServer);
	//loader.RegisterComponentLoadFunction("Rigidbody", LoadRigidBodyServer);
	loader.RegisterComponentLoadFunction("CharacterController", LoadCharacterControllerServer);

	//loader.RegisterComponentLoadFunction("MeshFilter", LoadMeshFilter);
	//loader.RegisterComponentLoadFunction("Camera", LoadCamera);
	//loader.RegisterComponentLoadFunction("CircleCollider", LoadCircleCollider);
	//loader.RegisterComponentLoadFunction("SoundComponent", LoadSoundComponent);
	//loader.RegisterComponentLoadFunction("LuaScriptComponent", LoadScriptComponent);
	//loader.RegisterComponentLoadFunction("FireComponent", LoadFireComponent);
	//loader.RegisterComponentLoadFunction("PointLightComponent", LoadPointLightComponent);
}

CServerLoadManager::CServerLoadManager(CGameServer& aGameServer)
	: myCurrentGameServer(aGameServer)
{
	//myCurrentScene = nullptr;
	//myCurrentPlaystate = nullptr;

	RegisterFunctions();
}


CServerLoadManager::~CServerLoadManager()
{
	//myCurrentScene = nullptr;
	//myCurrentPlaystate = nullptr;
}
