#include "stdafx.h"
#include "CharacterControllerManager.h"

#include <characterkinematic/PxControllerManager.h>
#include "characterkinematic/PxCapsuleController.h"
#include "characterkinematic/PxController.h"

#include "Physics.h"
#include "PhysicsScene.h"
#include "PhysicsCharacterController.h"
#include "SimulationEventCallback.h"
#include <PxScene.h>
#include "Components/Component.h"
#include <PxRigidDynamic.h>

namespace Physics
{
	using namespace physx;

	CCharacterControllerManager::CCharacterControllerManager(CPhysicsScene* aPhysicsScene, CPhysics* aPhysics)
	{
		myPhysics = aPhysics;
		myScene = aPhysicsScene;
		myControllerManager = PxCreateControllerManager(*aPhysicsScene->GetScene());
		myControllerManager->setOverlapRecoveryModule(true);
	}

	CCharacterControllerManager::~CCharacterControllerManager()
	{
		myScene = nullptr;
		myPhysics = nullptr;
		myControllerManager->purgeControllers();
	}

	Physics::CPhysicsCharacterController* CCharacterControllerManager::CreateCharacterController(const SCharacterControllerDesc& aDesc, ComponentId anId)
	{
		PxCapsuleControllerDesc desc;
		desc.slopeLimit = aDesc.slopeLimit;
		desc.stepOffset = aDesc.stepOffset;
		desc.radius = aDesc.radius;
		desc.height = aDesc.halfHeight;
		desc.contactOffset = aDesc.skinWidth;
		desc.reportCallback = myPhysics->myEventCallback;
		desc.climbingMode = PxCapsuleClimbingMode::eEASY;
		desc.material = myPhysics->CreateMaterial(SMaterialData());
		PxController* controller = myControllerManager->createController(desc);
		
		return new CPhysicsCharacterController(controller, aDesc);
	}
}
