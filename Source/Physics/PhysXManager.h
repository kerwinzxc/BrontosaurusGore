#pragma once
#include "Include/apex_1.4/UserRenderResourceManager.h"

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
	class PxCooking;
}

namespace Physics
{
	class CApex;
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
		physx::PxRigidActor* CreateDynamicCollider(physx::PxGeometry& aGeometry, physx::PxMaterial& aMaterial);


		physx::PxGeometry* CreateBoxGeometry(const CU::Vector3f& aSize);
		physx::PxGeometry* CreateMeshGeometry(const char* aPath);
		physx::PxGeometry* CreateSphereGeometry(const float aRadius);

		physx::PxMaterial* CreateMaterial(const float aStaticFriction, const float aDynamicFriction, const float aRestitution);

		void SetGlobalPose(physx::PxRigidActor* aActor, const CU::Matrix44f& aTransformation);
		void AddActor(physx::PxRigidActor& aActor);
		void RemoveActor(physx::PxRigidActor& aActor);
		void SetPvdCameraTransform(const CU::Matrix44f& aTransformation);


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
		physx::PxCooking* myCooking;

		CPhysXAllocator * myPhysXAllocatorCallback;
		CPhysXErrorHander * myPhysXErrorHandlerCallback;
		CApex* myApex;
	};
}
