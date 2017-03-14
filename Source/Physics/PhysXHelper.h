#pragma once
#include "../CommonUtilities/matrix44.h"
namespace physx
{
	class PxQuat;
}

namespace Physics
{
	physx::PxQuat MatrixToQuat(const CU::Matrix44f& aTransformation);
	CU::Matrix44f QuatToMatrix(const physx::PxQuat& aQuat);
}