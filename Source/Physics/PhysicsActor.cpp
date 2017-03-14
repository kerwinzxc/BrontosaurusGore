#include "stdafx.h"
#include "PhysicsActor.h"

#include <PxShape.h>
#include <PxRigidActor.h>
#include "PxRigidBody.h"
#include "PxRigidDynamic.h"
#include "PhysXHelper.h"
#include "Shape.h"

namespace Physics
{
	CPhysicsActor::CPhysicsActor(physx::PxRigidActor* aActor, CShape* aShape)
	{
		myPxActor = aActor;
		myShape = aShape;
		myPxActor->userData = nullptr;
	}

	CPhysicsActor::~CPhysicsActor()
	{
		if(myPxActor)
			myPxActor->release();
		myPxActor = nullptr;

		if (myShape)
			delete myShape;
		myShape = nullptr;
	}

	void CPhysicsActor::SetIsTrigger(const bool aIsTrigger)
	{
		if (myShape != nullptr)
		{
			myIsTrigger = aIsTrigger;
			physx::PxShape* shape = nullptr;
			myPxActor->getShapes(&shape, 1);
			myPxActor->detachShape(*shape);
			shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !aIsTrigger);
			shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, aIsTrigger);
			myPxActor->attachShape(*shape);
		}
	}


	void CPhysicsActor::SetTransformation(const CU::Matrix44f & aTransformation)
	{
		physx::PxTransform transformation;
		transformation.p = { aTransformation.m41, aTransformation.m42 ,aTransformation.m43 };
		transformation.q = MatrixToQuat(aTransformation);
		myPxActor->setGlobalPose(transformation);
	}

	CU::Matrix44f  CPhysicsActor::GetTransformation()
	{
		physx::PxTransform globalPose = myPxActor->getGlobalPose();
		CU::Matrix44f transformation = QuatToMatrix(globalPose.q);
		transformation.SetPosition({ globalPose.p.x, globalPose.p.y, globalPose.p.z });
		return transformation;
	}

	void CPhysicsActor::SetShape(CShape* aShape)
	{
		myPxActor->attachShape(*aShape->myShape);
		myShape = aShape;
	}

	Physics::IPhysicsCallback* CPhysicsActor::GetCallbackData()
	{
		return myCallback;
	}

	void CPhysicsActor::SetCallbackData(IPhysicsCallback* aCallbacker)
	{
		myCallback = aCallbacker;
		myPxActor->userData = aCallbacker;
	}
}
