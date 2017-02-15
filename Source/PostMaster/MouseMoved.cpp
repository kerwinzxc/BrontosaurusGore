#include "stdafx.h"
#include "MouseMoved.h"
#include "../GUI/GUIManager.h"
#include "../GUI/GUICursor.h"
#include "../BrontosaurusEngine/Engine.h"


MouseMoved::MouseMoved(const CU::Vector2f& aMousePosition)
	: myMousePosition(aMousePosition)
{
}

MouseMoved::~MouseMoved()
{
}


eMessageReturn MouseMoved::DoEvent(GUI::GUIManager* aGUIManager) const
{
	if (aGUIManager != nullptr)
	{
		CU::Vector2f windowSize(CEngine::GetInstance()->GetWindowSize());

		return aGUIManager->MouseMoved(CU::Vector2f(myMousePosition.x / windowSize.x, myMousePosition.y / windowSize.y));
	}

	return eMessageReturn::eContinue;
}

eMessageReturn MouseMoved::DoEvent(GUICursor* aGUICursor) const
{
	if (aGUICursor != nullptr)
	{
		CU::Vector2f windowSize(CEngine::GetInstance()->GetWindowSize());
				
		aGUICursor->SetPosition(CU::Vector2f(myMousePosition / windowSize));
	}

	return eMessageReturn::eContinue;
}
