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
#include "../ThreadedPostmaster/Subscriber.h"


enum class eServerState
{
	eWaitingForClients,
	eLoadingLevel,
	eInGame,
};

class CGameServer;

struct SClientData
{
	std::string myIP;
	std::string myPort;
	std::string myName;
	int ResponseTime;
	bool IsReady;
};

struct SImportantWaitData
{
	SImportantWaitData(): myNetworkMessage(nullptr), myWaitedTime(0)
	{
	}

	SImportantWaitData(CImportantNetworkMessage* aNetworkMessage)
		: myNetworkMessage(aNetworkMessage),
		  myWaitedTime(0)
	{
	}

	CImportantNetworkMessage* myNetworkMessage;
	CU::Time myWaitedTime;
};

class CServerMain : public Postmaster::ISubscriber
{
public:
	CServerMain();
	~CServerMain();
	void StartServer();

	void UpdateImportantMessages(const CU::Time aDeltaTime);
	void RecieveImportantResponse(CImportantNetworkMessage* aNetworkMessage);

	void ConnectClient(SNetworkPackageHeader aHeader, std::string aName, const char* anIp, const char* aPort);
	void RecievePingResponse(SNetworkPackageHeader aHeader);
	void RecievePing(SNetworkPackageHeader aHeader, const char* data, unsigned aDataSize, const char* anIp, const char* aPort);

	void Ping(ClientID aClientID);
	void Ping();
	void DisconectClient(ClientID aClient);
	void UpdatePing(CU::Time aDeltaTime);

	void SendTo(CNetworkMessage* aNetworkMessage, bool aIsResend = false);

	void HandleChatMessage(CNetworkMessage_ChatMessage* aNetworkMessageChatMessage);
	bool CheckIfClientsReady() const;
	void StartGame();
	bool Update();

private:

	CU::TimerManager myTimerManager;
	CU::TimerHandle myTimerHandle;

	TShared_NetworkWrapper myNetworkWrapper;

	std::map<int, CU::Time> myPings;

	std::map<int, SImportantWaitData> myImportantMessages;
	int myImportantCount;

	std::map<ClientID, SClientData> myClients;

	ClientID currentFreeId;

	std::map<ClientID, float> myPendingPings;
	CMessageManager myMessageManager;

	eServerState myServerState;

	CGameServer* myGameServer;
};

