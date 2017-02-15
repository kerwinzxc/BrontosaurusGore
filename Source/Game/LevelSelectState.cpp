#include "stdafx.h"
#include "LevelSelectState.h"
#include "GUI/GUIManager.h"
#include "PostMaster/PostMaster.h"
#include "PostMaster/Message.h"
#include "PostMaster/Event.h"

LevelSelectState::LevelSelectState(StateStack& aStateStack)
	: State(aStateStack)
	, myGUIManager(nullptr)
{
}

LevelSelectState::~LevelSelectState()
{
	SAFE_DELETE(myGUIManager);
}

void LevelSelectState::Init()
{
	myGUIManager = new GUI::GUIManager();
	myGUIManager->Init("Models/mainMenu/levelSelect.fbx");
}

eStateStatus LevelSelectState::Update(const CU::Time& aDeltaTime)
{
	myGUIManager->Update(aDeltaTime);
	
	return myStatus;
}

void LevelSelectState::Render()
{
	myGUIManager->Render();
}

void LevelSelectState::OnEnter(const bool aLetThroughRender)
{
	PostMaster::GetInstance().Subscribe(this, eMessageType::eMouseMessage);
	myGUIManager->RestartRenderAndUpdate(false, false);
}

void LevelSelectState::OnExit(const bool /*aLetThroughRender*/)
{
	PostMaster::GetInstance().UnSubscribe(this, eMessageType::eMouseMessage);
	myGUIManager->PauseRenderAndUpdate();
}

bool LevelSelectState::GetLetThroughRender() const
{
	return true;
}

eMessageReturn LevelSelectState::Recieve(const Message& aMessage)
{
	return aMessage.myEvent.DoEvent(this);
}
