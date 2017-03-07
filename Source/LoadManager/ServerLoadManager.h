#pragma once
#include "../TServer/GameServer.h"



class CServerLoadManager
{
public:
	friend class ServerLoadManagerGuard;

	static CServerLoadManager* GetInstance();


	//void SetCurrentScene(CScene* aScene);

	//void SetCurrentPlayState(CPlayState* aPlaystate);
	CGameServer& GetCurrentGameServer() const;

	void RegisterFunctions();

private:
	static void CreateInstance(CGameServer& aGameServer);
	static void DestroyInstance();

	CServerLoadManager(CGameServer& aGameServer);
	~CServerLoadManager();

	static CServerLoadManager* ourInstance;

	CGameServer& myCurrentGameServer;
	
};

class ServerLoadManagerGuard
{
public:
	~ServerLoadManagerGuard()
	{
		CServerLoadManager::DestroyInstance();
	}

private:
	ServerLoadManagerGuard(CGameServer& aGameServer)
	{
		CServerLoadManager::CreateInstance(aGameServer);
	}

	friend void CGameServer::Load(const int aLevelIndex);
};

#define GET_SERVERLOADMANAGER(LOCAL_VARIABLE)									\
CServerLoadManager* LOCAL_VARIABLE_invisiblePtr = CServerLoadManager::GetInstance();	\
if (LOCAL_VARIABLE_invisiblePtr == nullptr) { return NULL_COMPONENT; }	\
CServerLoadManager& LOCAL_VARIABLE = *LOCAL_VARIABLE_invisiblePtr