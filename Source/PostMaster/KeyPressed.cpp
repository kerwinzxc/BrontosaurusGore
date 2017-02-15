#include "stdafx.h"
#include "KeyPressed.h"

#include "../CommonUtilities/EKeyboardKeys.h"
#include "../BrontosaurusEngine/DebugInfoDrawer.h"
#include "../Components/ModelComponent.h"
#include "Game\CreditsState.h"
#include "../Game/PlayState.h"
#include"Game\PauseMenu.h"
#include "..\BrontosaurusEngine\ModelInstance.h"
#include "../BrontosaurusEngine/Console.h"
#include "../Components/CollisionComponentManager.h"


//temp
#include "../BrontosaurusEngine/Renderer.h"
#include "PostMaster.h"
#include "PushState.h"
#include "Game/SplashScreen.h"
#include "PopCurrentState.h"

KeyPressed::KeyPressed(const CU::eKeys& aKey)
	: myKey(aKey)
{
}

KeyPressed::~KeyPressed()
{
}

eMessageReturn KeyPressed::DoEvent(CDebugInfoDrawer* aDebugInfoDrawer) const
{
	if (aDebugInfoDrawer != nullptr)
	{
		aDebugInfoDrawer->PressedKey(myKey);
	}

	return eMessageReturn::eContinue;
}

eMessageReturn KeyPressed::DoEvent(CreditsState *aCreditState) const
{
	if (aCreditState != nullptr)
	{
		switch (myKey)
		{
		case CU::eKeys::RETURN:
		case CU::eKeys::ESCAPE:
			aCreditState->GoToMainMenu();
			break;
		default:
			break;
		}
	}

	return eMessageReturn::eContinue;
}

eMessageReturn KeyPressed::DoEvent(CPlayState* aPlayState) const
{
	if (aPlayState != nullptr)
	{
		switch (myKey)
		{
		case CU::eKeys::ESCAPE:
			aPlayState->Pause();
			return eMessageReturn::eStop;
			break;
		}
	}

	return eMessageReturn::eContinue;
}

eMessageReturn KeyPressed::DoEvent(CSplashScreen* aSplashScreen) const
{
	if (aSplashScreen)
	{
		aSplashScreen->UserWantsToContinue();
	}

	return eMessageReturn::eContinue;
}

eMessageReturn KeyPressed::DoEvent(PauseMenu *aPauseMenu) const
{
	switch (myKey)
	{
	case CU::eKeys::P:
	case CU::eKeys::ESCAPE:
		aPauseMenu->Resume();
		break;
	default:
		break;
	}
	return eMessageReturn::eContinue;
}

eMessageReturn KeyPressed::DoEvent(CConsole* aConsole) const
{
	if (aConsole != nullptr)
	{
		return aConsole->TakeKeyBoardInput(myKey);
	}

	return eMessageReturn::eContinue;
}

eMessageReturn KeyPressed::DoEvent(CCollisionComponentManager* aCollisionComponentManager) const
{
	if (myKey == CU::eKeys::F3)
	{
		if (aCollisionComponentManager != nullptr)
		{
			aCollisionComponentManager->FlipShouldRender();
		}
	}

	return eMessageReturn::eContinue;
}

eMessageReturn KeyPressed::DoEvent(CRenderer* aRenderer) const
{
	if (aRenderer == nullptr)
	{
		return eMessageReturn::eContinue;
	}

#ifdef _RETAIL_BUILD
//#error "detta är temp-kod, man ska för fan inte kunna stänga av bloom med b in game, mvh carl"
#endif // _RETAIL_BUILD


	switch (myKey)
	{
	case CU::eKeys::B:
		//aRenderer->GetSettings().Bloom = !aRenderer->GetSettings().Bloom;
		break;
	default:
		break;
	}

	return eMessageReturn::eContinue;
}
