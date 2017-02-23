#include "stdafx.h"
#include "GameServer.h"


CGameServer::CGameServer()
{
	myIsRunning = false;
}


CGameServer::~CGameServer()
{
}

void CGameServer::Init()
{
	myMainTimer = myTimerManager.CreateTimer();
	myMainServer.StartServer();
}

void CGameServer::Start()
{
	CU::Work work(std::bind(&CServerMain::Update, &myMainServer));
	myThreadPool.AddWork(work);

	do
	{
		myIsRunning = Update();
	} while (myIsRunning == true);
}

bool CGameServer::Update()
{
	return true;
}
