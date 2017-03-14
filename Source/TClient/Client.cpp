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
#include "../ThreadedPostmaster/ConetctMessage.h"
#include "../ThreadedPostmaster/ConectedMessage.h"
#include "../TShared/NetworkMessage_LoadLevel.h"
#include "../ThreadedPostmaster/LoadLevelMessage.h"
#include "../ThreadedPostmaster/SetClientIDMessage.h"
#include "../Components/PlayerNetworkComponent.h"
#include "../ThreadedPostmaster/PlayerPositionMessage.h"
#include "../TShared/NetworkMessage_PlayerPositionMessage.h"
#include "../TShared/NetworkMessage_SpawnOtherPlayer.h"
#include "../TShared/NetworkMessage_ServerReady.h"
#include "../TShared/NetworkMessage_WeaponShoot.h"
#include "../ThreadedPostmaster/OtherPlayerSpawned.h"
#include "../ThreadedPostmaster/NetworkDebugInfo.h"
#include "../ThreadedPostmaster/GameEventMessage.h"
#include "../Game/GameEventMessenger.h"
#include "../CommonUtilities/StringHelper.h"
#include "../TShared/NetworkMessage_Disconected.h"


CClient::CClient() : myMainTimer(0), myState(eClientState::DISCONECTED), myId(0), myServerIp(""), myServerPingTime(0), myServerIsPinged(false), myPlayerPositionUpdated(false), myRoundTripTime(0)
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
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(L"Disconected From Server"));
	//myChat.StopChat();
}

void CClient::UpdatePing(const CU::Time& aTime)
{
	if (myServerIsPinged && myState == eClientState::CONECTED)
	{
		myServerPingTime += aTime.GetSeconds();

		if (myServerPingTime.GetSeconds() >= 20)
		{
			std::cout << "Server is not responding" << std::endl;
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(L"Server Not Responding"));
			Disconect();
		}
	}
}

void CClient::Ping()
{
	if (myState == eClientState::CONECTED && myServerIsPinged == false)
	{
		CNetworkMessage_Ping* tempMessagePing = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_Ping>("__Server");

		myNetworkWrapper.Send(tempMessagePing, myServerIp.c_str(), SERVER_PORT);

		myServerIsPinged = true;
		myServerPingTime = 0;
	}
}

