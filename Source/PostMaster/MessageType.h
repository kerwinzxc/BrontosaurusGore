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
//Ifall du vill l�gga till en till messagetyp s� kompilera om Postmaster Projekter annars kraschar det. 
//Och lenght ska vara l�ngst ner.


const int MessageTypeLength = static_cast<int>(eMessageType::eLength);
