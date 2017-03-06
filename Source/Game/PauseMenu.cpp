#include "stdafx.h"
#include "PauseMenu.h"

//#include "Audio/AudioInterface.h"

#include "BrontosaurusEngine/Engine.h"
#include "BrontosaurusEngine/Renderer.h"

#include "BrontosaurusEngine/SpriteInstance.h"
#include "CommonUtilities/InputMessage.h"
#include <CommonUtilities/EKeyboardKeys.h>
#include "CommonUtilities/EInputReturn.h"

CPauseMenuState::CPauseMenuState(StateStack& aStateStack)
	: State(aStateStack, eInputMessengerType::ePauseMenu, 2)
	, myPauseScreenSprite(nullptr)
{
}

CPauseMenuState::~CPauseMenuState()
{
	SAFE_DELETE(myPauseScreenSprite);
}

void CPauseMenuState::Init()
{
	myPauseScreenSprite = new CSpriteInstance("Sprites/PauseMenu/pauseMenu.dds");
}

eStateStatus CPauseMenuState::Update(const CU::Time& aDeltaTime)
{
	return myStatus;
}

void CPauseMenuState::Render()
{
	if (myPauseScreenSprite)
	{
		SChangeStatesMessage* changeStateMessage = new SChangeStatesMessage();
		changeStateMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStateMessage->myRasterizerState = eRasterizerState::eNoCulling;
		changeStateMessage->myBlendState = eBlendState::eAddBlend;
		RENDERER.AddRenderMessage(changeStateMessage);

		myPauseScreenSprite->Render();
	}
}

void CPauseMenuState::OnEnter(const bool /*aLetThroughRender*/)
{
}

void CPauseMenuState::OnExit(const bool /*aLetThroughRender*/)
{
}

bool CPauseMenuState::GetLetThroughRender() const
{
	return true;
}

void CPauseMenuState::Resume()
{
	//Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new PopCurrentState());
}

void CPauseMenuState::LevelSelect()
{
}

void CPauseMenuState::MainMenu()
{
}

void CPauseMenuState::Quit()
{
}

CU::eInputReturn CPauseMenuState::RecieveInput(const CU::SInputMessage& aInputMessage)
{
	if (aInputMessage.myType == CU::eInputType::eKeyboardPressed)
	{
		if (aInputMessage.myKey == CU::eKeys::ESCAPE || aInputMessage.myKey == CU::eKeys::P)
		{
			myStatus = eStateStatus::ePop;
		}
	}

	return CU::eInputReturn::eKeepSecret;
}
