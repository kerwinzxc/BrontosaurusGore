#pragma once
#include "../TShared/CommonNetworkIncludes.h"
#include "../TShared/TShared_NetworkWrapper.h"
#include <map>
#include "../CommonUtilities/Timer.h"
#include "../CommonUtilities/TimerManager.h"
#include "../CommonUtilities/GrowingArray.h"
#include <string>
#include "../TShared/MessageManager.h"
#include "../TShared/NetworkMessage_ChatMessage.h"


class CGameServer;

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

	void SendTo(CNetworkMessage* aNetworkMessage);

	void HandleChatMessage(CNetworkMessage_ChatMessage* aNetworkMessageChatMessage);
	bool Update();

private:
	CU::TimerManager myTimerManager;
	CU::TimerHandle myTimerHandle;

	TShared_NetworkWrapper myNetworkWrapper;

	std::map<__int16, CImportantNetworkMessage*> myImportantMessages;

	std::map<ClientID, SClientAdress> myClients;
	ClientID currentFreeId;

	std::map<ClientID, float> myPendingPings;
	CMessageManager myMessageManager;

	CGameServer* myGameServer;
};

