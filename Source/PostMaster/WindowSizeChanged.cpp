#include "stdafx.h"
#include "WindowSizeChanged.h"

EvWindowSizeChanged::EvWindowSizeChanged(const unsigned int aNewWindowWidth, const unsigned int aNewWindowHeight)
	: myNewWindowWidth(aNewWindowWidth)
	, myNewWindowHeight(aNewWindowHeight)
{
}

EvWindowSizeChanged::~EvWindowSizeChanged()
{
}
