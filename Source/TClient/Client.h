#pragma once
#include "../TShared/TShared_NetworkWrapper.h"
#include "../CommonUtilities/Timer.h"
#include "../CommonUtilities/TimerManager.h"
#include "../TShared/MessageManager.h"
#include "Chat.h"

enum class eClientState
{
	DISCONECTED,
	CONECTING,
	CONECTED,
};

class CClient
{
public:
	CClient();
	~CClient();

	bool StartClient();
	void Disconect();
	void UpdatePing(const CU::Time& aTime);
	void Ping();
	void Update();

	bool Connect(const char* anIp, std::string aClientName);

	CMessageManager myMessageManager;

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

