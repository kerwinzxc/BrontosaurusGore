#pragma once

namespace physx
{
	class PxFoundation;
	class PxPhysics;
	class PxPvd;
	class PxScene;
	class PxDefaultCpuDispatcher;
	class PxMaterial;
	class PxRigidActor;
	class PxGeometry;
	class PxRigidStatic;
	class PxVec3;
}

namespace Physics
{

	class CPhysXAllocator;
	class CPhysXErrorHander;

	class CPhysXManager
	{
	public:

		static void Create();
		static void Destroy();

		static CPhysXManager& GetInstance();

		void Update(const float dt);

		//mebe add layers?
		physx::PxRigidActor* CreateStaticCollider(physx::PxGeometry& aGeometry, physx::PxMaterial& aMaterial);

		physx::PxGeometry* CreateBoxGeometry(const CU::Vector3f& aSize);
		physx::PxMaterial* CreateMaterial(const float aStaticFriction, const float aDynamicFriction, const float aRestitution);

		void SetGlobalPose(physx::PxRigidActor* aActor, const CU::Matrix44f& aTransformation);

	private:
		CPhysXManager();
		~CPhysXManager();
		
		physx::PxScene* CreateScene();
	private:
		static CPhysXManager * ourInstance;
	
		physx::PxScene* myScene;


		physx::PxFoundation* myFoundation;
		physx::PxPhysics* myPhysics;
		physx::PxPvd* myPvd;
		physx::PxDefaultCpuDispatcher* myDispatcher;

		CPhysXAllocator * myPhysXAllocatorCallback;
		CPhysXErrorHander * myPhysXErrorHandlerCallback;
	};
}
