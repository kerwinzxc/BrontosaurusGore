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
		c1.Normalize();
		CU::Vector3f c2 = aTransformation.myUpVector;
		c2.Normalize();
		CU::Vector3f c3 = aTransformation.myForwardVector;
		c3.Normalize();

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

	CU::Matrix44f Slerp(const CU::Matrix44f& aLeft, const CU::Matrix44f& aRight, const float aLerpValue)
	{
		CU::Matrix44f result;

		physx::PxQuat left = MatrixToQuat(aLeft);
		physx::PxQuat right = MatrixToQuat(aRight);


		CU::Vector4f leftV(left.x, left.y, left.z, left.w);
		CU::Vector4f rightV(right.x, right.y, right.z, right.w);

		CU::Vector4f lerpedQuats = leftV.Lerp(rightV, aLerpValue);

		float dot = left.dot(right);

		if (dot < 0)
		{
			dot = -dot;
			right = -right;
		}

		if (dot < 0.95f)
		{
			float angle = acosf(dot);
			physx::PxQuat slerpie = (left * sinf(angle * (1.f - aLerpValue)) + right * sinf(angle * aLerpValue));

			CU::Vector4f slerpieV(slerpie.x, slerpie.y, slerpie.z, slerpie.w);
			slerpieV /= sinf(angle);
			lerpedQuats = slerpieV;
			//slerpie = physx::PxQuat(slerpieV.x, slerpieV.y, slerpieV.z, slerpieV.w);

			//return (q1*sinf(angle*(1 - t)) + q3*sinf(angle*t)) / sinf(angle);
		}
		//else // if the angle is small, use linear interpolation								
		//{
		//	return lerp(q1, q3, t);
		//}
		
		
		
		
		
		result = QuatToMatrix(physx::PxQuat(lerpedQuats.x, lerpedQuats.y, lerpedQuats.z, lerpedQuats.w));

		return result;
	}
}