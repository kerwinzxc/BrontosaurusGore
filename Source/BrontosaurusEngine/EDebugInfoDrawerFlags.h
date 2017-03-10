#pragma once

enum eDebugFlags
{
	eDebugFlags_FPS = 1 << 0,
	eDebugFlags_LogicFPS = 1 << 1,
	eDebugFlags_DrawCalls = 1 << 2,
	eDebugFlags_MemoryUsage = 1 << 3,
	eDebugFlags_RoundTripTime = 1 << 4,
	eDebugFlags_DataAmmountSent = 1 << 5
};
