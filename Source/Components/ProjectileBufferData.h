#pragma once
#include <string>
#include "../CommonUtilities/GrowingArray.h"
class CProjectileComponent;
struct SProjectileBufferData 
{
	CU::GrowingArray<CProjectileComponent*> projectileBuffer;
	std::string projectileName;
};