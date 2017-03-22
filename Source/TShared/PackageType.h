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
	ePlayerPosition,
	eImportantResponse,
	eLoadLevel,
	eClientReady,
	eServerReady,
	eSpawnOtherPlayer,
	eTryToShoot,
	eOKShoot,
	eWeaponShoot,
	eDisconected,
	ePickupHealth,
	ePickupAmmo,
	ePickupArmor,
	eEnemyPosition,
	eTakeDamage,
	eSize,
};