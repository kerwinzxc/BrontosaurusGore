#include "stdafx.h"
#include "PhysicsCharacterController.h"

#include <characterkinematic/PxController.h>
#include <PxScene.h>
#include "PxQueryReport.h"
#include "CollisionLayers.h"
#include "PxRigidDynamic.h"

namespace Physics
{
	CPhysicsCharacterController::CPhysicsCharacterController(physx::PxController * aPxController, const SCharacterControllerDesc & aData)
	{
		myData = aData;
		myController = aPxController;
		myController->getActor()->userData = nullptr;
	}

	CPhysicsCharacterController::~CPhysicsCharacterController()
	{
		//myController->release();
		myController = nullptr;
	}

	void CPhysicsCharacterController::Move(const CU::Vector3f& aDisplacement, const CU::Time aDeltaTime)
	{
		float dt = aDeltaTime.GetMilliseconds();
		
		physx::PxControllerFilters controllerFilters;
		physx::PxFilterData filterData;
		physx::PxU32 colGroups = 0xffffffff; //everything (-1)
		colGroups &= ~(colGroups); //Turn off bits 1>0
		filterData.word0 = 1; //I am now everything but aFilterMask
		filterData.word1 = colGroups; //I don't Collide with anything
							  //controllerFilters.mFilterCallback = myCollisionHandler;
		controllerFilters.mFilterData = &filterData;
		controllerFilters.mFilterFlags = physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC;

		physx::PxVec3 displacement = { aDisplacement.x, aDisplacement.y , aDisplacement.z }; //TODO: om knas kanske här
		myIsGrounded = myController->move(displacement, myData.minMoveDistance, aDeltaTime.GetSeconds(), controllerFilters) & physx::PxControllerCollisionFlag::eCOLLISION_DOWN;
	}

	void CPhysicsCharacterController::Resize(const float aHeight)
	{
		myController->resize(aHeight);
	}

	CU::Vector3f CPhysicsCharacterController::GetFootPosition()
	{
		physx::PxExtendedVec3 position = myController->getFootPosition();
		return CU::Vector3f(position.x, position.y, position.z);
	}

	bool CPhysicsCharacterController::GetIsGrounded()
	{
		return myIsGrounded;
	}

	void CPhysicsCharacterController::SetPosition(const CU::Vector3f& aPosition)
	{
		physx::PxExtendedVec3 position = { aPosition.x, aPosition.y, aPosition.z };
		myController->setPosition(position);
	}

	CU::Vector3f CPhysicsCharacterController::GetPosition()
	{
		physx::PxExtendedVec3 position = myController->getPosition();
		return CU::Vector3f(position.x, position.y, position.z);
	}

	Physics::IPhysicsCallback* CPhysicsCharacterController::GetCallbackData()
	{
		return myCallback;
	}

	void CPhysicsCharacterController::SetCallbackData(IPhysicsCallback* aCallbacker)
	{
		myCallback = aCallbacker;
		myController->getActor()->userData = aCallbacker;
	}

}
