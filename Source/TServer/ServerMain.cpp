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

#include "GameServer.h"
#include "../TShared/NetworkMessage_LoadLevel.h"
#include <array>
#include "../TShared/NetworkMessage_ClientReady.h"

#include "../KevinLoader/KevinLoader.h"
#include "../TShared/NetworkMessage_ServerReady.h"
#include "../TShared/NetworkMessage_ClientReady.h"
#include "ServerMessageManager.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/PostOffice.h"
#include "ThreadedPostmaster/PrintMessage.h"

std::thread* locLoadingThread = nullptr;

CServerMain::CServerMain() : myTimerHandle(0), myImportantCount(0), currentFreeId(ID_FREE), myServerState(eServerState::eWaitingForClients), myGameServer(nullptr)
{
	KLoader::CKevinLoader::CreateInstance();
	CServerMessageManager::CreateInstance(*this);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this,eMessageType::eNetworkMessage);

	myIsRunning = false;
	myCanQuit = false;
}


CServerMain::~CServerMain()
{
	myIsRunning = false;
	while (!myCanQuit) continue;

	KLoader::CKevinLoader::DestroyInstance();
	CServerMessageManager::DestroyInstance();
}

void CServerMain::StartServer()
{
	myTimerHandle = myTimerManager.CreateTimer();
	myNetworkWrapper.Init(DEFAULT_PORT, CServerMessageManager::GetInstance());
	myMessageManager = CServerMessageManager::GetInstance();

	myGameServer = new CGameServer();

	myGameServer->Init();

	Update();
}

void CServerMain::UpdateImportantMessages(const CU::Time aDeltaTime)
{
	for (auto pair : myImportantMessages)
	{
		SImportantWaitData& waitData = myImportantMessages.at(pair.first);

		waitData.myWaitedTime += aDeltaTime;
		if (waitData.myWaitedTime.GetMilliseconds() > 75)
		{
			SendTo(myMessageManager->CreateMessage(waitData.myMessage), true);
			waitData.myWaitedTime = 0;
		}
	}
}

void CServerMain::RecieveImportantResponse(CImportantNetworkMessage* aNetworkMessage)
{
	const int &importantID = aNetworkMessage->GetImportantId();
	SImportantWaitData waitData = myImportantMessages.at(importantID);

	myImportantMessages.erase(importantID);

	const SNetworkPackageHeader& header = waitData.myMessage.myHeader;
	myClients.at(header.myTargetID).ResponseTime = waitData.myWaitedTime.GetMilliseconds();

}

void CServerMain::ConnectClient(SNetworkPackageHeader aHeader, std::string aName, const char* anIp, const char* aPort)
{
	DL_PRINT("Client trying to connect");

	SClientData newClient;
	newClient.myIP = anIp;
	newClient.myPort = aPort;
	newClient.myName = aName;
	newClient.IsReady = false;

	ClientID newClientId = currentFreeId++;
	myClients[newClientId] = newClient;
	SNetworkPackageHeader acceptPackage;
	acceptPackage.myPackageType = (ePackageType::eConnectResponse);
	acceptPackage.mySenderID = ID_SERVER;
	acceptPackage.myTargetID = newClientId;
	acceptPackage.myTimeStamp = myTimerManager.GetTimer(myTimerHandle).GetLifeTime().GetMilliseconds();

	CNetworkMessage_ConectResponse* message = myMessageManager->CreateMessage<CNetworkMessage_ConectResponse>(acceptPackage);

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
	header.myPackageType = (ePackageType::ePing);
	header.mySenderID = ID_SERVER;
	header.myTargetID = aClientID;

	SClientData client = myClients[aClientID];

	CNetworkMessage_Ping* messagePing = myMessageManager->CreateMessage<CNetworkMessage_Ping>(header);

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
	header.myPackageType = (ePackageType::eChat);
	header.myTimeStamp = GetCurrentTime();

	CNetworkMessage_ChatMessage* leaveMessage = myMessageManager->CreateMessage<CNetworkMessage_ChatMessage>(header);
	leaveMessage->myChatMessage = ("Server: " + leftClientsName + " has lef the server");


	for (auto client : myClients)
	{
		header.myTargetID = client.first;
		myNetworkWrapper.Send(leaveMessage, client.second.myIP.c_str(), client.second.myPort.c_str());
	}
}

void CServerMain::UpdatePing(CU::Time aDeltaTime)
{
	/*for (auto pendingPing : myPendingPings)
	{
		myPendingPings[pendingPing.first] += aDeltaTime.GetSeconds();

		if (myPendingPings[pendingPing.first] >= 100)
		{
			std::string temp;
			temp += "Client: " + myClients[pendingPing.first].myName + " not responding";
			DL_PRINT(temp.c_str());
			DisconectClient(pendingPing.first);
			break;
		}
	}*/
}

