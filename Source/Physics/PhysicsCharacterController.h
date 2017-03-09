#pragma once

namespace physx
{
	class PxController;
}


namespace Physics
{
	struct SCharacterControllerDesc
	{
		float slopeLimit = 45.f;
		float stepOffset = 0.3f;
		//float skinWidth = 0.08f;
		float minMoveDistance = 0.001f;
		float radius = 0.5f;
		float height = 1.0f;
	};

	class CPhysicsCharacterController
	{
	public:
		CPhysicsCharacterController(physx::PxController* aPxController, const SCharacterControllerDesc& aData);
		~CPhysicsCharacterController();

		void Move(const CU::Vector3f& aDisplacement, const CU::Time aDeltaTime);

		void Resize(const float aHeight);		
		CU::Vector3f GetFootPosition();

		bool GetIsGrounded();

		//This one "teleports" the player, no colliding yo
		void SetPosition(const CU::Vector3f& aPosition);
		CU::Vector3f GetPosition();
	private:
		void SetGrounded();

	private:
		SCharacterControllerDesc myData;
		physx::PxController* myController;

		bool myIsGrounded;
	};
}