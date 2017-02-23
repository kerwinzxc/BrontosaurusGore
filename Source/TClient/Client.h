#pragma once
#include "../TShared/TShared_NetworkWrapper.h"
#include "../CommonUtilities/Timer.h"
#include "../CommonUtilities/TimerManager.h"
#include "../TShared/MessageManager.h"
#include "Chat.h"
#include "../PostMaster/Subscriber.h"

enum class eClientState
{
	DISCONECTED,
	CONECTING,
	CONECTED,
};

class CClient : public Subscriber
{
	friend class CClientMessageManager;
public:
	CClient();
	~CClient();

	bool StartClient();
	void Disconect();
	void UpdatePing(const CU::Time& aTime);
	void Ping();
	void Update();
	void Send(CNetworkMessage* aNetworkMessage);

	eMessageReturn Recieve(const Message& aMessage) override;

	bool Connect(const char* anIp, std::string aClientName);

	CClientMessageManager* myMessageManager;

private:
	CChat myChat;

	CU::TimerManager myTimerManager;
	CU::TimerHandle myMainTimer;

	TShared_NetworkWrapper myNetworkWrapper;
	eClientState myState;
	CU::Timer myTimer;
	short myId;

	std::string myName;
	const char* myServerIp;
	CU::TimeUnit myServerPingTime;
	bool myServerIsPinged;
};

