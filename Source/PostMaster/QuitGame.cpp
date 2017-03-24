#include "stdafx.h"
#include "QuitGame.h"
#include "ThreadedPostmaster\Subscriber.h"


CQuitGame::CQuitGame()
	: IMessage(eMessageType::eQuitGame)
{
}

CQuitGame::~CQuitGame()
{
}

Postmaster::Message::IMessage* CQuitGame::Copy()
{
	return new CQuitGame();
}

eMessageReturn CQuitGame::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	aSubscriber.DoEvent(*this);
	return eMessageReturn::eContinue;
}
