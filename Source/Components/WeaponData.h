#pragma once
struct ProjectileData;
struct WeaponData 
{
	ProjectileData* projectileData;
	const char* name;
	float fireRate;
};