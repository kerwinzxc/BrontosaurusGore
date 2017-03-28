#pragma once
#include "WeaponSoundData.h"
struct SProjectileData;
struct SWeaponData 
{
	SProjectileData* projectileData;
	SWeaponSoundData soundData;
	std::string name;
	std::string modelFilePath;
	float fireRate;
	float modelPositionX;
	float modelPositionY;
	float modelPositionZ;
	float shootPositionX;
	float shootPositionY;
	float shootPositionZ;
	unsigned short randomSpreadAngle;
	unsigned short projectilesFiredPerShot;
};