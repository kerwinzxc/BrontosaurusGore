#include "stdafx.h"
#include "LevelSelectState.h"
#include "GUI/GUIManager.h"
#include "PostMaster/PostMaster.h"
#include "PostMaster/Message.h"
#include "PostMaster/Event.h"

LevelSelectState::LevelSelectState(StateStack& aStateStack)
	: State(aStateStack, eInputMessengerType::eLevelSelect)
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

void LevelSelectState::OnEnter(const bool /*aLetThroughRender*/)
{
	myGUIManager->RestartRenderAndUpdate(false, false);
}

void LevelSelectState::OnExit(const bool /*aLetThroughRender*/)
{
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
