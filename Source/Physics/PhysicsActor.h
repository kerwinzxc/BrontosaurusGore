#pragma once

namespace physx
{
	class PxRigidActor;
}

namespace Physics
{
	enum class EActorType
	{
		eStatic,
		eDynamic
	};

	class CPhysicsActor
	{
	friend class CShape;
	friend class CPhysicsScene;
	
	public:
		CPhysicsActor(physx::PxRigidActor* aActor, CShape* aShape);
		virtual ~CPhysicsActor();

		void SetIsTrigger(const bool aIsTrigger);
		void SetTransformation(const CU::Matrix44f& aTransformation);
		CU::Matrix44f GetTransformation();
		virtual const EActorType GetType() const = 0;

		virtual void SetShape(CShape* aShape);

	protected:
		CShape* myShape;
		inline physx::PxRigidActor* GetPxActor() { return myPxActor; };

	private:
		physx::PxRigidActor* myPxActor;
		bool myIsTrigger;
	};
}

