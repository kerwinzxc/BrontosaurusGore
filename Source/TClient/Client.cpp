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
#include "../ThreadedPostmaster/ResetToCheckPointMessage.h"
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
#include "../TShared/NetworkMessage_PickupHealth.h"
#include "../TShared/NetWorkMessage_PickupAmmo.h"
#include "../TShared/NetWorkmessage_PickupArmor.h"
#include "../TShared/Networkmessage_pickupkey.h"
#include "../TShared/NetworkMessage_DoorMessage.h"
#include "../TShared/NetworkMessage_SetCheckpointMessage.h"
#include "../TShared/NetworkMessage_ResetToCheckpoint.h"
#include "../TShared/NetworkMessage_SpawnEnemyRepesention.h"
#include "../TShared/NetworkMessage_SetIsRepesentationActive.h"
#include "../Components/DoorManager.h"
#include "../Game/EnemyFactory.h"

#include "../Components/PickupComponentManager.h"
#include "../TShared/NetworkMessage_EnemyPosition.h"
#include "..\TShared\NetworkMessage_EnemyTransformation.h"
#include "../Components/EnemyClientRepresentationManager.h"

#include "../Components/HealthComponentManager.h"
#include "../TShared/NetworkMessage_TakeDamage.h"
#include "../Components/CheckpointComponentManager.h"
#include "../Components/CheckPointComponent.h"

#include "../Components/NetworkPlayerReciverComponent.h"
#include "../ThreadedPostmaster/RevivePlayerMessage.h"

#include "../Game/PollingStation.h"

//temp!!! hoppas jag...
#include "../CommonUtilities/JsonValue.h"
#include "../CommonUtilities/WindowsHelper.h"


CClient::CClient() : myMainTimer(0), myState(eClientState::DISCONECTED), myId(0), myServerIp(""), myServerPingTime(0), myServerIsPinged(false), myPlayerPositionUpdated(false), myRoundTripTime(0)
{

	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eChangeLevel);
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

	std::string processName = "TServer_Applictaion_x64_";

#ifdef _DEBUG
	processName += "Debug";
#elif defined(RETAIL)
	processName += "Retail";
#elif defined(NDEBUG)
	processName += "Release";
