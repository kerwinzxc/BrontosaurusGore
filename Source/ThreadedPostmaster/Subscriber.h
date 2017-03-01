#pragma once
#include "../PostMaster/EMessageReturn.h"
#include "../PostMaster/NetworkPosMessageEvent.h"

class PushState;
class PopCurrentState;
class KeyCharPressed;
class FocusChange;
class DroppedFile;
class ConsoleCalledUpon;
class DrawCallsCount;
class CServerReadyMessage;
class CSendNetowrkMessageMessage;

namespace Postmaster
{
	

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
	};
}
