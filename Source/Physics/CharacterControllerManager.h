#pragma once
#include "../Components/Component.h"

namespace physx
{
	class PxControllerManager;
}

namespace Physics
{
	class CPhysics;
	class CPhysicsScene;
	class CPhysicsCharacterController;
	struct SCharacterControllerDesc;


	class CCharacterControllerManager
	{
	public:
		CCharacterControllerManager(CPhysicsScene* aPhysicsScene, CPhysics* aPhysics);
		~CCharacterControllerManager();

		CPhysicsCharacterController* CreateCharacterController(const SCharacterControllerDesc& aDesc, ComponentId anId);

	private:
		physx::PxControllerManager* myControllerManager;
		CPhysicsScene* myScene;
		CPhysics* myPhysics;
	};
}