void CServerMain::SendTo(CNetworkMessage* aNetworkMessage, bool aIsResend)
{
	SNetworkPackageHeader header = aNetworkMessage->GetHeader();

	if (aNetworkMessage->IsImportant() == true && aIsResend == false)
	{
		CImportantNetworkMessage* const importantMessage = static_cast<CImportantNetworkMessage*>(aNetworkMessage);
		importantMessage->SetImportantId(myImportantCount);
		SImportantWaitData waitData(importantMessage);

		myImportantMessages[myImportantCount] = waitData;

		myImportantCount += 1;
	}

	switch (aNetworkMessage->GetHeader().myTargetID)
	{
	case ID_ALL_BUT_ME:
	case ID_ALL:
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
}

bool CServerMain::CheckIfClientsReady() const
{
	for (auto client : myClients)
	{
		if (client.second.IsReady == false)// || myClients.size() < 2)
		{
			return false;
		}
	}
	return true;
}

void CServerMain::StartGame()
{
	myServerState = eServerState::eInGame;

	SNetworkPackageHeader header;
	header.myTargetID = ID_ALL;
	header.mySenderID = ID_SERVER;
	header.myPackageType = (ePackageType::eServerReady);
	header.myTimeStamp = GetCurrentTime();

	CNetworkMessage_ServerReady* message = myMessageManager->CreateMessage<CNetworkMessage_ServerReady>(header);

	SendTo(message);
}

bool CServerMain::Update()
{
	float currentTime = 0.f;

	myIsRunning = true;
	while (myIsRunning)
	{
		Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();

		myTimerManager.UpdateTimers();
		currentTime += myTimerManager.GetTimer(myTimerHandle).GetDeltaTime().GetSeconds();

		const CU::Time deltaTime = myTimerManager.GetTimer(myTimerHandle).GetDeltaTime();

		UpdatePing(myTimerManager.GetTimer(myTimerHandle).GetDeltaTime());
		UpdateImportantMessages(myTimerManager.GetTimer(myTimerHandle).GetDeltaTime().GetSeconds());

		char* currentSenderIp = nullptr;
		char* currentSenderPort = nullptr;

		CNetworkMessage* currentMessage = myNetworkWrapper.Recieve(&currentSenderIp, &currentSenderPort);

		if (currentMessage->IsImportant() && static_cast<ePackageType>(currentMessage->GetHeader().myPackageType) != ePackageType::eImportantResponse)
		{
			CImportantNetworkMessage* importantNetworkMessage = static_cast<CImportantNetworkMessage*>(currentMessage);
			importantNetworkMessage->UnpackMessage();

			SNetworkPackageHeader header;
			header.myTargetID = currentMessage->GetHeader().mySenderID;
			header.mySenderID = ID_SERVER;
			header.myPackageType = (ePackageType::eImportantResponse);
			header.myTimeStamp = static_cast<int>(GetCurrentTime());

			CImportantNetworkMessage* ResponseMessage = myMessageManager->CreateMessage<CImportantNetworkMessage>(header);
			SendTo(ResponseMessage);
		}

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
		}
		break;
		case ePackageType::ePing:
		{
			//std::cout << "Ping message recievd from client " << std::endl;
			//DL_PRINT("SERVER:Ping");
			SNetworkPackageHeader newHeader;
			newHeader.myPackageType = (ePackageType::ePingResponse);
			newHeader.mySenderID = ID_SERVER;
			newHeader.myTargetID = currentMessage->GetHeader().mySenderID;
			newHeader.myTimeStamp = 100;

			CNetworkMessage_PingResponse* newMessage = myMessageManager->CreateMessage<CNetworkMessage_PingResponse>(newHeader);
			myNetworkWrapper.Send(newMessage, currentSenderIp, currentSenderPort);
		}
		break;
		case ePackageType::ePingResponse:
		{
			//DL_PRINT("SERVER:PingRespons");
			RecievePingResponse(currentMessage->GetHeader());
		}
		break;
		case ePackageType::eChat:
		{
			HandleChatMessage(currentMessage->CastTo<CNetworkMessage_ChatMessage>());
		}
		break;
		case ePackageType::ePlayerPosition:
		{

		}
		break;
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
		break;
		case ePackageType::eImportantResponse:
			RecieveImportantResponse(currentMessage->CastTo<CImportantNetworkMessage>());
			break;
		case ePackageType::eLoadLevel:
			if (currentMessage->GetHeader().mySenderID == ID_FREE && myServerState == eServerState::eWaitingForClients)
			{
				for (auto client : myClients)
				{
					myClients.at(client.first).IsReady = false;
				}
				myServerState = eServerState::eLoadingLevel;
				CNetworkMessage_LoadLevel *loadLevelMessage = currentMessage->CastTo<CNetworkMessage_LoadLevel>();
				//myGameServer->Load(loadLevelMessage->myLevelIndex);
				locLoadingThread = new std::thread(&CGameServer::Load, myGameServer, loadLevelMessage->myLevelIndex);
			}
			break;
		case ePackageType::eClientReady:
			if (myServerState == eServerState::eLoadingLevel)
			{
				myClients.at(currentMessage->GetHeader().mySenderID).IsReady = true;

				//if (CheckIfClientsReady() == true)
				//{
				//	StartGame();
				//}
			}
			break;
		case ePackageType::eZero:
		case ePackageType::eSize:
		default: break;
		}

		delete currentSenderIp;
		delete currentSenderPort;

		if (currentTime > 1.f)
		{
			Ping();
			currentTime = 0.f;
		}

		if (myServerState == eServerState::eLoadingLevel)
		{
			if (myGameServer && myGameServer->IsLoaded())
			{
				if (CheckIfClientsReady() == true)
				{
					if (locLoadingThread)
					{
						locLoadingThread->join();
						delete locLoadingThread;
						locLoadingThread = nullptr;

						StartGame();
					}
				}
			}
		}

		if (myServerState == eServerState::eInGame)
		{
			myGameServer->Update(deltaTime);
		}
	}

	myCanQuit = true;

	return false;
}

eMessageReturn CServerMain::DoEvent(const CSendNetowrkMessageMessage& aSendNetowrkMessageMessage)
{
	SendTo(aSendNetowrkMessageMessage.UnpackHolder());

	/*CNetworkMessage* temp = aSendNetowrkMessageMessage.UnpackHolder();
	if (temp->GetHeader().myPackageType == static_cast<char>(ePackageType::ePosition))
	{
		CNetworkMessage_Position* position = temp->CastTo<CNetworkMessage_Position>();
		int x = position->GetPosition().x;
		int y = position->GetPosition().y;
		int z = position->GetPosition().z;
	}*/

	return eMessageReturn::eContinue;
}
