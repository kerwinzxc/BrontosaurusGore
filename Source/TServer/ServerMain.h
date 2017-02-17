#pragma once
#include <CommonNetworkIncludes.h>
#include "TShared_NetworkWrapper.h"
#include <map>
#include "../CommonUtilities/Timer.h"
#include "../CommonUtilities/TimerManager.h"
#include "../CommonUtilities/GrowingArray.h"
#include <string>
#include "MessageManager.h"
#include "NetworkMessage_ChatMessage.h"

struct SClientAdress
{
	std::string myIP;
	std::string myPort;
	std::string myName;
};

class CServerMain
{
public:
	CServerMain();
	~CServerMain();
	void StartServer();

	void ConnectClient(SNetworkPackageHeader aHeader, std::string aName, const char* anIp, const char* aPort);
	void RecievePingResponse(SNetworkPackageHeader aHeader);
	void RecievePing(SNetworkPackageHeader aHeader, const char* data, unsigned aDataSize, const char* anIp, const char* aPort);

	void Ping(ClientID aClientID);
	void Ping();
	void DisconectClient(ClientID aClient);
	void UpdatePing(CU::Time aDeltaTime);

	void HandleChatMessage(CNetworkMessage_ChatMessage* aNetworkMessageChatMessage);
	bool Update();

private:
	CU::TimerManager myTimerManager;
	CU::TimerHandle myTimerHandle;

	TShared_NetworkWrapper myNetworkWrapper;

	std::map<ClientID, SClientAdress> myClients;
	ClientID currentFreeId;

	std::map<ClientID, float> myPendingPings;
	CMessageManager myMessageManager;
};

