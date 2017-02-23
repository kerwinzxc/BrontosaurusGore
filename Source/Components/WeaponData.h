#pragma once
struct ProjectileData;
struct WeaponData 
{
	ProjectileData* projectileData;
	std::string name;
	float fireRate;
};