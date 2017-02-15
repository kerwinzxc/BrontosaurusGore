#include "stdafx.h"
#include "FocusChange.h"
#include "../BrontosaurusEngine/InputManager.h"

FocusChange::FocusChange(const bool aHasFocus)
{
	myHasFocus = aHasFocus;
}

FocusChange::~FocusChange()
{
}

eMessageReturn FocusChange::DoEvent(CInputManager* aInputManager) const
{
	if (aInputManager)
	{
		aInputManager->LockUnlockMouse(myHasFocus);
	}

	return eMessageReturn::eContinue;
}
