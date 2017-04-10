#include "stdafx.h"
#include "MenuState.h"
#include "InputMessage.h"
#include "EInputReturn.h"

CMenuState::CMenuState(StateStack& aStateStack): State(aStateStack, eInputMessengerType::eMainMenu), myShowStateBelow(false)
{
}

CMenuState::~CMenuState()
{
}

void CMenuState::Init()
{
	CSpriteInstance* spriteInstance = new CSpriteInstance("Sprites/Menu/TestButton.dds");
	spriteInstance->SetPosition(CU::Vector2f(0.5f, 0.5f));
	myManager.CreateSprite(spriteInstance, 1);
}

eStateStatus CMenuState::Update(const CU::Time& aDeltaTime)
{
	myManager.Update(aDeltaTime);
	return eStateStatus::eKeep;
}

void CMenuState::Render()
{
	myManager.Render();
}

void CMenuState::OnEnter(const bool aLetThroughRender)
{
}

void CMenuState::OnExit(const bool aLetThroughRender)
{
}

CU::eInputReturn CMenuState::RecieveInput(const CU::SInputMessage& aInputMessage)
{
	switch (aInputMessage.myType)
	{
	case CU::eInputType::eMouseMoved: 
		myManager.UpdateMousePosition(aInputMessage.myMousePosition);
		break;
	case CU::eInputType::eMousePressed: break;
	case CU::eInputType::eMouseReleased: break;
	case CU::eInputType::eScrollWheelChanged: break;
	case CU::eInputType::eKeyboardPressed: break;
	case CU::eInputType::eKeyboardReleased: break;
	default: break;
	}

	return CU::eInputReturn::eKeepSecret;
}
