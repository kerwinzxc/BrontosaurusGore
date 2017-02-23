#pragma once
struct SProjectileData;
struct SWeaponData 
{
	SProjectileData* projectileData;
	std::string name;
	float fireRate;
};