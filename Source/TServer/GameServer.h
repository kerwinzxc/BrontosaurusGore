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
	CServerPlayerNetworkComponent* AddPlayer(const unsigned short aClientID) const;
	CEnemyComponentManager* GetEnemyComponentManager();
	CWeaponSystemManager* GetCWeaponSystemManager();
	CSpawnerManager* GetSpawnerManager();
	inline CDamageOnCollisionComponentManager* GetDamageOnCollisionComponentManager() const;
	inline CColliderComponentManager* GetColliderComponentManager();
private:
	CGameObjectManager* myGameObjectManager;
	CAmmoComponentManager* myAmmoComponentManager;
	CWeaponSystemManager* myWeaponSystemManager;
	CWeaponFactory* myWeaponFactory;
	CMovementComponentManager* myMovementComponentManager;
	CDamageOnCollisionComponentManager* myDamageOnCollisionComponentManager;
	CSpawnerManager* mySpawnerManager;
	CCheckPointSystem* myCheckPointSystem;

	CU::TimerManager myTimerManager;
	CU::TimerHandle myMainTimer;

	CU::ThreadPool myThreadPool;



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
