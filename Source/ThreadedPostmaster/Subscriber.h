#pragma once
#include "../PostMaster/EMessageReturn.h"
#include "../PostMaster/NetworkPosMessageEvent.h"
#include "SpawnOtherPlayerMessage.h"

class PushState;
class PopCurrentState;
class KeyCharPressed;
class FocusChange;
class DroppedFile;
class ConsoleCalledUpon;
class DrawCallsCount;
class CServerReadyMessage;
class CSendNetowrkMessageMessage;
class CConectMessage;
class CConectedMessage;
class CLoadLevelMessage;
class CSetClientIDMessage;
class CPlayerPositionMessage;
class CSpawnOtherPlayerMessage;

namespace Postmaster
{
	namespace Message
	{
		class CPrintMessage;
	}

	class ISubscriber
	{
	public:
		ISubscriber();
		virtual ~ISubscriber();
		virtual eMessageReturn DoEvent(const ConsoleCalledUpon& aConsoleCalledUpon);
		virtual eMessageReturn DoEvent(const DrawCallsCount& aConsoleCalledupon);
		virtual eMessageReturn DoEvent(const DroppedFile& aDroppedFile);
		virtual eMessageReturn DoEvent(const FocusChange& aFocusChange);
		virtual eMessageReturn DoEvent(const KeyCharPressed& aCharPressed);
		virtual eMessageReturn DoEvent(const PopCurrentState& aPopCurrent);
		virtual eMessageReturn DoEvent(const PushState& aPushState);
		virtual eMessageReturn DoEvent(const CNetworkPosMessageEvent& aPositionEvent);
		virtual eMessageReturn DoEvent(const CSendNetowrkMessageMessage& aSendNetowrkMessageMessage);
		virtual eMessageReturn DoEvent(const CServerReadyMessage& aSendNetowrkMessageMessage);
		virtual eMessageReturn DoEvent(const Message::CPrintMessage& aPrintMessage);
		virtual eMessageReturn DoEvent(const CConectMessage& aConectMessage);
		virtual eMessageReturn DoEvent(const CConectedMessage& aConectedMessage);
		virtual eMessageReturn DoEvent(const CLoadLevelMessage& aLoadLevelMessage);
		virtual eMessageReturn DoEvent(const CSetClientIDMessage& aSetClientIDMessage);
		virtual eMessageReturn DoEvent(const CPlayerPositionMessage& aPlayerPositionMessage);
		virtual eMessageReturn DoEvent(const CSpawnOtherPlayerMessage& aSpawnOtherPlayerMessage);
	};
}
