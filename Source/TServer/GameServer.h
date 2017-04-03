#pragma once
#include "ServerMain.h"
#include "../CommonUtilities/ThreadPool.h"
#include "../CommonUtilities/TimerManager.h"
#include "../Components/EnemyComponentManager.h"
#include "../Physics/Foundation.h"
#include "../Physics/Physics.h"
#include "../Game/BackgroundLoadingManager.h"

class CGameObjectManager;
class CAmmoComponentManager;
class CWeaponSystemManager;
class CWeaponFactory;
class CMovementComponentManager;
class CSpawnerManager;
class CWaveManager;

class CGameServer
{
public:
	CGameServer();
	~CGameServer();

	void Init();
	void Start();

	CGameObjectManager& GetGameObjectManager();
	inline CMovementComponentManager* GetMovementComponentManager();

	void Load(const int aLevelIndex);
	void ReInit();

	void CreateManagersAndFactories();
	void DestroyManagersAndFactories();
	bool Update(CU::Time aDeltaTime);

	bool IsLoaded() const;
	std::thread::id& GetThreadID();
	CServerPlayerNetworkComponent* AddPlayer(const unsigned short aClientID);
	CEnemyComponentManager* GetEnemyComponentManager();
	CWeaponSystemManager* GetCWeaponSystemManager();
	CSpawnerManager* GetSpawnerManager();
	inline CDamageOnCollisionComponentManager* GetDamageOnCollisionComponentManager() const;
	inline CColliderComponentManager* GetColliderComponentManager();
private:
	CU::GrowingArray<CServerPlayerNetworkComponent*> myPlayersNetworkComponents;
	CGameObjectManager* myGameObjectManager;
	CAmmoComponentManager* myAmmoComponentManager;
	CWeaponSystemManager* myWeaponSystemManager;
	CWeaponFactory* myWeaponFactory;
	CMovementComponentManager* myMovementComponentManager;
	CDamageOnCollisionComponentManager* myDamageOnCollisionComponentManager;
	CSpawnerManager* mySpawnerManager;
	CCheckPointSystem* myCheckPointSystem;
	CWaveManager* myWaveManager;

	CU::TimerManager myTimerManager;
	CU::TimerHandle myMainTimer;

	CU::ThreadPool myThreadPool;

	std::thread::id myThreadID;



	bool myIsRunning;
	bool myInGame;
	std::atomic_bool myIsLoaded;

	float myTime;
	CEnemyComponentManager* myEnemyComponentManager;
	Physics::CPhysics* myPhysics;
	Physics::CPhysicsScene* myPhysicsScene;
	CColliderComponentManager* myColliderComponentManager;
};

CMovementComponentManager* CGameServer::GetMovementComponentManager()
{
	return myMovementComponentManager;
}

inline CDamageOnCollisionComponentManager* CGameServer::GetDamageOnCollisionComponentManager() const
{
	return myDamageOnCollisionComponentManager;
}

inline CColliderComponentManager* CGameServer::GetColliderComponentManager()
{
	return myColliderComponentManager;
}
