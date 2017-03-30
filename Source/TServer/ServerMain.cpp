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
#include "../TShared/NetworkMessage_PickupHealth.h"
#include "../TShared/NetworkMessage_PickupAmmo.h"
#include "../TShared/Networkmessage_PickupArmor.h"
#include "../TShared/NetworkMessage_PickupKey.h"
#include "../TShared/NetworkMessage_SetCheckpointMessage.h"


#include "GameServer.h"
#include "../TShared/NetworkMessage_LoadLevel.h"
#include <array>

#include "../KevinLoader/KevinLoader.h"
#include "../TShared/NetworkMessage_ServerReady.h"
#include "ServerMessageManager.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/PostOffice.h"
#include "../ThreadedPostmaster/PrintMessage.h"
#include "../TShared/NetworkMessage_SpawnOtherPlayer.h"
#include "../ThreadedPostmaster/PlayerPositionMessage.h"
#include "../TShared/NetworkMessage_PlayerPositionMessage.h"
#include "../TShared/NetworkMessage_WeaponShoot.h"
#include "../Components/GameObject.h"
#include "../CommonUtilities/StringHelper.h"
#include "../TShared/NetworkMessage_Disconected.h"
#include "../CommonUtilities/StringHelper.h"
#include "../TShared/NetworkMessage_TakeDamage.h"
#include "../TShared/NetworkMessage_DoorMessage.h"
#include "../TShared/NetworkMessage_PlayerDied.h"
#include "../TShared/NetworkMessage_PlayerRespawned.h"
#include "../TShared/NetworkMessage_ResetToCheckpoint.h"
#include "../TShared/NetworkMessage_RevivePlayer.h"
#include "../Physics/PhysXHelper.h"
#include "../Components/HealthComponentManager.h"
#include "../Components/ComponentMessage.h"
#include "../ThreadedPostmaster/ResetToCheckPointMessage.h"

std::thread* locLoadingThread = nullptr;

CServerMain::CServerMain() : myTimerHandle(0), myImportantCount(0), currentFreeId(ID_FREE), myServerState(eServerState::eWaitingForClients), myGameServer(nullptr)
{
	KLoader::CKevinLoader::CreateInstance();
	CServerMessageManager::CreateInstance(*this);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);

	myIsRunning = false;
	myCanQuit = false;
	
	CU::CJsonValue playerControls;
	std::string errorMessage = playerControls.Parse("Json/Player/playerData.json");
	if (!errorMessage.empty())
	{
		DL_PRINT_WARNING("Could not load %s, using default values", errorMessage.c_str());

		myPlayerRespawnTime = 2.0f;
	}
	else
	{
		myPlayerRespawnTime = playerControls["RespawnTime"].GetFloat();
	}
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

ClientID CServerMain::ConnectClient(SNetworkPackageHeader aHeader, std::string aName, const char* anIp, const char* aPort)
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
	return newClientId;
}

void CServerMain::RecievePingResponse(SNetworkPackageHeader aHeader)
{

	if (myPendingPings.count(aHeader.mySenderID) > 0)
	{
		myClients.at(aHeader.mySenderID).ResponseTime = myPendingPings.at(aHeader.mySenderID);
		myPendingPings.erase(aHeader.mySenderID);
	}
}


void CServerMain::Ping(ClientID aClientID)
{
	//Ping was my best best friend growing up
	//though Ping stole my gir....
	//Now I'm on a quest to slay the dark overlord Ping to restore balnce to the force!
	CNetworkMessage_Ping* pingMessage = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_Ping>(aClientID);

	SendTo(pingMessage);

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

	CNetworkMessage_Disconected* disconectedMessage = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_Disconected>(ID_ALL);

	disconectedMessage->SetClientName(leftClientsName);
	SendTo(disconectedMessage);
}

