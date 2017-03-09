#include "stdafx.h"
#include "PhysXHelper.h"
#include <foundation/PxQuat.h>
#include "foundation/PxMat33.h"

namespace Physics
{
	physx::PxQuat MatrixToQuat(const CU::Matrix44f& aTransformation)
	{
		physx::PxMat33 orientation;

		CU::Vector3f c1 = aTransformation.myRightVector;
		CU::Vector3f c2 = aTransformation.myUpVector;
		CU::Vector3f c3 = aTransformation.myForwardVector;

		orientation.column0 = { c1.x, c1.y, c1.z };
		orientation.column1 = { c2.x, c2.y, c2.z };
		orientation.column2 = { c3.x, c3.y, c3.z };
	
		return physx::PxQuat(orientation);
	}

	CU::Matrix44f QuatToMatrix(const physx::PxQuat& aQuat)
	{
		CU::Matrix44f returnOrientation;
		physx::PxMat33 orientation(aQuat);

		returnOrientation.m1 = { orientation.column0.x, orientation.column0.y, orientation.column0.z };
		returnOrientation.m2 = { orientation.column1.x, orientation.column1.y, orientation.column1.z };
		returnOrientation.m3 = { orientation.column2.x, orientation.column2.y, orientation.column2.z };

		return returnOrientation;
	}
}