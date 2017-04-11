#pragma once
#include "HealthPoint.h"
struct SExplosionData
{
	float radius;
	float knockBackForce;
	healthPoint damage;
	bool isPlayerFriendly;
	CU::GrowingArray<std::string> explosionParticles;
};