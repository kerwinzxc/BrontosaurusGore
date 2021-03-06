#pragma once
#include "../PostMaster/EMessageReturn.h"
#include "../PostMaster/NetworkPosMessageEvent.h"
#include "SpawnOtherPlayerMessage.h"
#include <thread>

class PushState;
class PopCurrentState;
class KeyCharPressed;
class FocusChange;
class DroppedFile;
class ConsoleCalledUpon;
class DrawCallsCount;
class CServerReadyMessage;
class CSendNetworkMessageMessage;
class CConectMessage;
class CConectedMessage;
class CLoadLevelMessage;
class CSetClientIDMessage;
class CPlayerPositionMessage;
class CSpawnOtherPlayerMessage;
class COtherPlayerSpawned;
class CChangeLevel;
class CQuitGame;
class CNetworkDebugInfo;
class CGameEventMessage;
class CAddToCheckPointResetList;
class CSetAsNewCheckPointMessage;
class CResetToCheckPointMessage;
class CCreateExplosionMessage;
class CDeactivateExplosionMessage;
class CRevivePlayerMessage;
class CStartWaveMessage;
class CAddEnemyToWave;
class CPlayerEnteredArena;

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
		virtual eMessageReturn DoEvent(const CSendNetworkMessageMessage& aSendNetowrkMessageMessage);
		virtual eMessageReturn DoEvent(const CServerReadyMessage& aSendNetowrkMessageMessage);
		virtual eMessageReturn DoEvent(const Message::CPrintMessage& aPrintMessage);
		virtual eMessageReturn DoEvent(const CConectMessage& aConectMessage);
		virtual eMessageReturn DoEvent(const CConectedMessage& aConectedMessage);
		virtual eMessageReturn DoEvent(const CLoadLevelMessage& aLoadLevelMessage);
		virtual eMessageReturn DoEvent(const CSetClientIDMessage& aSetClientIDMessage);
		virtual eMessageReturn DoEvent(const CPlayerPositionMessage& aPlayerPositionMessage);
		virtual eMessageReturn DoEvent(const CSpawnOtherPlayerMessage& aSpawnOtherPlayerMessage);
		virtual eMessageReturn DoEvent(const COtherPlayerSpawned& aOtherPlayerSpawnedMessage);
		virtual eMessageReturn DoEvent(const CChangeLevel& aChangeLevelMessage);
		virtual eMessageReturn DoEvent(const CQuitGame& aQuitGameMessage);
		virtual eMessageReturn DoEvent(const CNetworkDebugInfo& aDebugInfo);
		virtual eMessageReturn DoEvent(const CGameEventMessage & aGameEventMessageMessage);
		virtual eMessageReturn DoEvent(const CAddToCheckPointResetList & aAddToCheckPointResetListMessage);
		virtual eMessageReturn DoEvent(const CSetAsNewCheckPointMessage & aSetAsNewCheckPointMessage);
		virtual eMessageReturn DoEvent(const CResetToCheckPointMessage & aResetToCheckPointMessage);
		virtual eMessageReturn DoEvent(const CCreateExplosionMessage & aCreateExplosionMessage);
		virtual eMessageReturn DoEvent(const CDeactivateExplosionMessage & aDeactivateExplosionMessage);
		virtual eMessageReturn DoEvent(const CRevivePlayerMessage & aRevivePlayerMessage);
		virtual eMessageReturn DoEvent(const CStartWaveMessage & aStartWaveMessage);
		virtual eMessageReturn DoEvent(const CAddEnemyToWave & aAddEnemyToWave);
		virtual eMessageReturn DoEvent(const CPlayerEnteredArena & aPLayerEnteredArena);

		void SetSubscribedThread(const std::thread::id& aId);
		const std::thread::id& GetSubscribedId() const;
	private:
		std::thread::id mySubscribedId;
	};
}
