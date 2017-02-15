#pragma once

enum class eMessageType
{
	eWindowSizeChanged,
	eMouseMessage,
	eKeyboardMessage,
	eInputMessagePressed,
	eStateStackMessage,
	eFocusChanged,
	eDrawCallsThisFrame,
	ePlayerDied,
	eLoadPlayer,
	ePlayerPickedPowerUp,
	ePlayerCreated,
	ePlayerMissed,
	ePlayerHitSomething,
	eEnemyDied,
	eScore,
	eDestroyComponent,
	eObjectives,
	eChangeLevel,
	eShrineOrWellClicked,
	eConsoleCalledUpon,
	QuestRelated,
	eHatAdded,
	eDroppedFile,
	eBuyButtonPressed,
	ePlayerHealthChanged,
	ePlayerManaChanged,
	eShopItemSelected,
	eShopClosed,
	eGameObjectDied,
	eGoldChanged,
	eSecretMouseMessageMvhCarl,
	ePlayerGotHat,
	eGameEventMessage,
	eLength,
};
//Ifall du vill l�gga till en till messagetyp s� kompilera om Postmaster Projekter annars kraschar det. 
//Och lenght ska vara l�ngst ner.


const int MessageTypeLength = static_cast<int>(eMessageType::eLength);
