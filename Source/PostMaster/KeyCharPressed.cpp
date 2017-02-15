#include "stdafx.h"
#include "KeyCharPressed.h"
#include "BrontosaurusEngine\Console.h"

KeyCharPressed::KeyCharPressed(const char aKey)
	: myKey(aKey)
{
}


KeyCharPressed::~KeyCharPressed()
{
}

eMessageReturn KeyCharPressed::DoEvent(CConsole *aConsole) const
{
	return aConsole->TakeKeyBoardInputPressedChar(myKey);
}
