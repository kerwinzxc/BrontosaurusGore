#pragma once

enum class eMessageType
{
	eCharPressed,
	eWindowSizeChanged,
	eStateStackMessage,
	eFocusChanged,
	eDrawCallsThisFrame,
	eDestroyComponent,
	eConsoleCalledUpon,
	eDroppedFile,
	eChangeLevel,
	eQuitGame,
	eGameEventMessage,
	eNetworkMessage,
	ePrint,
	eDebugInfo,
	eAddToCheckPointRespawn,
	eSetNewCheckPoint,
	eResetToCheckPointMessage,
	eCreateExplosionMessage,
	eDeactivateExplosion,
	eLength,
};

static const int MessageTypeLength = static_cast<int>(eMessageType::eLength);
