#pragma once
#include "HealthPoint.h"
struct SExplosionData;
struct SProjectileData
{
	std::string projectileModelFilePath;
	SExplosionData* explosionData;
	float movementSpeed;
	float maximumTravelRange;
	healthPoint damage;
	bool shouldExplodeOnImpact;
};