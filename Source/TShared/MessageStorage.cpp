#include "stdafx.h"
#include "MessageStorage.h"
CMessageStorage* CMessageStorage::ourInstance = nullptr;

void CMessageStorage::CreateInstance()
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CMessageStorage;
	}
	else
	{
		DL_ASSERT("CMessage Storage already created");
	}
}

CMessageStorage* CMessageStorage::GetInstance()
{
	return  ourInstance;
}

void CMessageStorage::DestroyInstance()
{
	if (ourInstance == nullptr)
	{
		DL_ASSERT("CMessageStorage is not created");
	}

	delete ourInstance;
	ourInstance = nullptr;
}

CMessageStorage::CMessageStorage(): myCurrentFreeId(0)
{
}


CMessageStorage::~CMessageStorage()
{
}
