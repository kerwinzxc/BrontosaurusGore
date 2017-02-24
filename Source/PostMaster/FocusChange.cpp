#include "stdafx.h"
#include "FocusChange.h"
#include "../BrontosaurusEngine/InputManager.h"

FocusChange::FocusChange(const bool aHasFocus) : IMessage(eMessageType::eFocusChanged)
{
	myHasFocus = aHasFocus;
}

FocusChange::~FocusChange()
{
}

eMessageReturn FocusChange::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

bool FocusChange::GetHasFocus() const
{
	return myHasFocus;
}

Postmaster::Message::IMessage* FocusChange::Copy()
{
	return new FocusChange(*this);
}
