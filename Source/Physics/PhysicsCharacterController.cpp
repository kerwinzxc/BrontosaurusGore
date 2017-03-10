#include "stdafx.h"
#include "PhysicsCharacterController.h"

#include <characterkinematic/PxController.h>
#include <PxScene.h>
#include "PxQueryReport.h"


namespace Physics
{

	CPhysicsCharacterController::CPhysicsCharacterController(physx::PxController * aPxController, const SCharacterControllerDesc & aData)
	{
		myData = aData;
		myController = aPxController;
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
		controllerFilters.mFilterFlags = physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC;
		
		physx::PxVec3 displacement = { aDisplacement.x, aDisplacement.y , aDisplacement.z }; //TODO: om knas kanske här
		myController->move(displacement, myData.minMoveDistance, aDeltaTime.GetSeconds(), controllerFilters);
		SetGrounded();
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

	void CPhysicsCharacterController::SetGrounded()
	{
		physx::PxRaycastBuffer hit;
		physx::PxExtendedVec3 footPosition = myController->getFootPosition();
		physx::PxVec3 origin = physx::toVec3(footPosition);

		physx::PxVec3 dir = -myController->getUpDirection();
		physx::PxReal dist = 0.01f;
		bool result = myController->getScene()->raycast(origin, dir, dist, hit);
		if (result == true)
		{	
			if (hit.block.distance <= myController->getContactOffset() + 0.05f) // small ypsilon for funz
			{
				myIsGrounded = true;
				return;
			}
			
		}
		myIsGrounded = false;
	}
}
