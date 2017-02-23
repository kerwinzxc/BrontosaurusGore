#pragma once

class CGameServer;

class CServerLoadManager
{
public:
	CServerLoadManager();
	~CServerLoadManager();
	friend class LoadManagerGuard;

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

