#pragma once
#include "vector3.h"

namespace CU
{
	struct AABB
	{
		CU::Vector3f myCenterPos;
		CU::Vector3f myMinPos;
		CU::Vector3f myMaxPos;

		//TODO: WTH
		//Edvub testar fosho
		float myRadius;

	};
}