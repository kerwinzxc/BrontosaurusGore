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
	eGameEventMessage,
	eNetworkMessage,
	ePrint,
	eDebugInfo,
	eAddToCheckPointRespawn,
	eLength,
};

static const int MessageTypeLength = static_cast<int>(eMessageType::eLength);
