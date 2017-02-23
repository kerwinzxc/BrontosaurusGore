#pragma once
#include "ServerMain.h"
#include "../CommonUtilities/ThreadPool.h"
#include "../CommonUtilities/TimerManager.h"

class CGameServer
{
public:
	CGameServer();
	~CGameServer();

	void Init();
	void Start();

private:
	bool Update();

	CU::TimerManager myTimerManager;
	CU::TimerHandle myMainTimer;

	CU::ThreadPool myThreadPool;
	CServerMain myMainServer;
	bool myIsRunning;
	bool myInGame;
};

