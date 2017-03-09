#pragma once

namespace physx
{
	class PxScene;
	class PxRigidActor;
}

namespace Physics
{
	class CPhysicsActor;
	class CPhysicsScene
	{
	public:
		CPhysicsScene(physx::PxScene* aPxScene);
		~CPhysicsScene();

		// dt in seconds
		bool Simulate(const CU::Time aDeltaTime);

		void AddActor(CPhysicsActor* aActor);
		void RemoveActor(CPhysicsActor* aActor);
		inline physx::PxScene* GetScene() { return myPxScene; }
	private:
		physx::PxScene* myPxScene;



		float mySimulationTimer;

		static constexpr float ourSimulationFrequensy = 1.0f / 60.f;
	};
}