void CServerMain::UpdatePing(CU::Time aDeltaTime)
{
	for (auto pendingPing : myPendingPings)
	{
		if (myPendingPings[pendingPing.first].GetSeconds() >= 0)
		{
			myPendingPings[pendingPing.first] += aDeltaTime.GetSeconds();
		}

		if (myPendingPings[pendingPing.first].GetSeconds() >= 10)
		{
			std::string temp;
			temp += "Client: " + myClients[pendingPing.first].myName + " not responding";
			DL_PRINT(temp.c_str());
			DisconectClient(pendingPing.first);
			break;
		}
	}
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
			myNetworkWrapper.Send(aNetworkMessage, myClients.at(header.myTargetID).myIP.c_str(), myClients.at(header.myTargetID).myPort.c_str());
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

	message->SetNumberOfPlayers(myClients.size() - 1);

	SendTo(message);

	for (auto client : myClients)
	{
		CServerPlayerNetworkComponent* playerNetworkComponent = myGameServer->AddPlayer(client.first);

		myClients.at(client.first).myComponent = playerNetworkComponent;

		for (auto secondClient : myClients)
		{
			if (secondClient.first != client.first)
			{
				CNetworkMessage_SpawnOtherPlayer* message2/*the sequel to message*/ = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_SpawnOtherPlayer>(secondClient.first);

				message2->SetPlayerId(client.first);

				SendTo(message2);
			}
		}
	}
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

		for (auto deadPlayer : myDeadPlayers)
		{
			
			myDeadPlayers.at(deadPlayer.first) -= deltaTime.GetSeconds();
			if(deadPlayer.second <= 0)
			{
				CNetworkMessage_RevivePlayer* revivePlayerMessage = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_RevivePlayer>(deadPlayer.first);
				SendTo(revivePlayerMessage);
			}
		}

		UpdateImportantMessages(myTimerManager.GetTimer(myTimerHandle).GetDeltaTime().GetSeconds());

		char* currentSenderIp = nullptr;
		char* currentSenderPort = nullptr;

		CNetworkMessage* currentMessage = myNetworkWrapper.Recieve(&currentSenderIp, &currentSenderPort);

		while (currentMessage->GetHeader().myPackageType != ePackageType::eZero)
		{
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
				ClientID clientId = ConnectClient(connectMessage->GetHeader(), connectMessage->myClientName, currentSenderIp, currentSenderPort);

				SNetworkPackageHeader header = connectMessage->GetHeader();
				header.myTargetID = ID_ALL_BUT_ME;
				header.mySenderID = clientId;
				connectMessage->SetHeader(header);

				myAlivePlayers.emplace(connectMessage->GetHeader().mySenderID, 1);

				SendTo(connectMessage);
			}
			break;
			case ePackageType::ePing:
			{
				CNetworkMessage_PingResponse* pingResponse = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_PingResponse>(currentMessage->GetHeader().mySenderID);
				SendTo(pingResponse);
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
				if (myServerState == eServerState::eInGame && myGameServer->IsLoaded())
				{
					CNetworkMessage_PlayerPositionMessage* positionMessage = currentMessage->CastTo<CNetworkMessage_PlayerPositionMessage>();

					const unsigned ID = positionMessage->GetID();

					CGameObject*const gameObject = myClients.at(ID).myComponent->GetParent();
					gameObject->SetWorldTransformation(positionMessage->GetTransformation());
					gameObject->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
					gameObject->SetName("Spelaren");

					SendTo(positionMessage);
				}
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
			case ePackageType::eWeaponShoot:
			{
				CNetworkMessage_WeaponShoot* shoot = currentMessage->CastTo<CNetworkMessage_WeaponShoot>();

				SendTo(shoot);
			}
			break;
			case ePackageType::eImportantResponse:
				RecieveImportantResponse(currentMessage->CastTo<CImportantNetworkMessage>());
				break;
			case ePackageType::eLoadLevel:
				if (myServerState == eServerState::eInGame)
				{
					myGameServer->ReInit();
					myServerState = eServerState::eWaitingForClients;
				}
				if (currentMessage->GetHeader().mySenderID == ID_FREE && myServerState == eServerState::eWaitingForClients)
				{
					for (auto& client : myClients)
					{
						client.second.IsReady = false;
						client.second.myComponent = nullptr;
					}
					myServerState = eServerState::eLoadingLevel;
					CNetworkMessage_LoadLevel *loadLevelMessage = currentMessage->CastTo<CNetworkMessage_LoadLevel>();
					myGameServer->CreateManagersAndFactories();
					locLoadingThread = new std::thread(&CGameServer::Load, myGameServer, loadLevelMessage->myLevelIndex);


					SendTo(loadLevelMessage);

				}
				break;
			case ePackageType::eClientReady:
				if (myServerState == eServerState::eLoadingLevel)
				{
					myClients.at(currentMessage->GetHeader().mySenderID).IsReady = true;
				}
				break;
			case ePackageType::eDisconected:
			{
				DisconectClient(currentMessage->GetHeader().mySenderID);
			}
			break;
			case ePackageType::eTakeDamage:
			{
				CNetworkMessage_TakeDamage* damage = currentMessage->CastTo<CNetworkMessage_TakeDamage>();
				CHealthComponentManager::GetInstance()->TakeDamage(damage->GetID(), damage->GetDamageTaken());
				SendTo(damage);
			}
			break;
			case ePackageType::ePickupHealth:
			{
				CNetworkMessage_PickupHealth* pickup = currentMessage->CastTo<CNetworkMessage_PickupHealth>();
				SendTo(pickup);
			}
			break;
			case ePackageType::ePickupAmmo:
			{
				CNetWorkMessage_PickupAmmo* pickup = currentMessage->CastTo<CNetWorkMessage_PickupAmmo>();
				SendTo(pickup);
			}
			break;
			case ePackageType::ePickupArmor:
			{
				CNetworkmessage_PickupArmor* pickup = currentMessage->CastTo<CNetworkmessage_PickupArmor>();
				SendTo(pickup);
			}
			break;
			case ePackageType::ePickupKey:
			{
				CNetworkMessage_PickupKey* pickup = currentMessage->CastTo<CNetworkMessage_PickupKey>();
				SendTo(pickup);
			}
			break;
			case ePackageType::eDoorMessage:
			{
				CNetworkMessage_DoorMessage* doormessage = currentMessage->CastTo<CNetworkMessage_DoorMessage>();
				SendTo(doormessage);
			}
			break;
			case ePackageType::eSetCheckpointMessage:
			{
				CNetworkMessage_SetCheckpointMessage* doormessage = currentMessage->CastTo<CNetworkMessage_SetCheckpointMessage>();
				SendTo(doormessage);
			}
			break;
			case ePackageType::ePlayerDied:
			{
				CNetworkMessage_PlayerDied* playerDied = currentMessage->CastTo<CNetworkMessage_PlayerDied>();

				myAlivePlayers.erase(playerDied->GetHeader().mySenderID);
				myDeadPlayers.emplace(playerDied->GetHeader().mySenderID, myPlayerRespawnTime);
				if (myAlivePlayers.size() <= 0)
				{
					CNetworkMessage_ResetToCheckpoint* reset = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_ResetToCheckpoint>(ID_ALL);

					SendTo(reset);
					Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CResetToCheckPointMessage());
				}
			}
			break;
			case ePackageType::ePlayerRespawned:
			{
				CNetworkMessage_PlayerRespawned* playerDied = currentMessage->CastTo<CNetworkMessage_PlayerRespawned>();

				myDeadPlayers.erase(playerDied->GetHeader().mySenderID);
				myAlivePlayers.emplace(playerDied->GetHeader().mySenderID, myPlayerRespawnTime);
			}
			break;
			case ePackageType::eZero:
			case ePackageType::eSize:
			default: break;
			}

			delete currentSenderIp;
			delete currentSenderPort;

			currentMessage = myNetworkWrapper.Recieve(&currentSenderIp, &currentSenderPort);
		}

		


		if (currentTime > 1.f)
		{
			Ping();
			currentTime = 0.f;
			PrintDebugInfo();
		}

		if (myServerState == eServerState::eLoadingLevel)
		{
			if (myGameServer && myGameServer->IsLoaded())
			{
				if (CheckIfClientsReady() == true)
				{
					if (locLoadingThread != nullptr)
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
		std::this_thread::yield();
	}

	myCanQuit = true;

	return false;
}
#include "../TShared/NetworkMessage_EnemyTransformation.h"
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

void CServerMain::PrintDebugInfo()
{
	std::wstring dataInfo;
	dataInfo = L"Data sent(kB/s): ";
	dataInfo += CU::StringHelper::ToWStringWithPrecision(static_cast<float>(myNetworkWrapper.GetAndClearDataSent()) / 1000, 3);
	DL_PRINT(dataInfo.c_str());

	for (auto client : myClients)
	{
		std::wstring clientData = L"Client ";
		clientData += CU::StringToWString(client.second.myName);
		clientData += L" Ping(ms): ";
		clientData += CU::StringHelper::ToWStringWithPrecision(client.second.ResponseTime.GetMilliseconds(), 1);
		DL_PRINT(clientData.c_str());
	}
}
