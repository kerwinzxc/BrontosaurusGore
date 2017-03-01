#pragma once

enum class ePackageType
{
	eZero = 0,
	eConnect,
	ePing,
	ePingResponse,
	eConnectResponse,
	eChat,
	ePosition,
	eImportantResponse,
	eLoadLevel,
	eClientReady,
	eServerReady,
	eSize,
};