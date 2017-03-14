#pragma once

namespace physx
{
	class PxScene;
	class PxRigidActor;
}

namespace Physics
{
	class CPhysicsActor;
	class IPhysicsCallback;

	struct SRaycastHitData
	{
		bool hit = false;
		float distance = 0.0f;
		CU::Vector3f position;
		CU::Vector3f normal;
		unsigned int faceIndex;
		IPhysicsCallback* actor;
	};

	class CPhysicsScene
	{
	public:
		CPhysicsScene(physx::PxScene* aPxScene);
		~CPhysicsScene();

		// dt in seconds
		bool Simulate(const CU::Time aDeltaTime);

		SRaycastHitData Raycast(const CU::Vector3f& aOrigin, const CU::Vector3f& aDirection, float aRayLength);

		void AddActor(CPhysicsActor* aActor);
		void RemoveActor(CPhysicsActor* aActor);
		inline physx::PxScene* GetScene() { return myPxScene; }
	private:
		physx::PxScene* myPxScene;

		float mySimulationTimer;
		static constexpr float ourSimulationFrequensy = 1.0f / 60.f;
	};
}