#include "stdafx.h"
#include "MainMenuState.h"

#include "../PostMaster/PostMaster.h"
#include "../PostMaster/Message.h"
#include "../PostMaster/Event.h"

#include "GUI/GUIManager.h"
#include "BrontosaurusEngine/Skybox.h"
#include "BrontosaurusEngine/Renderer.h"
#include "BrontosaurusEngine/Engine.h"
#include "Audio/AudioInterface.h"
#include "BackgroundLoadingManager.h"

MainMenuState::MainMenuState(StateStack& aStateStack)
	: State(aStateStack)
	, myGUIManager(nullptr)
	, mySkyBox(nullptr)
	, myIsGoingToLevelSelect(false)
{
}

MainMenuState::~MainMenuState()
{
	SAFE_DELETE(myGUIManager);
	
	if (mySkyBox && mySkyBox->DecRef() <= 0)
	{
		SAFE_DELETE(mySkyBox);
	}
}

void MainMenuState::Init()
{
	myGUIManager = new GUI::GUIManager();
	myGUIManager->Init("Models/mainMenu/mainMenu.fbx");
	
	mySkyBox = new CSkybox();
	mySkyBox->Init("default_cubemap.dds");
}

eStateStatus MainMenuState::Update(const CU::Time& aDeltaTime)
{
	myGUIManager->Update(aDeltaTime);

	return myStatus;
}

void MainMenuState::Render()
{
	SChangeStatesMessage statemsg;
	statemsg.myBlendState = eBlendState::eNoBlend;
	statemsg.myRasterizerState = eRasterizerState::eNoCullingClockwise;
	statemsg.myDepthStencilState = eDepthStencilState::eDisableDepth;
	statemsg.mySamplerState = eSamplerState::eClamp;
	
	RENDERER.AddRenderMessage(new SChangeStatesMessage(statemsg));
	
	SRenderSkyboxMessage* msg = new SRenderSkyboxMessage();
	mySkyBox->AddRef();
	msg->mySkybox = mySkyBox;
	
	RENDERER.AddRenderMessage(msg);
	
	myGUIManager->Render();
}

void MainMenuState::OnEnter(const bool aLetThroughRender)
{
	//bool result;
	Audio::CAudioInterface::GetInstance()->LoadBank("Audio/mainMenu.bnk");
	Audio::CAudioInterface::GetInstance()->PostEvent("PlayBasSong");
	
	if (myIsGoingToLevelSelect == false)
	{
		myGUIManager->RestartRenderAndUpdate(false, false);
	}
	else
	{
		PostMaster::GetInstance().Subscribe(myGUIManager, eMessageType::eMouseMessage, 5);
		PostMaster::GetInstance().Subscribe(myGUIManager, eMessageType::eKeyboardMessage);
	}
	
	myIsGoingToLevelSelect = false;
	
	myGUIManager->SetRenderMouse(true);
}

void MainMenuState::OnExit(const bool /*aLetThroughRender*/)
{
	Audio::CAudioInterface::GetInstance()->PostEvent("StopBasSong");
	Audio::CAudioInterface::GetInstance()->PostEvent("switchBank");
	Audio::CAudioInterface::GetInstance()->UnLoadBank("Audio/menMenu.bnk");
	

	if (myIsGoingToLevelSelect == false)
	{
		myGUIManager->PauseRenderAndUpdate();
	}
	else
	{
		PostMaster::GetInstance().UnSubscribe(myGUIManager, eMessageType::eMouseMessage);
		PostMaster::GetInstance().UnSubscribe(myGUIManager, eMessageType::eKeyboardMessage);
	}
	
	myGUIManager->SetRenderMouse(false);
}

bool MainMenuState::GetLetThroughRender() const
{
	return true;
}
