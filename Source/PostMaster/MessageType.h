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
	eLength,
};

static const int MessageTypeLength = static_cast<int>(eMessageType::eLength);