#endif

	processName += ".exe";
	WindowsHelper::CloseProgram(processName);
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
		
		positionWaitTime += deltaTime;
		myServerPingTime += deltaTime;
		CNetworkMessage* currentMessage = myNetworkWrapper.Recieve(nullptr, nullptr);

		while (currentMessage->GetHeader().myPackageType != ePackageType::eZero)
		{
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

				if(myNetworkRecieverComponents.find(ID) != myNetworkRecieverComponents.end())
				{
					//myNetworkRecieverComponents.at(ID)->GetParent()->GetLocalTransform() = playerPosition->GetTransformation();

					/*CU::CJsonValue playerControls;
					std::string errorMessage = playerControls.Parse("Json/Player/Controls.json");
					myNetworkRecieverComponents.at(ID)->GetParent()->GetLocalTransform().SetRotation(playerPosition->GetTransformation());
					myNetworkRecieverComponents.at(ID)->GetParent()->GetLocalTransform().LerpPosition(playerPosition->GetTransformation().GetPosition(), 0);*/

					myNetworkRecieverComponents.at(ID)->GetParent()->GetLocalTransform().SetRotation(playerPosition->GetTransformation());
					myNetworkRecieverComponents.at(ID)->SetInpolationPosition(playerPosition->GetTransformation().GetPosition());
					//myNetworkRecieverComponents.at(ID)->GetParent()->GetLocalTransform() = playerPosition->GetTransformation();

					//myNetworkRecieverComponents.at(ID)->GetParent()->GetLocalTransform().SetRotation(playerPosition->GetTransformation());

					/*CU::CJsonValue playerControls;
					std::string errorMessage = playerControls.Parse("Json/Player/Controls.json");

					myNetworkRecieverComponents.at(ID)->GetParent()->GetLocalTransform().GetPosition().InterPolateTowards(playerPosition->GetTransformation().GetPosition(), playerControls["MaxSpeed"].GetFloat());*/
					myNetworkRecieverComponents.at(ID)->GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
				}
				



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
				myNetworkRecieverComponents.clear();
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
				data.myVector3f = shoot->GetDirection();

				switch (shoot->GetShooter())
				{
				case CNetworkMessage_WeaponShoot::Shooter::Player:
					myNetworkRecieverComponents.at(shoot->GetHeader().mySenderID)->GetParent()->NotifyComponents(eComponentMessageType::eSelectWeapon, data2);
					myNetworkRecieverComponents.at(shoot->GetHeader().mySenderID)->GetParent()->NotifyComponents(eComponentMessageType::eShootWithNetworking, data);
					break;
				case CNetworkMessage_WeaponShoot::Shooter::Enemy:
				{
					CEnemyClientRepresentation& target = CEnemyClientRepresentationManager::GetInstance().GetRepresentation(shoot->GetId());
					target.GetParent()->NotifyComponents(eComponentMessageType::eSelectWeapon, data2);
					target.GetParent()->NotifyComponents(eComponentMessageType::eShootWithNetworking, data);
				}
				break;
				default: break;
				}


			}
			break;
			case ePackageType::ePickupHealth:
			{
				CNetworkMessage_PickupHealth* pickup = currentMessage->CastTo<CNetworkMessage_PickupHealth>();
				CPickupComponentManager::GetInstance()->DeactivateHealthPack(pickup->GetID());
			}
			break;
			case ePackageType::eSetCheckpointMessage:
			{
				CNetworkMessage_SetCheckpointMessage* checkpointMEssage = currentMessage->CastTo<CNetworkMessage_SetCheckpointMessage>();
				CCheckPointComponent* checkpointComponent = CCheckpointComponentManager::GetInstance()->GetComponent(checkpointMEssage->GetId());
				checkpointComponent->SetAsNewCheckPointWithNetwork();
			}
			break;
			case ePackageType::ePickupAmmo:
			{
				CNetWorkMessage_PickupAmmo* pickup = currentMessage->CastTo<CNetWorkMessage_PickupAmmo>();
				CPickupComponentManager::GetInstance()->DeactivateAmmoPack(pickup->GetID());
			}
			break;
			case ePackageType::ePickupArmor:
			{
				CNetworkmessage_PickupArmor* pickup = currentMessage->CastTo<CNetworkmessage_PickupArmor>();
				CPickupComponentManager::GetInstance()->DeactivateArmorPack(pickup->GetID());
			}
			break;
			case ePackageType::ePickupKey:
			{
				CNetworkMessage_PickupKey* pickup = currentMessage->CastTo<CNetworkMessage_PickupKey>();
				CPickupComponentManager::GetInstance()->DeactivateKeyPickup(pickup->GetNetWorkID());
				CPollingStation::GetInstance()->AddKey(pickup->GetLockID());
			}
			break;
			case ePackageType::eDoorMessage:
			{
				CNetworkMessage_DoorMessage* doorMesssage = currentMessage->CastTo<CNetworkMessage_DoorMessage>();
				switch (doorMesssage->GetDoorAction())
				{
				case eDoorAction::eClose:
					CDoorManager::GetInstance()->CloseDoor(doorMesssage->GetNetworkID());
					break;
				case eDoorAction::eOpen:
					CDoorManager::GetInstance()->OpenDoor(doorMesssage->GetNetworkID());
					break;
				case eDoorAction::eUnlock:
					CDoorManager::GetInstance()->UnlockDoor(doorMesssage->GetNetworkID());
					break;
				default:
					break;
				}
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
			case ePackageType::eEnemyPosition:
			{
				CNetworkMessage_EnemyPosition* message = currentMessage->CastTo<CNetworkMessage_EnemyPosition>();
				CEnemyClientRepresentation& target = CEnemyClientRepresentationManager::GetInstance().GetRepresentation(message->GetId());
				target.GetParent()->SetWorldPosition(message->GetPosition()); // doesn't get sent anymore :/
				target.GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
			}
			break;
			case ePackageType::eEnemyTransformaion:
			{
				CNetworkMessage_EnemyTransformation* message = currentMessage->CastTo<CNetworkMessage_EnemyTransformation>();
				CEnemyClientRepresentation& target = CEnemyClientRepresentationManager::GetInstance().GetRepresentation(message->GetId());
				target.SetFutureMatrix(message->GetTransformation());
				target.GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
				SComponentMessageData positiondata;
				positiondata.myVector3f = message->GetTransformation().GetPosition();
				//target.GetParent()->NotifyComponents(eComponentMessageType::eSetControllerPosition, positiondata);
			}
			break;
			case ePackageType::eTakeDamage:
			{
				CNetworkMessage_TakeDamage* message = currentMessage->CastTo<CNetworkMessage_TakeDamage>();

				if (message->GetID() == -1)
				{
					SComponentMessageData data;
					data.myInt = message->GetDamageTaken();
					CPollingStation::GetInstance()->GetPlayerObject()->NotifyComponents(eComponentMessageType::eTakeDamage, data);
					break;
				}
				CHealthComponentManager::GetInstance()->TakeDamage(message->GetID(), message->GetDamageTaken());
			}
			break;
			case ePackageType::eResetToCheckpoint:
			{
				CNetworkMessage_ResetToCheckpoint* reset = currentMessage->CastTo<CNetworkMessage_ResetToCheckpoint>();

				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CResetToCheckPointMessage());
			}
			break;
			case ePackageType::eRevivePlayer:
			{
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CRevivePlayerMessage());
			}
			break;
			case  ePackageType::eSpawnEnemyRepresentation:
			{
				CNetworkMessage_SpawnEnemyRepesention* enemyRep = currentMessage->CastTo<CNetworkMessage_SpawnEnemyRepesention>();
				CEnemyFactory::GetInstance()->CreateRepesention(enemyRep->GetHealth(),enemyRep->GetEnemyType())->GetParent()->NotifyComponents(eComponentMessageType::eDeactivate,SComponentMessageData());
			}
			break;
			case ePackageType::eSetRepesentationActive:
			{
				CNetworkMessage_SetIsRepesentationActive* setmessage = currentMessage->CastTo<CNetworkMessage_SetIsRepesentationActive>();
				if (setmessage->GetActive() == Ja)
				{
					CEnemyClientRepresentationManager::GetInstance().GetRepresentation(setmessage->GetNetworkID()).GetParent()->NotifyComponents(eComponentMessageType::eActivate, SComponentMessageData());
					break;
				}
				CEnemyClientRepresentationManager::GetInstance().GetRepresentation(setmessage->GetNetworkID()).GetParent()->NotifyComponents(eComponentMessageType::eDeactivate, SComponentMessageData());
			}
			break;
			case ePackageType::eZero:
			case ePackageType::eSize:
			default: break;
			}
			currentMessage = myNetworkWrapper.Recieve(nullptr, nullptr);
		}

		UpdatePing(myTimerManager.GetTimer(myMainTimer).GetDeltaTime());
		


		if (currentTime > 1.f)
		{
			Ping();
			currentTime = 0.f;

			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CNetworkDebugInfo(myNetworkWrapper.GetAndClearDataSent(), myRoundTripTime));
		}

		std::map<unsigned int, CNetworkPlayerReciverComponent*>::iterator it;
		for (it = myNetworkRecieverComponents.begin(); it != myNetworkRecieverComponents.end(); it++)
		{
			if (it->second != nullptr)
			{
				it->second->Update(deltaTime);
			}
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

		std::this_thread::yield();
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

eMessageReturn CClient::DoEvent(const CSendNetworkMessageMessage& aSendNetworkMessageMessage)
{
	Send(aSendNetworkMessageMessage.UnpackHolder());
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
	myNetworkRecieverComponents[aMassage.GetComponent()->GetPlayerID()] = aMassage.GetComponent();
	return eMessageReturn::eStop;
}

eMessageReturn CClient::DoEvent(const CChangeLevel& aChangeLevelMessage)
{
	myNetworkRecieverComponents.clear();
	return eMessageReturn::eContinue;
}
