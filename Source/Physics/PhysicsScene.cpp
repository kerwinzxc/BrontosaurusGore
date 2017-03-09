#include "stdafx.h"
#include "PhysicsScene.h"
#include <PxScene.h>
#include <PxRigidActor.h>

namespace
{
	physx::PxQuat MatrixToQuat(const CU::Matrix44f& aTransformation)
	{
		// Output quaternion
		float w, x, y, z;
		// Determine which of w,x,y, or z has the largest absolute value
		float fourWSquaredMinus1 = aTransformation.m11 + aTransformation.m22 + aTransformation.m33;
		float fourXSquaredMinus1 = aTransformation.m11 - aTransformation.m22 - aTransformation.m33;
		float fourYSquaredMinus1 = aTransformation.m22 - aTransformation.m11 - aTransformation.m33;
		float fourZSquaredMinus1 = aTransformation.m33 - aTransformation.m11 - aTransformation.m22;

		int biggestIndex = 0;
		float fourBiggestSquaredMinus1 = fourWSquaredMinus1;

		if (fourXSquaredMinus1 > fourBiggestSquaredMinus1) {
			fourBiggestSquaredMinus1 = fourXSquaredMinus1;
			biggestIndex = 1;
		}
		if (fourYSquaredMinus1 > fourBiggestSquaredMinus1) {
			fourBiggestSquaredMinus1 = fourYSquaredMinus1;
			biggestIndex = 2;
		}
		if (fourZSquaredMinus1 > fourBiggestSquaredMinus1) {
			fourBiggestSquaredMinus1 = fourZSquaredMinus1;
			biggestIndex = 3;
		}
		// Per form square root and division
		float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
		float mult = 0.25f / biggestVal;

		// Apply table to compute quaternion values
		switch (biggestIndex)
		{
		case 0:
			w = biggestVal;
			x = (aTransformation.m23 - aTransformation.m32) * mult;
			y = (aTransformation.m31 - aTransformation.m13) * mult;
			z = (aTransformation.m12 - aTransformation.m21) * mult;
			break;
		case 1:
			x = biggestVal;
			w = (aTransformation.m23 - aTransformation.m32) * mult;
			y = (aTransformation.m12 + aTransformation.m21) * mult;
			z = (aTransformation.m31 + aTransformation.m13) * mult;
			break;
		case 2:
			y = biggestVal;
			w = (aTransformation.m31 - aTransformation.m13) * mult;
			x = (aTransformation.m12 + aTransformation.m21) * mult;
			z = (aTransformation.m23 + aTransformation.m32) * mult;
			break;
		case 3:
			z = biggestVal;
			w = (aTransformation.m12 - aTransformation.m21) * mult;
			x = (aTransformation.m31 + aTransformation.m13) * mult;
			y = (aTransformation.m23 + aTransformation.m32) * mult;
			break;
		}

		return physx::PxQuat(x, y, z, w);
	}
}

namespace Physics
{
	CPhysicsScene::CPhysicsScene(physx::PxScene* aPxScene)
	{
		myPxScene = aPxScene;
	}

	CPhysicsScene::~CPhysicsScene()
	{
	}


	void CPhysicsScene::AddActor(physx::PxRigidActor* aActor)
	{
		myPxScene->addActor(*aActor);
	}

	void CPhysicsScene::RemoveActor(physx::PxRigidActor* aActor)
	{
		myPxScene->removeActor(*aActor);
	}

	void CPhysicsScene::SetActorPose(physx::PxRigidActor* aActor, const CU::Matrix44f& aTransformation)
	{
		physx::PxTransform transformation;
		transformation.p = { aTransformation.m41, aTransformation.m42 ,aTransformation.m43 };
		transformation.q = MatrixToQuat(aTransformation);

		aActor->setGlobalPose(transformation);
	}
}