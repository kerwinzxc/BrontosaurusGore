#include "stdafx.h"
#include "ServerMessageManager.h"

CServerMessageManager * CServerMessageManager::ourInstance = nullptr;

CServerMessageManager::CServerMessageManager(CServerMain& aServer) : myServer(aServer)
{
}


void CServerMessageManager::CreateInstance(CServerMain & aServer)
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CServerMessageManager(aServer);
	}
}

CServerMessageManager * CServerMessageManager::GetInstance()
{
	return ourInstance;
}

void CServerMessageManager::DestroyInstance()
{
	//SAFE_DELETE(ourInstance);
	delete ourInstance;
	ourInstance = nullptr;
}

CServerMessageManager::~CServerMessageManager()
{
}
