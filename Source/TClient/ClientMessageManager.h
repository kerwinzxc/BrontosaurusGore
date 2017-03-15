#pragma once
#include "../TShared/MessageManager.h"
#include "Client.h"
#include "../TShared/NetworkMessageHolder.h"


class CClientMessageManager : public CMessageManager
{
public:
	static void CreateInstance(CClient& aClient);
	static CClientMessageManager* GetInstance();
	static void DestroyInstance();

	template <typename TYPE>
	TYPE* CreateMessage(const std::string& aTarget);

	template <typename TYPE>
	TYPE* CreateMessage(const int aTargetID);

	template <typename TYPE>
	TYPE* CreateMessage(SNetworkMessageHolder aMessageHolder);

private:
	static CClientMessageManager* ourInstance;

	CClientMessageManager(CClient& aClient);
	~CClientMessageManager();

	CClient& myClient;
};

template <typename TYPE>
TYPE* CClientMessageManager::CreateMessage(const std::string& aTarget)
{
	SNetworkPackageHeader header;
	if(aTarget == "__Server")
	{
		header.myTargetID = ID_SERVER;
	}
	else if (aTarget == "__All")
	{
		header.myTargetID = ID_ALL;
	}
	else if (aTarget == "__All_But_Me")
	{
		header.myTargetID = ID_ALL_BUT_ME;
	}

	header.mySenderID = myClient.myId;
	return CMessageManager::CreateMessage<TYPE>(header);
}

template <typename TYPE>
TYPE* CClientMessageManager::CreateMessage(const int aID)
{
	SNetworkPackageHeader header;

	header.myTargetID = aID;

	header.mySenderID = myClient.myId;
	return CMessageManager::CreateMessage<TYPE>(header);
}

template <typename TYPE>
TYPE* CClientMessageManager::CreateMessage(SNetworkMessageHolder aMessageHolder)
{
	TYPE* message = CMessageManager::CreateMessage<TYPE>(aMessageHolder.myHeader);
	message->SetData(aMessageHolder.Stream);

	return message;
}
