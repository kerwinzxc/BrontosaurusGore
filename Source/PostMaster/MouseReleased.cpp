#include "stdafx.h"
#include "MouseReleased.h"

#include "../GUI/GUIManager.h"
#include "Game/LevelSelectState.h"
#include "CommonUtilities/EMouseButtons.h"

MouseReleased::MouseReleased(const CU::Vector2f& aMousePosition, const CU::eMouseButtons& aMouseButton)
	: myMousePosition(aMousePosition)
	, myMouseButton(aMouseButton)
{
}

MouseReleased::~MouseReleased()
{
}

eMessageReturn MouseReleased::DoEvent(GUI::GUIManager* aGUIManager) const
{
	if (aGUIManager != nullptr)
	{
		return aGUIManager->MouseReleased(myMouseButton, CU::Vector2f(myMousePosition.x / 1920.f, myMousePosition.y / 1080.f));
	}

	return eMessageReturn::eContinue;
}

eMessageReturn MouseReleased::DoEvent(LevelSelectState* aLevelSelectState) const
{
	if (aLevelSelectState != nullptr)
	{
		aLevelSelectState->SetStateStatus(eStateStatus::ePop);
	}

	return eMessageReturn::eContinue;
}
