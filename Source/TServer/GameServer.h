#pragma once
#include "ServerMain.h"
#include "../CommonUtilities/ThreadPool.h"
#include "../CommonUtilities/TimerManager.h"

class CGameObjectManager;
class CAmmoComponentManager;
class CWeaponSystemManager;
class CWeaponFactory;
class CNetworkComponentManager;

class CGameServer
{
public:
	CGameServer();
	~CGameServer();

	void Init();
	void Start();

	CGameObjectManager& GetGameObjectManager();
	CNetworkComponentManager& GetNetworkComponentManager();

	void Load(const int aLevelIndex);

	void CreateManagersAndFactories();
	bool Update(CU::Time aDeltaTime);

	bool IsLoaded() const;
private:


	CGameObjectManager* myGameObjectManager;
	CAmmoComponentManager* myAmmoComponentManager;
	CWeaponSystemManager* myWeaponSystemManager;
	CWeaponFactory* myWeaponFactory;
	CNetworkComponentManager* myNetworkComponentManager;


	CU::TimerManager myTimerManager;
	CU::TimerHandle myMainTimer;

	CU::ThreadPool myThreadPool;



	bool myIsRunning;
	bool myInGame;
	bool myIsLoaded;
};

