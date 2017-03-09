#include "stdafx.h"
#include "ChangeLevel.h"
#include "Game\LoadState.h"
#include "..\StateStack\StateStack.h"

#include "TShared\NetworkMessage_LoadLevel.h"
#include "TClient\ClientMessageManager.h"
#include "ThreadedPostmaster\Postmaster.h"
#include "ThreadedPostmaster\SendNetowrkMessageMessage.h"

CChangeLevel::CChangeLevel(const eMessageType aType, const int aNewLevelIndex)
	: IMessage(aType)
	, myNewLevelIndex(aNewLevelIndex)
{
}

CChangeLevel::~CChangeLevel()
{
}

eMessageReturn CChangeLevel::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

Postmaster::Message::IMessage* CChangeLevel::Copy()
{
	return new CChangeLevel(*this);
}

int CChangeLevel::GetLevelIndex() const
{
	return myNewLevelIndex;
}

State* CChangeLevel::CreateLoadState(StateStack& aStateStack) const
{
	CNetworkMessage_LoadLevel* netowrkMessageMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_LoadLevel>("__All_But_Me");
	netowrkMessageMessage->myLevelIndex = myNewLevelIndex;
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(netowrkMessageMessage));

	return new CLoadState(aStateStack, myNewLevelIndex);
}