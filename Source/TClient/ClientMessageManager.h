#pragma once
#include "../TShared/MessageManager.h"
#include "Client.h"

class CClientMessageManager : public CMessageManager
{
public:
	CClientMessageManager(CClient& aClient);
	~CClientMessageManager();

	template <typename TYPE>
	TYPE* CreateMessage(const std::string& aTarget);

private:
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
