#pragma once
#include "ServerMain.h"
#include "../CommonUtilities/ThreadPool.h"
#include "../CommonUtilities/TimerManager.h"

class CGameObjectManager;
class CAmmoComponentManager;
class CWeaponSystemManager;
class CWeaponFactory;
class CMovementComponentManager;

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

	void CreateManagersAndFactories();
	bool Update(CU::Time aDeltaTime);

	bool IsLoaded() const;
private:


	CGameObjectManager* myGameObjectManager;
	CAmmoComponentManager* myAmmoComponentManager;
	CWeaponSystemManager* myWeaponSystemManager;
	CWeaponFactory* myWeaponFactory;
	CMovementComponentManager* myMovementComponentManager;


	CU::TimerManager myTimerManager;
	CU::TimerHandle myMainTimer;

	CU::ThreadPool myThreadPool;



	bool myIsRunning;
	bool myInGame;
	bool myIsLoaded;

	float myTime;
};

CMovementComponentManager* CGameServer::GetMovementComponentManager()
{
	return myMovementComponentManager;
}