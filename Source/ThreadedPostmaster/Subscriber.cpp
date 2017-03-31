#include "Subscriber.h"
#include "Postmaster.h"
#include "../CommonUtilities/DL_Debug.h"


Postmaster::ISubscriber::ISubscriber()
{
}


Postmaster::ISubscriber::~ISubscriber()
{
	Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const ConsoleCalledUpon& aConsoleCalledUpon)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const DrawCallsCount& aConsoleCalledupon)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const DroppedFile& aDroppedFile)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const FocusChange& aFocusChange)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const KeyCharPressed& aCharPressed)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const PopCurrentState& aPopCurrent)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const PushState& aPushState)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CNetworkPosMessageEvent& aPositionEvent)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CSendNetworkMessageMessage& aSendNetowrkMessageMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CServerReadyMessage& aSendNetowrkMessageMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const Message::CPrintMessage& aPrintMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CConectMessage& aConectMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CConectedMessage& aConectedMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CLoadLevelMessage& aLoadLevelMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CSetClientIDMessage & aSetClientIDMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CPlayerPositionMessage& aPlayerPositionMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CSpawnOtherPlayerMessage& aSpawnOtherPlayerMessage)
{
	return  eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const COtherPlayerSpawned& aOtherPlayerSpawnedMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CChangeLevel& aChangeLevelMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CQuitGame& aQuitGameMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CNetworkDebugInfo& aDebugInfo)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CGameEventMessage& aGameEventMessageMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CAddToCheckPointResetList & aAddToCheckPointResetListMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CSetAsNewCheckPointMessage & aSetAsNewCheckPointMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CResetToCheckPointMessage & aResetToCheckPointMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CCreateExplosionMessage & aCreateExplosionMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CDeactivateExplosionMessage & aDeactivateExplosionMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn  Postmaster::ISubscriber::DoEvent(const CRevivePlayerMessage & aRevivePlayerMessage)
{
	return eMessageReturn::eContinue;
}

void Postmaster::ISubscriber::SetSubscribedThread(const std::thread::id & aId)
{
	if(mySubscribedId != std::thread::id() && mySubscribedId != aId)
	{
		DL_ASSERT("Postmaster subscriber is attempting to subscribe from multiple threads.");
	}

	mySubscribedId = aId;
}

const std::thread::id& Postmaster::ISubscriber::GetSubscribedId() const
{
	return mySubscribedId;
}
