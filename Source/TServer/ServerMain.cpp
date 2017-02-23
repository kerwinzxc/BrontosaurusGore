#include "stdafx.h"
#include "ServerMain.h"
#include "../CommonUtilities/DL_Debug.h"
#include "../CommonUtilities/TimerManager.h"
#include "../TShared/PackageType.h"
#include "../TShared/NetworkMessage_PingResponse.h"
#include "../TShared/NetworkMesssage_Connect.h"
#include "../TShared/NetworkMessage_Ping.h"
#include "../TShared/NetworkMessage_ConectResponse.h"
#include "../TShared/NetworkMessage_ChatMessage.h"
#include "../TShared/NetworkMessage_Position.h"


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
	DL_PRINT("Client trying to connect");

	SClientAdress newClient;
	newClient.myIP = anIp;
	newClient.myPort = aPort;
	newClient.myName = aName;

	ClientID newClientId = currentFreeId++;
	myClients[newClientId] = newClient;
	SNetworkPackageHeader acceptPackage;
	acceptPackage.myPackageType = static_cast<char>(ePackageType::eConnectResponse);
	acceptPackage.mySenderID = ID_SERVER;
	acceptPackage.myTargetID = newClientId;
	acceptPackage.myTimeStamp = myTimerManager.GetTimer(myTimerHandle).GetLifeTime().GetMilliseconds();

	CNetworkMessage_ConectResponse* message = myMessageManager.CreateMessage<CNetworkMessage_ConectResponse>(acceptPackage);

	message->myClientId = newClientId;

	myNetworkWrapper.Send(message, anIp, aPort);
	std::string temp;
	temp += "Client: " + aName + " connected";
	DL_PRINT(temp.c_str());
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
	header.myPackageType = static_cast<char>(ePackageType::ePing);
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
	std::string temp;
	temp += "Client " + myClients[aClient].myName + " disconected";
	DL_PRINT(temp.c_str());
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
			std::string temp;
			temp += "Client: " + myClients[pendingPing.first].myName + " not responding";
			DL_PRINT(temp.c_str());
			DisconectClient(pendingPing.first);
			break;
		}
	}
}

void CServerMain::SendTo(CNetworkMessage* aNetworkMessage)
{
	SNetworkPackageHeader header = aNetworkMessage->GetHeader();

	switch (aNetworkMessage->GetHeader().myTargetID)
	{
	case ID_ALL_BUT_ME:
		for (auto client : myClients)
		{
			if (client.first != header.mySenderID)
			{
				header.myTargetID = client.first;
				myNetworkWrapper.Send(aNetworkMessage, client.second.myIP.c_str(), client.second.myPort.c_str());
			}
		}
		break;
	case ID_SERVER:
		break;
	default:
		if (myClients.count(header.myTargetID) > 0)
		{
			myNetworkWrapper.Send(aNetworkMessage, myClients.at(header.myTargetID).myIP.c_str(), myClients.at(header.myTargetID).myIP.c_str());
		}
		break;
	}
}

void CServerMain::HandleChatMessage(CNetworkMessage_ChatMessage* aNetworkMessageChatMessage)
{
	std::string temp;
	temp += aNetworkMessageChatMessage->myChatMessage;
	DL_PRINT(temp.c_str());

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
		case ePackageType::eConnect:
			{
				std::string temp;
				temp += "Conect request recieved from IP ";
				temp += currentSenderIp;
				DL_PRINT(temp.c_str());

				CNetworkMessage_Connect* connectMessage = currentMessage->CastTo<CNetworkMessage_Connect>();
				ConnectClient(connectMessage->GetHeader(), connectMessage->myClientName, currentSenderIp, currentSenderPort);
				delete connectMessage;
			}
			break;
		case ePackageType::ePing:
			{
				//std::cout << "Ping message recievd from client " << std::endl;
				SNetworkPackageHeader newHeader;
				newHeader.myPackageType = static_cast<char>(ePackageType::ePingResponse);
				newHeader.mySenderID = ID_SERVER;
				newHeader.myTargetID = currentMessage->GetHeader().mySenderID;
				newHeader.myTimeStamp = 100;

				CNetworkMessage_PingResponse* newMessage = myMessageManager.CreateMessage<CNetworkMessage_PingResponse>(newHeader);
				myNetworkWrapper.Send(newMessage, currentSenderIp, currentSenderPort);
				delete newMessage;
			}
			break;
		case ePackageType::ePingResponse:
			{
				RecievePingResponse(currentMessage->GetHeader());
			}
			break;
		case ePackageType::eChat:
			{
				HandleChatMessage(currentMessage->CastTo<CNetworkMessage_ChatMessage>());
			}

		case ePackageType::ePosition:
			{
				CNetworkMessage_Position* position = currentMessage->CastTo<CNetworkMessage_Position>();

				std::string positionString;
				positionString += "Position: ";
				positionString += " X: ";
				positionString += position->GetPosition().x;
				positionString += " Y: ";
				positionString += position->GetPosition().y;
				positionString += " Z: ";
				positionString += position->GetPosition().z;

				DL_PRINT(positionString.c_str());

				SendTo(currentMessage);
			}
		case ePackageType::eZero:
		case ePackageType::eSize:
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
