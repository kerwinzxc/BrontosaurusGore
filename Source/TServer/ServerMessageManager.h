#pragma once
#include "../TShared/MessageManager.h"
#include "../TShared/NetworkMessageHolder.h"

class CServerMain;

class CServerMessageManager : public CMessageManager
{
public:
	static void CreateInstance(CServerMain& aServer);
	static CServerMessageManager* GetInstance();
	static void DestroyInstance();

	template <typename TYPE>
	TYPE* CreateMessage(unsigned aID);

	template <typename TYPE>
	TYPE* CreateMessage(SNetworkMessageHolder aMessageHolder);

private:
	static CServerMessageManager* ourInstance;

	CServerMessageManager(CServerMain& aServer);
	~CServerMessageManager();

	CServerMain& myServer;
};

template <typename TYPE>
TYPE* CServerMessageManager::CreateMessage(unsigned aID)
{
	SNetworkPackageHeader header;

	header.myTargetID = aID;

	header.mySenderID = ID_SERVER;
	return CMessageManager::CreateMessage<TYPE>(header);
}

template <typename TYPE>
TYPE* CServerMessageManager::CreateMessage(SNetworkMessageHolder aMessageHolder)
{
	TYPE* message = CMessageManager::CreateMessage<TYPE>(aMessageHolder.myHeader);
	message->SetData(aMessageHolder.Stream);

	return message;
}