void CClient::Update()
{
	float currentTime = 0.f;
	CU::Time positionWaitTime(0);

	CU::SetThreadName("ClientUpdate");
	myIsRunning = true;

	while (myIsRunning)
	{
		Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();

		if (myTimerManager.Size() == 0)
		{
			break;
		}
		myTimerManager.UpdateTimers();
		currentTime += myTimerManager.GetTimer(myMainTimer).GetDeltaTime().GetSeconds();
		const CU::Time deltaTime = myTimerManager.GetTimer(myMainTimer).GetDeltaTime();
		UpdatePing(myTimerManager.GetTimer(myMainTimer).GetDeltaTime());
		positionWaitTime += deltaTime;
		myServerPingTime += deltaTime;
		CNetworkMessage* currentMessage = myNetworkWrapper.Recieve(nullptr, nullptr);

		switch ((currentMessage->GetHeader().myPackageType))
		{
		case ePackageType::eConnectResponse:
			if (myState == eClientState::CONECTING)
			{
				CNetworkMessage_ConectResponse* conectResponseMessage = currentMessage->CastTo<CNetworkMessage_ConectResponse>();
				myId = conectResponseMessage->myClientId;
				myState = eClientState::CONECTED;

				std::cout << "Conected to server got id:" << myId << std::endl;
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CConectedMessage(myId));
			}
			break;
		case ePackageType::ePing:
			if (myState == eClientState::CONECTED)
			{
				//DL_PRINT("CLIENT:Ping");

				CNetworkMessage_PingResponse* newMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_PingResponse>("__Server");
				myNetworkWrapper.Send(newMessage, myServerIp.c_str(), SERVER_PORT);
			}
			break;
		case ePackageType::ePingResponse:
		{
			//DL_PRINT("CLIENT:PingRespons");
			myRoundTripTime = myServerPingTime.GetMilliseconds();
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
			CNetworkMessage_ServerReady* serverReady = currentMessage->CastTo<CNetworkMessage_ServerReady>();
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CServerReadyMessage(serverReady->GetNumberOfPlayers()));
		}
		break;
		case ePackageType::ePlayerPosition:
		{
			CNetworkMessage_PlayerPositionMessage* playerPosition = currentMessage->CastTo<CNetworkMessage_PlayerPositionMessage>();

			const unsigned ID = playerPosition->GetID();

			myNetworkRecieverComonents.at(ID)->GetParent()->SetWorldTransformation(playerPosition->GetTransformation());
			myNetworkRecieverComonents.at(ID)->GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		}
		break;
		case ePackageType::eSpawnOtherPlayer:
		{
			CNetworkMessage_SpawnOtherPlayer* spawnPlayer = currentMessage->CastTo<CNetworkMessage_SpawnOtherPlayer>();
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSpawnOtherPlayerMessage(spawnPlayer->GetPlayerID()));
		}
		break;
		case ePackageType::ePosition:
		{
			CNetworkMessage_Position *positionMessage = currentMessage->CastTo<CNetworkMessage_Position>();
			//std::cout << "Got position message with ID: " << positionMessage->GetID() << " and position: X:" << positionMessage->GetPosition().x << " Y:" << positionMessage->GetPosition().y << " Z:" << positionMessage->GetPosition().z << std::endl;

			CNetworkComponent* comp = CNetworkComponentManager::GetInstance()->GetComponent(positionMessage->GetID());
			//CU::Vector3f temp = comp->GetParent()->GetWorldPosition();
			comp->GetParent()->SetWorldPosition(positionMessage->GetPosition());
			//CU::Vector3f temp2 = comp->GetParent()->GetWorldPosition();
			comp->GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());

		}
		break;
		case ePackageType::eLoadLevel:
		{
			CNetworkMessage_LoadLevel *loadLevelMessage = currentMessage->CastTo<CNetworkMessage_LoadLevel>();
			
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CLoadLevelMessage(loadLevelMessage->myLevelIndex));
		}
		break;
		case ePackageType::eWeaponShoot:
		{
			CNetworkMessage_WeaponShoot* shoot = currentMessage->CastTo<CNetworkMessage_WeaponShoot>();

			SComponentMessageData data;
			SComponentMessageData data2;
			data2.myInt = shoot->GetWeaponIndex();
			myNetworkRecieverComonents.at(shoot->GetHeader().mySenderID)->GetParent()->NotifyComponents(eComponentMessageType::eSelectWeapon, data2);
			data.myVector3f = shoot->GetDirection();
			myNetworkRecieverComonents.at(shoot->GetHeader().mySenderID)->GetParent()->NotifyComponents(eComponentMessageType::eShootWithNetworking, data);
		}
		break;
		case ePackageType::eConnect:
			{
				CNetworkMessage_Connect* conectMessage = currentMessage->CastTo<CNetworkMessage_Connect>();
				std::wstring string;
				string += L"Player ";
				string += CU::StringToWString(conectMessage->myClientName);
				string += L" has conected!";
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(string));
			}
			break;
		case ePackageType::eDisconected:
			{
				CNetworkMessage_Disconected* disconectedMessage = currentMessage->CastTo<CNetworkMessage_Disconected>();
				std::wstring string;
				string += L"Player ";
				string += CU::StringToWString(disconectedMessage->GetClientName());
				string += L" has disconected!";
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(string));
			}
			break;
		case ePackageType::eZero:
		case ePackageType::eSize:
		default: break;
		}


		if (currentTime > 1.f)
		{
			Ping();
			currentTime = 0.f;

			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CNetworkDebugInfo(myNetworkWrapper.GetAndClearDataSent(), myRoundTripTime));
		}

		if (positionWaitTime.GetMilliseconds() > 32 && myPlayerPositionUpdated == true)
		{
			CNetworkMessage_PlayerPositionMessage * message = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_PlayerPositionMessage>("__All");

			message->SetTransformation(myLatestPlayerTransform);
			//message->SetPosition(myLatestPlayerPosition);
			message->SetID(myId);

			Send(message);
			myPlayerPositionUpdated = false;
			positionWaitTime = 0;
		}
	}

	myCanQuit = true;
}

void CClient::Send(CNetworkMessage* aNetworkMessage)
{
	myNetworkWrapper.Send(aNetworkMessage, myServerIp.c_str(), SERVER_PORT);
}

bool CClient::Connect(const char* anIp, std::string aClientName)
{
	myName = aClientName;
	myState = eClientState::CONECTING;

	myServerIp = anIp;

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

eMessageReturn CClient::DoEvent(const CConectMessage& aConectMessage)
{
	Connect(aConectMessage.myIP.c_str(), aConectMessage.myName);
	return eMessageReturn::eContinue;
}

eMessageReturn CClient::DoEvent(const CSetClientIDMessage & aMessage)
{
	aMessage.GetPlayerComponent()->SetID(myId);
	return eMessageReturn();
}

eMessageReturn CClient::DoEvent(const CPlayerPositionMessage& aMessage)
{
	if (aMessage.myId == myId && myLatestPlayerTransform != aMessage.myTransform)
	{
		myLatestPlayerTransform = aMessage.myTransform;
		myPlayerPositionUpdated = true;
	}
	return eMessageReturn::eContinue;
}

eMessageReturn CClient::DoEvent(const COtherPlayerSpawned& aMassage)
{
	myNetworkRecieverComonents[aMassage.GetComponent()->GetPlayerID()] = aMassage.GetComponent();
	return eMessageReturn::eStop;
}
