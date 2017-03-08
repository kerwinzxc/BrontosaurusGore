#pragma once
#include "HealthPoint.h"
struct SProjectileData
{
	std::string projectileModelFilePath;
	float movementSpeed;
	float maximumTravelRange;
	healthPoint damage;
};