#include "stdafx.h"
#include "LoadState.h"
#include "PlayState.h"
#include "Engine.h"
#include "ThreadPool.h"
#include "SpriteInstance.h"
#include "StateStack/StateStack.h"

#include "Renderer.h"
#include "BackgroundLoadingManager.h"
#include "Game/Game.h"
#include "TShared/NetworkMessage_ClientReady.h"
#include "TClient/ClientMessageManager.h"
#include "ThreadedPostmaster/Postmaster.h"
#include "ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "TClient/ServerReadyMessage.h"
#include "../Audio/AudioInterface.h"
#include "PollingStation.h"

CLoadState::CLoadState(StateStack& aStateStack, const int aLevelIndex)
	: State(aStateStack, eInputMessengerType::eLoadState)
	, myLevelIndex(aLevelIndex)
	, myGotOkFromServer(false)
	, myNumberOfPlayersToSpawnBeforeStarting(1)
{
	myPlayState = nullptr;
}

CLoadState::~CLoadState()
{
}

void CLoadState::Init()
{
	//RENDERER.ClearRenderQueue();
	CBackgroundLoadingManager &bLM = CBackgroundLoadingManager::GetInstance();

	myLoadSprite = new CSpriteInstance("Sprites/Menu/Main/Loading/default.dds");
	myLoadSprite->SetPivot({ 0.5f, 0.5f });
	myLoadSprite->SetPosition({ 0.5f, 0.5f });

	if (bLM.GetIfActive() == true)
	{
		if (bLM.GetLevelIndex() == myLevelIndex)
		{
			myPlayState = bLM.GetPlaystate();
		}
		else
		{
			myPlayState = new CPlayState(myStateStack, myLevelIndex);
			bLM.LoadAnotherState(myPlayState);
		}
	}
	else
	{
		bLM.CreateStateToLoad(myStateStack, myLevelIndex);
		myPlayState = bLM.GetPlaystate();
	}

	Audio::CAudioInterface::GetInstance()->PostEvent("Stop_Music");
	std::string BGMusic;
	BGMusic = "Music_Level" + std::to_string(myLevelIndex + 1);
	Audio::CAudioInterface::GetInstance()->PostEvent(BGMusic.c_str());

	CPollingStation::GetInstance()->SetCurrentLevelIndex(myLevelIndex);
}

eStateStatus CLoadState::Update(const CU::Time& aDeltaTime)
{
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
	//myLoadingAnimation.Update(aDeltaTime);

	if (myPlayState->IsLoaded() == true)
	{
		CClientMessageManager* const messageManagerInstance = CClientMessageManager::GetInstance();

		if (messageManagerInstance == nullptr)
		{
			DL_ASSERT("message manager is not created");
		}

		CNetworkMessage_ClientReady* readyMessage = messageManagerInstance->CreateMessage<CNetworkMessage_ClientReady>("__Server");

		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(readyMessage));
	}

	if (myGotOkFromServer == true && myNumberOfPlayersToSpawnBeforeStarting == 0)
	{
		myStateStack.SwapState(myPlayState);
		CBackgroundLoadingManager::GetInstance().Clear();
	}

	return eStateStatus::eKeep;
}

void CLoadState::Render()
{
	SChangeStatesMessage msg;

	msg.myBlendState = eBlendState::eAlphaBlend;
	msg.myDepthStencilState = eDepthStencilState::eDisableDepth;
	msg.myRasterizerState = eRasterizerState::eDefault;
	msg.mySamplerState = eSamplerState::eClamp;


	RENDERER.AddRenderMessage(new SChangeStatesMessage(msg));
	//myLoadingAnimation.Render();
	myLoadSprite->Render();
}

void CLoadState::OnEnter(const bool /*aLetThroughRender*/)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);
}

void CLoadState::OnExit(const bool /*aLetThroughRender*/)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

eMessageReturn CLoadState::DoEvent(const CServerReadyMessage& aServerReadyMessageMessageMessage)
{
	myGotOkFromServer = true;
	myNumberOfPlayersToSpawnBeforeStarting = aServerReadyMessageMessageMessage.myNumberOfPlayers;
	return eMessageReturn::eContinue;
}

eMessageReturn CLoadState::DoEvent(const CSpawnOtherPlayerMessage& aSpawnOtherPlayerMessage)
{
	myPlayState->SpawnOtherPlayer(aSpawnOtherPlayerMessage.myPlayerID);
	myNumberOfPlayersToSpawnBeforeStarting--;
	return eMessageReturn::eContinue;
}
