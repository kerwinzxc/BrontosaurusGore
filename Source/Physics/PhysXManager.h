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
		void InitApex();
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

		class DummyRenderResourceManager : public nvidia::UserRenderResourceManager
		{
		public:
			virtual nvidia::UserRenderVertexBuffer*   createVertexBuffer(const nvidia::UserRenderVertexBufferDesc&) { return NULL; }
			virtual void                        releaseVertexBuffer(nvidia::UserRenderVertexBuffer&) {}

			virtual nvidia::UserRenderIndexBuffer*    createIndexBuffer(const nvidia::UserRenderIndexBufferDesc&) { return NULL; }
			virtual void                        releaseIndexBuffer(nvidia::UserRenderIndexBuffer&) {}

			virtual nvidia::UserRenderBoneBuffer*     createBoneBuffer(const nvidia::UserRenderBoneBufferDesc&) { return NULL; }
			virtual void                        releaseBoneBuffer(nvidia::UserRenderBoneBuffer&) {}

			virtual nvidia::UserRenderInstanceBuffer* createInstanceBuffer(const nvidia::UserRenderInstanceBufferDesc&) { return NULL; }
			virtual void                        releaseInstanceBuffer(nvidia::UserRenderInstanceBuffer&) {}

			virtual nvidia::UserRenderSpriteBuffer*   createSpriteBuffer(const nvidia::UserRenderSpriteBufferDesc&) { return NULL; }
			virtual void                        releaseSpriteBuffer(nvidia::UserRenderSpriteBuffer&) {}

			virtual nvidia::UserRenderSurfaceBuffer*  createSurfaceBuffer(const nvidia::UserRenderSurfaceBufferDesc&) { return NULL; }
			virtual void                        releaseSurfaceBuffer(nvidia::UserRenderSurfaceBuffer&) {}

			virtual nvidia::UserRenderResource*       createResource(const nvidia::UserRenderResourceDesc&) { return NULL; }
			virtual void                        releaseResource(nvidia::UserRenderResource&) {}

			virtual uint32_t                       getMaxBonesForMaterial(void*) { return 0; }

			virtual bool getSpriteLayoutData(uint32_t, uint32_t, nvidia::UserRenderSpriteBufferDesc*) { return false; }

			virtual bool getInstanceLayoutData(uint32_t, uint32_t, nvidia::UserRenderInstanceBufferDesc*) { return false; }
		};

		::Physics::CPhysXManager::DummyRenderResourceManager* myRenderResourceManager;
	};
}
