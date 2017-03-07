#pragma once

namespace physx
{
	class PxScene;
	class PxRigidActor;
}

namespace Physics
{
	class CPhysicsScene
	{
	public:
		CPhysicsScene(physx::PxScene* aPxScene);
		~CPhysicsScene();

		void AddActor(physx::PxRigidActor* aActor);
		void RemoveActor(physx::PxRigidActor* aActor);
		void SetActorPose(physx::PxRigidActor* aActor, const CU::Matrix44f& aTransformation);

	private:


	private:
		physx::PxScene* myPxScene;

	};
}