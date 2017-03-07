#pragma once
struct SProjectileData;
struct SWeaponData 
{
	SProjectileData* projectileData;
	std::string name;
	std::string modelFilePath;
	float fireRate;
	unsigned short randomSpreadAngle;
	unsigned short projectilesFiredPerShot;
	bool shouldRayCast;
};