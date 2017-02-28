#pragma once
#include "../TShared/TShared_NetworkWrapper.h"
#include "../CommonUtilities/Timer.h"
#include "../CommonUtilities/TimerManager.h"
#include "../TShared/MessageManager.h"
#include "Chat.h"
#include "../PostMaster/Subscriber.h"
#include "../ThreadedPostmaster/Subscriber.h"

enum class eClientState
{
	DISCONECTED,
	CONECTING,
	CONECTED,
};

class CClient : public Postmaster::ISubscriber
{
	friend class CClientMessageManager;
public:

	static void Create();
	static void Destroy();
	static CClient& GetInstance();
	bool StartClient();
	void Disconect();
	void UpdatePing(const CU::Time& aTime);
	void Ping();
	void Update();
	void Send(CNetworkMessage* aNetworkMessage);

	bool Connect(const char* anIp, std::string aClientName);

	CClientMessageManager* myMessageManager;

private:

	CClient();
	~CClient();


	static CClient* ourInstance;

	CChat myChat;

	CU::TimerManager myTimerManager;
	CU::TimerHandle myMainTimer;

	float myCurrentPing;

	TShared_NetworkWrapper myNetworkWrapper;
	eClientState myState;
	CU::Timer myTimer;
	short myId;

	std::string myName;
	const char* myServerIp;
	CU::TimeUnit myServerPingTime;
	bool myServerIsPinged;
};

