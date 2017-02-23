#pragma once
#include "ServerMain.h"
#include "../CommonUtilities/ThreadPool.h"
#include "../CommonUtilities/TimerManager.h"

class CGameObjectManager;
class AmmoComponentManager;
class WeaponSystemManager;
class WeaponFactory;

class CGameServer
{
public:
	CGameServer();
	~CGameServer();

	void Init();
	void Start();

	CGameObjectManager& GetGameObjectManager();

	void Load();

	void CreateManagersAndFactories();

private:
	bool Update();

	CGameObjectManager* myGameObjectManager;
	AmmoComponentManager* myAmmoComponentManager;
	WeaponSystemManager* myWeaponSystemManager;
	WeaponFactory* myWeaponFactory;


	CU::TimerManager myTimerManager;
	CU::TimerHandle myMainTimer;

	CU::ThreadPool myThreadPool;
	CServerMain myMainServer;



	bool myIsRunning;
	bool myInGame;
};

