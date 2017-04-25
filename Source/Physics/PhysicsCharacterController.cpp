#include "stdafx.h"
#include "PhysicsCharacterController.h"

#include <characterkinematic/PxController.h>
#include <PxScene.h>
#include "PxQueryReport.h"
#include "PxRigidDynamic.h"

namespace Physics
{
	CPhysicsCharacterController::CPhysicsCharacterController(physx::PxController * aPxController, const SCharacterControllerDesc & aData)
	{
		myCollisionFlags = 0;
		myIsGrounded = true;
		myParentId = 0;
		myData = aData;
		myController = aPxController;
		myController->getActor()->userData = this;
	}

	CPhysicsCharacterController::~CPhysicsCharacterController()
	{
		//myController->release();
		if (myController && myController->getActor())
		{
			myController->getActor()->userData = nullptr;
		}
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
		filterData.word2 = myParentId;
		controllerFilters.mFilterData = &filterData;
		controllerFilters.mFilterFlags = physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC;

		physx::PxVec3 displacement = { aDisplacement.x, aDisplacement.y - 1000.0f * aDeltaTime.GetSeconds(), aDisplacement.z }; //TODO: om knas kanske här //Ja det ligger något i det. MvH Alex
		physx::PxControllerCollisionFlags flags = myController->move(displacement, myData.minMoveDistance, aDeltaTime.GetSeconds(), controllerFilters);
		SetCollisionFlags((uint8_t)flags);
	}

	void CPhysicsCharacterController::SetIsActive(const bool aIsActive)
	{
		myController->getActor()->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, !aIsActive);
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

	const Physics::EControllerConstraintsFlags CPhysicsCharacterController::GetConstraints()
	{
		return myCollisionFlags;
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
	}

	void CPhysicsCharacterController::SetParentId(ComponentId aNId)
	{
		myParentId = aNId;
	}

	void CPhysicsCharacterController::SetCollisionFlags(const uint8_t flags)
	{
		myCollisionFlags = 0;
		if (flags & physx::PxControllerCollisionFlag::eCOLLISION_SIDES)
		{
			myCollisionFlags |= EControllerConstraintsFlag::eCOLLISION_SIDES;
		}
		if (flags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN)
		{
			myCollisionFlags |= EControllerConstraintsFlag::eCOLLISION_DOWN;
		}
		if (flags & physx::PxControllerCollisionFlag::eCOLLISION_UP)
		{
			myCollisionFlags |= EControllerConstraintsFlag::eCOLLISION_UP;
		}
	}
}
