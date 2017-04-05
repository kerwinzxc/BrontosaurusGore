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
#include "../TShared/NetworkMessageHolder.h"
#include "../ThreadedPostmaster/Subscriber.h"
#include "../Components/ServerPlayerNetworkComponent.h"


enum class eServerState
{
	eWaitingForClients,
	eLoadingLevel,
	eInGame,
};

class CGameServer;

struct SClientData
{
	SClientData(): ResponseTime(0) {}

	std::string myIP;
	std::string myPort;
	std::string myName;
	CU::Time ResponseTime;
	bool IsReady;
	CServerPlayerNetworkComponent* myComponent = nullptr;
};

struct SImportantWaitData
{
	SImportantWaitData(): myWaitedTime(0)
	{
	}

	SImportantWaitData(CImportantNetworkMessage* aNetworkMessage): myWaitedTime(0)
	{
		aNetworkMessage->PackMessage();
		myMessage.myHeader = aNetworkMessage->GetHeader();
		myMessage.Stream = aNetworkMessage->GetSerializedData();
	}

	SNetworkMessageHolder myMessage;
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

	ClientID ConnectClient(SNetworkPackageHeader aHeader, std::string aName, const char* anIp, const char* aPort);
	void RecievePingResponse(SNetworkPackageHeader aHeader);

	void Ping(ClientID aClientID);
	void Ping();
	void DisconectClient(ClientID aClient);
	void UpdatePing(CU::Time aDeltaTime);

	void SendTo(CNetworkMessage* aNetworkMessage, bool aIsResend = false);

	void HandleChatMessage(CNetworkMessage_ChatMessage* aNetworkMessageChatMessage);
	bool CheckIfClientsReady() const;
	void StartGame();
	bool Update();

	eMessageReturn DoEvent(const CSendNetworkMessageMessage& aSendNetowrkMessageMessage) override;

	void PrintDebugInfo();
	void Shutdown();
	bool IsClosed();
private:

	CU::TimerManager myTimerManager;
	CU::TimerHandle myTimerHandle;

	TShared_NetworkWrapper myNetworkWrapper;

	std::map<int, CU::Time> myPings;

	std::map<int, SImportantWaitData> myImportantMessages;
	int myImportantCount;

	std::map<ClientID, SClientData> myClients;

	ClientID currentFreeId;

	std::map<ClientID, CU::Time> myPendingPings;
	std::map<unsigned char, float> myAlivePlayers;
	std::map<unsigned char, float> myDeadPlayers;
	CMessageManager* myMessageManager;

	eServerState myServerState;

	CGameServer* myGameServer;

	float myPlayerRespawnTime;
	bool myIsRunning;
	bool myCanQuit;
	bool myFinishedLoading;
	bool myIsClosed;
};

