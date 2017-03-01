#include "stdafx.h"
#include "ClientMessageManager.h"
#include "../CommonUtilities/DL_Debug.h"

CClientMessageManager* CClientMessageManager::ourInstance = nullptr;

void CClientMessageManager::CreateInstance(CClient& aClient)
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CClientMessageManager(aClient);
	}
	else
	{
		DL_ASSERT("CLientMessageManager already created");
	}
}

CClientMessageManager* CClientMessageManager::GetInstance()
{
	return ourInstance;
}

void CClientMessageManager::DestroyInstance()
{
	if (ourInstance != nullptr)
	{
		delete ourInstance;
		ourInstance = nullptr;
	}
}

CClientMessageManager::CClientMessageManager(CClient& aClient) : myClient(aClient)
{
}


CClientMessageManager::~CClientMessageManager()
{
}
