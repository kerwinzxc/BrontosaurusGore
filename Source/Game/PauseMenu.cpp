#include "stdafx.h"
#include "PauseMenu.h"

#include "../PostMaster/PostMaster.h"
#include "../PostMaster/Message.h"
#include "../PostMaster/Event.h"
#include "../PostMaster/PopCurrentState.h"

#include "GUI/GUIManager.h"
#include "BrontosaurusEngine/Skybox.h"
#include "BrontosaurusEngine/Renderer.h"
#include "BrontosaurusEngine/Engine.h"
#include "Audio/AudioInterface.h"

#include "Game.h"

PauseMenu::PauseMenu(StateStack& aStateStack)
	: State(aStateStack, eInputMessengerType::ePauseMenu)
	, myGUIManager(nullptr)
{
}


PauseMenu::~PauseMenu()
{
	SAFE_DELETE(myGUIManager);
}

void PauseMenu::Init()
{
	myGUIManager = new GUI::GUIManager();
	myGUIManager->Init("Models/mainMenu/pauseMenu.fbx"); 
}

eStateStatus PauseMenu::Update(const CU::Time & aDeltaTime)
{
	myGUIManager->Update(aDeltaTime);

	return myStatus;
}

void PauseMenu::Render()
{
	myGUIManager->Render();
}

void PauseMenu::OnEnter(const bool /*aLetThroughRender*/)
{
	myGUIManager->RestartRenderAndUpdate();
	//PostMaster::GetInstance().Subscribe(this, eMessageType::eStateMessage);
	//PostMaster::GetInstance().Subscribe(this, eMessageType::eKeyboardMessage);
}

void PauseMenu::OnExit(const bool /*aLetThroughRender*/)
{
	myGUIManager->PauseRenderAndUpdate();
	//PostMaster::GetInstance().UnSubscribe(this, eMessageType::eStateMessage);
	//PostMaster::GetInstance().UnSubscribe(this, eMessageType::eKeyboardMessage);
}


eMessageReturn PauseMenu::Recieve(const Message & aMessage)
{
	return aMessage.myEvent.DoEvent(this);
}

bool PauseMenu::GetLetThroughRender() const
{
	return true;
}

void PauseMenu::Resume()
{
	PostMaster::GetInstance().SendLetter(Message(eMessageType::eStateStackMessage, PopCurrentState()));
}

void PauseMenu::LevelSelect()
{
}

void PauseMenu::MainMenu()
{
}

void PauseMenu::Quit()
{
}
