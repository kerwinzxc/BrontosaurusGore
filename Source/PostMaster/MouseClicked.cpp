#include "stdafx.h"
#include "MouseClicked.h"

#include "../GUI/GUIManager.h"
#include "BrontosaurusEngine/Engine.h"
#include "CommonUtilities/EMouseButtons.h"
#include "Game/SplashScreen.h"

MouseClicked::MouseClicked(const CU::Vector2f& aMousePosition, const CU::eMouseButtons& aMouseButton)
	: myMousePosition(aMousePosition)
	, myMouseButton(aMouseButton)
{
}

MouseClicked::~MouseClicked()
{
}

eMessageReturn MouseClicked::DoEvent(GUI::GUIManager* aGUIManager) const
{
	if (aGUIManager != nullptr)
	{
		return aGUIManager->MouseClicked(myMouseButton, CU::Vector2f(myMousePosition.x / WINDOW_SIZE_F.x, myMousePosition.y / WINDOW_SIZE_F.y));
	}

	return eMessageReturn::eContinue;
}

eMessageReturn MouseClicked::DoEvent(CSplashScreen* aSplashScreen) const
{
	if (aSplashScreen)
	{
		aSplashScreen->UserWantsToContinue();
	}

	return eMessageReturn::eContinue;
}
