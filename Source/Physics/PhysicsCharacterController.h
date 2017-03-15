#pragma once
#include "PhysicsCallbackActor.h"

namespace physx
{
	class PxController;
}


namespace Physics
{
	struct SCharacterControllerDesc
	{
		CU::Vector3f center;
		float slopeLimit = 45.f;
		float stepOffset = 0.3f;
		float skinWidth = 0.08f;
		float minMoveDistance = 0.001f;
		float radius = 0.5f;
		float height = 1.0f;
	};

	using EControllerConstraintsFlags = char;
	enum EControllerConstraintsFlag : EControllerConstraintsFlags
	{
		eCOLLISION_SIDES	= (1 << 0),	// Character is colliding to the sides.
		eCOLLISION_UP		= (1 << 1),	// Character has collision above.
		eCOLLISION_DOWN		= (1 << 2)	// Character has collision below.
	};

	class CPhysicsCharacterController : public CPhysicsCallbackActor
	{
	public:
		CPhysicsCharacterController(physx::PxController* aPxController, const SCharacterControllerDesc& aData);
		~CPhysicsCharacterController();

		void Move(const CU::Vector3f& aDisplacement, const CU::Time aDeltaTime);

		void Resize(const float aHeight);		
		CU::Vector3f GetFootPosition();

		const EControllerConstraintsFlags GetConstraints();

		//This one "teleports" the player, no colliding yo
		void SetPosition(const CU::Vector3f& aPosition);
		CU::Vector3f GetPosition();

		IPhysicsCallback* GetCallbackData() override;
		void SetCallbackData(IPhysicsCallback* aCallbacker) override;

		inline const EActorType GetType() const override { return EActorType::eController; };

	private:
		void SetCollisionFlags(const char& flags);

	private:

		SCharacterControllerDesc myData;
		physx::PxController* myController;
		EControllerConstraintsFlags myCollisionFlags;
		bool myIsGrounded;
	};
}