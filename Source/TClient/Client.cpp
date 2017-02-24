#include "stdafx.h"
#include "Client.h"
#include "../CommonUtilities/DL_Debug.h"
#include <iostream>
#include "../TShared/PackageType.h"
#include "../TShared/NetworkMessage_Ping.h"
#include "../TShared/NetworkMessage_PingResponse.h"
#include "../TShared/NetworkMesssage_Connect.h"
#include "../TShared/NetworkMessage_ConectResponse.h"
#include "../CommonUtilities/CommonUtilities.h"
#include "../TShared/NetworkMessage_ChatMessage.h"
#include "ClientMessageManager.h"
#include "../PostMaster/EMessageReturn.h"
#include "../PostMaster/Message.h"
#include "../PostMaster/Event.h"
#include "../PostMaster/PostMaster.h"
#include "../ThreadedPostmaster/Postmaster.h"


CClient::CClient(): myMainTimer(0), myState(eClientState::DISCONECTED), myId(0), myServerIp(nullptr), myServerPingTime(0), myServerIsPinged(false), myMessageManager(nullptr)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);
}


CClient::~CClient()
{
}

bool CClient::StartClient()
{
	myNetworkWrapper.Init(0);
	myMessageManager = new CClientMessageManager(*this);
	myMainTimer = myTimerManager.CreateTimer();
	return true;
}

void CClient::Disconect()
{
	myState = eClientState::DISCONECTED;
	std::cout << "Disconected from server";
	myServerIsPinged = false;
	myChat.StopChat();
}

void CClient::UpdatePing(const CU::Time& aTime)
{
	if (myServerIsPinged && myState == eClientState::CONECTED)
	{
		myServerPingTime += aTime.GetSeconds();

		if (myServerPingTime >= 10)
		{
			std::cout << "Server is not responding" << std::endl;
			Disconect();
		}
	}
}

void CClient::Ping()
{
	if (myServerIsPinged == false)
	{
		SNetworkPackageHeader header;
		header.mySenderID = myId;
		header.myTargetID = ID_SERVER;
		header.myPackageType = static_cast<char>(ePackageType::ePing);
		header.myTimeStamp = 100;

		CNetworkMessage_Ping* tempMessagePing = myMessageManager->CreateMessage<CNetworkMessage_Ping>("__Server");

		myNetworkWrapper.Send(tempMessagePing, myServerIp, SERVER_PORT);

		//myNetworkWrapper.Send(header, nullptr, 0, myServerIp, SERVER_PORT);
		myServerIsPinged = true;
		myServerPingTime = 0;
	}
}

void CClient::Update()
{
	float currentTime = 0.f;

	//while (true)
	{
		myTimerManager.UpdateTimers();
		currentTime += myTimerManager.GetTimer(myMainTimer).GetDeltaTime().GetSeconds();

		UpdatePing(myTimerManager.GetTimer(myMainTimer).GetDeltaTime());

		CNetworkMessage* currentMessage = myNetworkWrapper.Recieve(nullptr, nullptr);

		switch (static_cast<ePackageType>(currentMessage->GetHeader().myPackageType))
		{
		case ePackageType::eConnectResponse:
			if (myState == eClientState::CONECTING)
			{
				CNetworkMessage_ConectResponse* conectResponseMessage = currentMessage->CastTo<CNetworkMessage_ConectResponse>();
				myId = conectResponseMessage->myClientId;
				delete conectResponseMessage;
				myState = eClientState::CONECTED;

				std::cout << "Conected to server got id:" << myId << std::endl;
				std::cout << "Starting chat" << std::endl;
				myChat.StartChat();
			}
			break;
		case ePackageType::ePing:
			if (myState == eClientState::CONECTED)
			{
				SNetworkPackageHeader header;
				header.mySenderID = myId;
				header.myTargetID = ID_SERVER;
				header.myPackageType = static_cast<char>(ePackageType::ePingResponse);
				header.myTimeStamp = 100;

				CNetworkMessage_PingResponse* newMessage = myMessageManager->CreateMessage<CNetworkMessage_PingResponse>("__Server");
				myNetworkWrapper.Send(newMessage, myServerIp, SERVER_PORT);
				delete newMessage;
			}
			break;
		case ePackageType::ePingResponse:
			{
				myServerPingTime = 0;
				myServerIsPinged = false;
			}
			break;
		case ePackageType::eChat:
			{
				CNetworkMessage_ChatMessage *chatMessage = currentMessage->CastTo<CNetworkMessage_ChatMessage>();
				std::cout << chatMessage->myChatMessage << std::endl;
			}
			break;
		case ePackageType::eZero:
		case ePackageType::eConnect:
		case ePackageType::eSize:
		default: break;
		}

		delete currentMessage;

		if (currentTime > 1.f)
		{
			Ping();
			currentTime = 0.f;
		}

		const std::string chatMessage = myChat.GetChatMessage();

		if (chatMessage != "")
		{
			SNetworkPackageHeader header;
			header.myPackageType = static_cast<char>(ePackageType::eChat);
			header.mySenderID = myId;
			header.myTargetID = ID_ALL_BUT_ME;
			header.myTimeStamp = KYLE;

			CNetworkMessage_ChatMessage* chatNMessage = myMessageManager->CreateMessage<CNetworkMessage_ChatMessage>("__All_But_Me");

			chatNMessage->myChatMessage = myName + ":" + chatMessage;
			myNetworkWrapper.Send(chatNMessage, myServerIp, SERVER_PORT);
		}
	}
}

void CClient::Send(CNetworkMessage* aNetworkMessage)
{
	myNetworkWrapper.Send(aNetworkMessage, myServerIp, SERVER_PORT);
}

bool CClient::Connect(const char* anIp, std::string aClientName)
{
	myName = aClientName;
	myState = eClientState::CONECTING;

	myServerIp = anIp;

	SNetworkPackageHeader header;
	header.myPackageType = static_cast<char>(ePackageType::eConnect);
	header.myTargetID = ID_SERVER;
	header.mySenderID = ID_ALL;
	header.myTimeStamp = myTimer.GetLifeTime().GetMilliseconds();

	CNetworkMessage_Connect* message = myMessageManager->CreateMessage<CNetworkMessage_Connect>("__Server");
	message->myClientName = aClientName;

	return  myNetworkWrapper.Send(message, anIp, SERVER_PORT);
}
