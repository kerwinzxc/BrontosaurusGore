#include "stdafx.h"
#include "Client.h"
#include "../CommonUtilities/DL_Debug.h"
#include <iostream>
#include "../TShared/PackageType.h"
#include "../TShared/NetworkMessage_Ping.h"
#include "../TShared/NetworkMessage_PingResponse.h"
#include "../TShared/NetworkMesssage_Connect.h"
#include "../TShared/NetworkMessage_ConectResponse.h"
#include "../TShared/NetworkMessage_Position.h"
#include "../CommonUtilities/CommonUtilities.h"
#include "../TShared/NetworkMessage_ChatMessage.h"
#include "ClientMessageManager.h"
#include "../PostMaster/EMessageReturn.h"
#include "../PostMaster/Message.h"
#include "../PostMaster/Event.h"
#include "../PostMaster/PostMaster.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../BrontosaurusEngine/Engine.h"
#include "../CommonUtilities/ThreadPool.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "ServerReadyMessage.h"
#include "../CommonUtilities/ThreadNamer.h"


#include "../Components/NetworkComponentManager.h"
#include "../Components/NetworkComponent.h"
#include "../Components/GameObject.h"
#include "../Components/ComponentMessage.h"


CClient::CClient(): myMainTimer(0), myCurrentPing(0), myState(eClientState::DISCONECTED), myId(0), myServerIp(nullptr), myServerPingTime(0), myServerIsPinged(false)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);
	CClientMessageManager::CreateInstance(*this);

	myIsRunning = false;
	myCanQuit = false;
}


CClient::~CClient()
{
	myIsRunning = false;
	while (!myCanQuit)
	{
		continue;
	}
	CClientMessageManager::DestroyInstance();
}

bool CClient::StartClient()
{
	myNetworkWrapper.Init(0, CClientMessageManager::GetInstance());
	myMainTimer = myTimerManager.CreateTimer();
	CEngine::GetInstance()->GetThreadPool()->AddWork(CU::Work(std::bind(&CClient::Update, this)));
	return true;
}

void CClient::Disconect()
{
	myState = eClientState::DISCONECTED;
	std::cout << "Disconected from server";
	myServerIsPinged = false;
	//myChat.StopChat();
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

		CNetworkMessage_Ping* tempMessagePing = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_Ping>("__Server");

		myNetworkWrapper.Send(tempMessagePing, myServerIp, SERVER_PORT);

		//myNetworkWrapper.Send(header, nullptr, 0, myServerIp, SERVER_PORT);
		myServerIsPinged = true;
		myServerPingTime = 0;
	}
}

void CClient::Update()
{
	float currentTime = 0.f;

	std::string pingString;
	pingString = "Ping: ";
	pingString += std::to_string(static_cast<int>(myCurrentPing * 1000 + 0.5f)).c_str();

	DL_PRINT(pingString.c_str());
	CU::SetThreadName("ClientUpdate");
	myIsRunning = true;

	while (myIsRunning)
	{
		Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();

		if(myTimerManager.Size() == 0)
		{
			break;
		}
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
				myState = eClientState::CONECTED;

				std::cout << "Conected to server got id:" << myId << std::endl;
				std::cout << "Starting chat" << std::endl;
				//myChat.StartChat();
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

				CNetworkMessage_PingResponse* newMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_PingResponse>("__Server");
				myNetworkWrapper.Send(newMessage, myServerIp, SERVER_PORT);
			}
			break;
		case ePackageType::ePingResponse:
			{
				myCurrentPing = myServerPingTime;
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
		case ePackageType::eServerReady:
			{
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CServerReadyMessage());
			}
			break;
		case ePackageType::ePosition:
		{
			CNetworkMessage_Position *positionMessage = currentMessage->CastTo<CNetworkMessage_Position>();
			//std::cout << "Got position message with ID: " << positionMessage->GetID() << " and position: X:" << positionMessage->GetPosition().x << " Y:" << positionMessage->GetPosition().y << " Z:" << positionMessage->GetPosition().z << std::endl;

			CNetworkComponent* comp = CNetworkComponentManager::GetInstance()->GetComponent(positionMessage->GetID());
			comp->GetParent()->SetWorldPosition(positionMessage->GetPosition());
			comp->GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());

		}
		break;
		case ePackageType::eZero:
		case ePackageType::eConnect:
		case ePackageType::eSize:
		default: break;
		}


		if (currentTime > 1.f)
		{
			Ping();
			currentTime = 0.f;
		}

		//const std::string chatMessage = myChat.GetChatMessage();

		//if (chatMessage != "")
		//{
		//	SNetworkPackageHeader header;
		//	header.myPackageType = static_cast<char>(ePackageType::eChat);
		//	header.mySenderID = myId;
		//	header.myTargetID = ID_ALL_BUT_ME;
		//	header.myTimeStamp = KYLE;

		//	CNetworkMessage_ChatMessage* chatNMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_ChatMessage>("__All_But_Me");

		//	chatNMessage->myChatMessage = myName + ":" + chatMessage;
		//	myNetworkWrapper.Send(chatNMessage, myServerIp, SERVER_PORT);
		//}
	}

	myCanQuit = true;
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

	CNetworkMessage_Connect* message = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_Connect>("__Server");
	message->myClientName = aClientName;

	myNetworkWrapper.Send(message, anIp, SERVER_PORT);

	while (myState == eClientState::CONECTING)
	{
		DL_PRINT("wait Connecting");
	}

	return  true;
}

short CClient::GetID()
{
	return myId;
}

eMessageReturn CClient::DoEvent(const CSendNetowrkMessageMessage& aSendNetowrkMessageMessage)
{
	Send(aSendNetowrkMessageMessage.UnpackHolder());
	return eMessageReturn::eContinue;
}
