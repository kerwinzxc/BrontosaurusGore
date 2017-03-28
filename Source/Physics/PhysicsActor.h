#pragma once
#include "IPhysicsCallback.h"
#include "PhysicsCallbackActor.h"

namespace physx
{
	class PxRigidActor;
}

namespace Physics
{
	class CPhysicsActor : public CPhysicsCallbackActor
	{
		friend class CShape;
		friend class CPhysicsScene;

	public:
		CPhysicsActor(physx::PxRigidActor* aActor, CShape* aShape);
		virtual ~CPhysicsActor();

		void SetIsActive(const bool aIsActive);
		bool GetIsActive();
		void SetUseGravity(const bool aUseGravity);

		
		void SetIsTrigger(const bool aIsTrigger);
		void SetTransformation(const CU::Matrix44f& aTransformation);
		CU::Matrix44f GetTransformation();
		virtual void SetShape(CShape* aShape);

		IPhysicsCallback* GetCallbackData() override;
		void SetCallbackData(IPhysicsCallback* aCallbacker) override;
		
	protected:
		CShape* myShape;
		inline physx::PxRigidActor* GetPxActor() { return myPxActor; };

	private:
		physx::PxRigidActor* myPxActor;
		bool myIsTrigger;
	};
}

