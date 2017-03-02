#pragma once

enum class ePackageType: __int8
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