#pragma once

enum class eMessageType
{
	eCharPressed,
	eWindowSizeChanged,
	eStateStackMessage,
	eFocusChanged,
	eDrawCallsThisFrame,
	ePlayerDied,
	eLoadPlayer,
	eEnemyDied,
	eDestroyComponent,
	eObjectives,
	eChangeLevel,
	eConsoleCalledUpon,
	QuestRelated,
	eDroppedFile,
	eBuyButtonPressed,
	ePlayerHealthChanged,
	ePlayerManaChanged,
	eGameObjectDied,
	eGoldChanged,
	eGameEventMessage,
	eLength,
};

static const int MessageTypeLength = static_cast<int>(eMessageType::eLength);
