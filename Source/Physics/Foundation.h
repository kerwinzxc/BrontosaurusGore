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
	class PxCooking;
}

namespace Physics
{
	class CPhysics;
	class CPhysXAllocator;
	class CPhysXErrorHander;

	class CFoundation
	{
	public:
		static void Create();
		static void Destroy();
		static CFoundation* GetInstance();

		CPhysics* CreatePhysics();

	private:

		void InitializeFoundation();
		void InitializePVD();
		void InitializeCooking();

		CFoundation();
		~CFoundation();


	private:
		static CFoundation* ourInstance;

		CU::GrowingArray<CPhysics*, char> myPhysics;
		physx::PxFoundation* myFoundation;
		physx::PxPvd* myPvd;
		physx::PxCooking* myCooking;

		CPhysXAllocator* myPhysXAllocatorCallback;
		CPhysXErrorHander* myPhysXErrorHandlerCallback;

	};
}

