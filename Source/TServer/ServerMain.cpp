#include "stdafx.h"
#include "ServerMain.h"
#include "../CommonUtilities/DL_Debug.h"
#include "../CommonUtilities/TimerManager.h"
#include "PackageType.h"
#include "NetworkMessage_PingResponse.h"
#include "NetworkMesssage_Connect.h"
#include "NetworkMessage_Ping.h"
#include "NetworkMessage_ConectResponse.h"
#include "NetworkMessage_ChatMessage.h"


CServerMain::CServerMain(): myTimerHandle(0), currentFreeId(ID_FREE)
{
}


CServerMain::~CServerMain()
{
}

void CServerMain::StartServer()
{
	myTimerHandle = myTimerManager.CreateTimer();
	myNetworkWrapper.Init(DEFAULT_PORT);
}

void CServerMain::ConnectClient(SNetworkPackageHeader aHeader, std::string aName, const char* anIp, const char* aPort)
{
	std::cout << "Client trying to connect" << std::endl;

	SClientAdress newClient;
	newClient.myIP = anIp;
	newClient.myPort = aPort;
	newClient.myName = aName;

	ClientID newClientId = currentFreeId++;
	myClients[newClientId] = newClient;
	SNetworkPackageHeader acceptPackage;
	acceptPackage.myPackageType = static_cast<char>(ePackageType::CONNECT_RESPONSE);
	acceptPackage.mySenderID = ID_SERVER;
	acceptPackage.myTargetID = newClientId;
	acceptPackage.myTimeStamp = myTimerManager.GetTimer(myTimerHandle).GetLifeTime().GetMilliseconds();

	CNetworkMessage_ConectResponse* message = myMessageManager.CreateMessage<CNetworkMessage_ConectResponse>(acceptPackage);

	message->myClientId = newClientId;

	myNetworkWrapper.Send(message, anIp, aPort);
	std::cout << "Client: " << aName << " connected" << std::endl;
}

void CServerMain::RecievePingResponse(SNetworkPackageHeader aHeader)
{
	//std::cout << "Recieved ping response from ";

	/*if (myClients.count(aHeader.mySenderID) > 0)
	{
		std::cout << myClients.at(aHeader.mySenderID).myName << std::endl;
	}
	else
	{
		std::cout << "unknown" << std::endl;
	}*/

	if (myPendingPings.count(aHeader.mySenderID) > 0)
	{
		myPendingPings.erase(aHeader.mySenderID);
	}
}


void CServerMain::Ping(ClientID aClientID)
{
	SNetworkPackageHeader header;
	header.myPackageType = static_cast<char>(ePackageType::PING);
	header.mySenderID = ID_SERVER;
	header.myTargetID = aClientID;

	SClientAdress client = myClients[aClientID];

	CNetworkMessage_Ping* messagePing = myMessageManager.CreateMessage<CNetworkMessage_Ping>(header);

	myNetworkWrapper.Send(messagePing, client.myIP.c_str(), client.myPort.c_str());
	myPendingPings[aClientID] = 0;
}

void CServerMain::Ping()
{
	for (auto client : myClients)
	{
		if (myPendingPings.count(client.first) == 0)
		{
			Ping(client.first);
		}
	}
}

void CServerMain::DisconectClient(ClientID aClient)
{
	std::cout << "Client " << myClients[aClient].myName << " disconected" << std::endl;
	const std::string leftClientsName = myClients[aClient].myName;
	myClients.erase(aClient);
	
	if (myPendingPings.count(aClient) > 0)
	{
		myPendingPings.erase(aClient);
	}

	SNetworkPackageHeader header;
	header.mySenderID = ID_SERVER;
	header.myPackageType = static_cast<char>(ePackageType::eChat);
	header.myTimeStamp = GetCurrentTime();

	CNetworkMessage_ChatMessage* leaveMessage = myMessageManager.CreateMessage<CNetworkMessage_ChatMessage>(header);
	leaveMessage->myChatMessage = ("Server: " + leftClientsName + " has lef the server");


	for (auto client: myClients)
	{
		header.myTargetID = client.first;
		myNetworkWrapper.Send(leaveMessage, client.second.myIP.c_str(), client.second.myPort.c_str());
	}
	delete leaveMessage;
}

