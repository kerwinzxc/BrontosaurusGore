#pragma once
#include "../CommonUtilities/vector3.h"

struct SFluidParticle
{
	CU::Vector3f position;
	CU::Vector3f velocity;
	float viscosity = 0.5;
};