void CServerMain::UpdatePing(CU::Time aDeltaTime)
{
	for (auto pendingPing : myPendingPings)
	{
		myPendingPings[pendingPing.first] += aDeltaTime.GetSeconds();

		if (myPendingPings[pendingPing.first] >= 10)
		{
			std::cout << "Client: " << myClients[pendingPing.first].myName << " not responding" << std::endl;
			DisconectClient(pendingPing.first);
			break;
		}
	}
}

void CServerMain::HandleChatMessage(CNetworkMessage_ChatMessage* aNetworkMessageChatMessage)
{
	std::cout << aNetworkMessageChatMessage->myChatMessage << std::endl;

	SNetworkPackageHeader header = aNetworkMessageChatMessage->GetHeader();
	header.myTimeStamp = 42;
	switch (aNetworkMessageChatMessage->GetHeader().myTargetID)
	{
	case ID_ALL_BUT_ME:
		for (auto client : myClients)
		{
			if (client.first != header.mySenderID)
			{
				header.myTargetID = client.first;
				aNetworkMessageChatMessage->SetHeader(header);
				myNetworkWrapper.Send(aNetworkMessageChatMessage, client.second.myIP.c_str(), client.second.myPort.c_str());
			}
		}
		break;
	case ID_SERVER:
		break;
	default:
		if (myClients.count(header.myTargetID) > 0)
		{
			myNetworkWrapper.Send(aNetworkMessageChatMessage, myClients.at(header.myTargetID).myIP.c_str(), myClients.at(header.myTargetID).myIP.c_str());
		}
		break;
	}

	delete aNetworkMessageChatMessage;
}

bool CServerMain::Update()
{
	float currentTime = 0.f;

	while (true)
	{
		myTimerManager.UpdateTimers();
		currentTime += myTimerManager.GetTimer(myTimerHandle).GetDeltaTime().GetSeconds();

		UpdatePing(myTimerManager.GetTimer(myTimerHandle).GetDeltaTime());

		char* currentSenderIp = nullptr;
		char* currentSenderPort = nullptr;

		CNetworkMessage* currentMessage = myNetworkWrapper.Recieve(&currentSenderIp, &currentSenderPort);

		switch (static_cast<ePackageType>(currentMessage->GetHeader().myPackageType))
		{
		case ePackageType::CONNECT:
			{
				std::cout << "Conect request recieved from IP " << currentSenderIp << std::endl;
				CNetworkMessage_Connect* connectMessage = currentMessage->CastTo<CNetworkMessage_Connect>();
				ConnectClient(connectMessage->GetHeader(), connectMessage->myClientName, currentSenderIp, currentSenderPort);
				delete connectMessage;
			}
			break;
		case ePackageType::PING:
			{
				//std::cout << "Ping message recievd from client " << std::endl;
				SNetworkPackageHeader newHeader;
				newHeader.myPackageType = static_cast<char>(ePackageType::PING_RESPONSE);
				newHeader.mySenderID = ID_SERVER;
				newHeader.myTargetID = currentMessage->GetHeader().mySenderID;
				newHeader.myTimeStamp = 100;

				CNetworkMessage_PingResponse* newMessage = myMessageManager.CreateMessage<CNetworkMessage_PingResponse>(newHeader);
				myNetworkWrapper.Send(newMessage, currentSenderIp, currentSenderPort);
				delete newMessage;
			}
			break;
		case ePackageType::PING_RESPONSE:
			{
				RecievePingResponse(currentMessage->GetHeader());
			}
			break;
		case ePackageType::eChat:
			{
				HandleChatMessage(currentMessage->CastTo<CNetworkMessage_ChatMessage>());
			}
		case ePackageType::ZERO:
		case ePackageType::SIZE:
		default: break;
		}

		delete currentSenderIp;
		delete currentSenderPort;
		delete currentMessage;

		if (currentTime > 1.f)
		{
			Ping();
			currentTime = 0.f;
		}
	}
}
