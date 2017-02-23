#include "stdafx.h"
#include "PhysXManager.h"
#include "PhysXAllocator.h"
#include "PhysXErrorHander.h"

#include <PxPhysicsAPI.h>
//#include <PxScene.h>
#include <PxRigidActor.h>

#include <foundation/PxFoundation.h>
#include <pvd/PxPvd.h>

//#include <extensions/PxDefaultErrorCallback.h>
//#include <extensions/PxDefaultAllocator.h>


#define PVD_HOST_IP "127.0.0.1"		// IP of the PC which is running PVD
#define PVD_PORT 5425				// TCP port to connect to, where PVD is listening
#define PVD_TIMEOUT 100				// timeout in milliseconds to wait for PVD to respond, consoles and remote PCs need a higher timeout.

// I don't know if we need theese?
#define _STATIC_CPPLIB
#define _DISABLE_DEPRECATE_STATIC_CPPLIB  // disable warnings about defining _STATIC_CPPLIB


#ifndef _RETAIL_BUILD
#define PX_ERRORCHECK(r, m) if (r == false) myPhysXErrorHandlerCallback->reportError(physx::PxErrorCode::eABORT, m, __FILE__, __LINE__);
#else
#define PX_ERRORCHECK(r, m) r; m;
#endif



namespace
{
	physx::PxQuat MatrixToQuat(const CU::Matrix44f& aTransformation)
	{
		// Output quaternion
		float w, x, y, z;
		// Determine which of w,x,y, or z has the largest absolute value
		float fourWSquaredMinus1 = aTransformation.m11 + aTransformation.m22 + aTransformation.m33;
		float fourXSquaredMinus1 = aTransformation.m11 - aTransformation.m22 - aTransformation.m33;
		float fourYSquaredMinus1 = aTransformation.m22 - aTransformation.m11 - aTransformation.m33;
		float fourZSquaredMinus1 = aTransformation.m33 - aTransformation.m11 - aTransformation.m22;

		int biggestIndex = 0;
		float fourBiggestSquaredMinus1 = fourWSquaredMinus1;

		if (fourXSquaredMinus1 > fourBiggestSquaredMinus1) {
			fourBiggestSquaredMinus1 = fourXSquaredMinus1;
			biggestIndex = 1;
		}
		if (fourYSquaredMinus1 > fourBiggestSquaredMinus1) {
			fourBiggestSquaredMinus1 = fourYSquaredMinus1;
			biggestIndex = 2;
		}
		if (fourZSquaredMinus1 > fourBiggestSquaredMinus1) {
			fourBiggestSquaredMinus1 = fourZSquaredMinus1;
			biggestIndex = 3;
		}
		// Per form square root and division
		float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
		float mult = 0.25f / biggestVal;

		// Apply table to compute quaternion values
		switch (biggestIndex) 
		{
		case 0:
			w = biggestVal;
			x = (aTransformation.m23 - aTransformation.m32) * mult;
			y = (aTransformation.m31 - aTransformation.m13) * mult;
			z = (aTransformation.m12 - aTransformation.m21) * mult;
			break;
		case 1:
			x = biggestVal;
			w = (aTransformation.m23 - aTransformation.m32) * mult;
			y = (aTransformation.m12 + aTransformation.m21) * mult;
			z = (aTransformation.m31 + aTransformation.m13) * mult;
			break;
		case 2:
			y = biggestVal;
			w = (aTransformation.m31 - aTransformation.m13) * mult;
			x = (aTransformation.m12 + aTransformation.m21) * mult;
			z = (aTransformation.m23 + aTransformation.m32) * mult;
			break;
		case 3:
			z = biggestVal;
			w = (aTransformation.m12 - aTransformation.m21) * mult;
			x = (aTransformation.m31 + aTransformation.m13) * mult;
			y = (aTransformation.m23 + aTransformation.m32) * mult;
			break;
		}

		return physx::PxQuat(x, y, z, w);
	}
}


Physics::CPhysXManager* Physics::CPhysXManager::ourInstance = nullptr;

namespace Physics
{
	using namespace physx;

	CPhysXManager::CPhysXManager()
		: myPhysics(nullptr)
		, myFoundation(nullptr)
		, myPvd(nullptr)
		, myDispatcher(nullptr)
	{
		PHYSICS_LOG("Initializing PhysX\n");
		myPhysXAllocatorCallback = new CPhysXAllocator();
		myPhysXErrorHandlerCallback = new CPhysXErrorHander();

		CPhysXAllocator& AllocatorCallback = *myPhysXAllocatorCallback;
		CPhysXErrorHander& ErrorCallback = *myPhysXErrorHandlerCallback;


		PHYSICS_LOG("Creating Foundation\n");

		myFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, AllocatorCallback, ErrorCallback);
		PX_ERRORCHECK(myFoundation != nullptr, "PxCreateFoundation failed!\n");

		bool recordMemoryAllocations = false;
#ifndef _RETAIL_BUILD
	#ifdef _DEBUG
		recordMemoryAllocations = true;
	#endif

		myPvd = PxCreatePvd(*myFoundation);
		PX_ERRORCHECK(myPvd != nullptr, "PxCreatePvd failed!\n");
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST_IP, PVD_PORT, PVD_TIMEOUT);
		if (myPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL))
		{
			PHYSICS_LOG("Successfully connected to PVD...\n");
		}
		else
		{
			PHYSICS_LOG("Failed to connect to PVD...\n");
		}

#endif

		myPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *myFoundation, physx::PxTolerancesScale(), recordMemoryAllocations, myPvd);
		PX_ERRORCHECK(myPhysics != nullptr, "PxCreatePhysics failed!\n");

		myScene = CreateScene();

	}
	CPhysXManager::~CPhysXManager()
	{
		if (myPvd)
		{
			myPvd->disconnect();
			SAFE_RELEASE(myPvd);
		}
		SAFE_RELEASE(myScene);
		SAFE_RELEASE(myPhysics);
		SAFE_RELEASE(myFoundation);
	}



	void CPhysXManager::Create()
	{
		ourInstance = new CPhysXManager();
	}

	void CPhysXManager::Destroy()
	{
		delete ourInstance;
		ourInstance = nullptr;
	}

	CPhysXManager& CPhysXManager::GetInstance()
	{
		return *ourInstance;
	}

	// Update in 60 - FPS
	void CPhysXManager::Update(const float dt)
	{
		myScene->simulate(1.0f / 60.0f);
		myScene->fetchResults(true);
	}

	PxRigidActor* CPhysXManager::CreateStaticCollider(PxGeometry& aGeometry, PxMaterial& aMaterial)
	{
		PxShape* shape = myPhysics->createShape(aGeometry, aMaterial);
		PxTransform transform(0, 0, 0);
		PxRigidStatic* rigidStatic = myPhysics->createRigidStatic(transform);
		rigidStatic->attachShape(*shape);
		shape->release();
		myScene->addActor(*rigidStatic);
		return rigidStatic;
	}

	PxScene* CPhysXManager::CreateScene()
	{
		PxScene* pxScene = nullptr;
		PxSceneDesc sceneDesc(myPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.82f, 0.0f);
		
		myDispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = myDispatcher;
		//maybe GPU Dispatcher
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		pxScene = myPhysics->createScene(sceneDesc);

#ifndef _RETAIL_BUILD
		PxPvdSceneClient* pvdClient = pxScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::Enum::eTRANSMIT_CONTACTS,		true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::Enum::eTRANSMIT_SCENEQUERIES,	true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::Enum::eTRANSMIT_CONSTRAINTS,		true);
		}
#endif
		/*myDefaultMaterial = myPhysics->createMaterial(0.5f, 0.5f, 0.6f);

		PxRigidStatic* groundPlane = PxCreatePlane(*myPhysics, PxPlane(0, 1, 0, 0), *myDefaultMaterial);
		pxScene->addActor(*groundPlane);

		PxShape* shape = myPhysics->createShape(PxBoxGeometry(2.0f, 2.0f, 2.0f), *myDefaultMaterial);
		for (PxU32 i = 0; i < 10; i++)
		{
			for (PxU32 j = 0; j < 10 - i; j++)
			{
				PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(10 - i), PxReal(i * 2 + 1), 0) * 2.0f);
				PxRigidDynamic* body = myPhysics->createRigidDynamic(PxTransform(PxVec3(0, 50, 0)).transform(localTm));
				body->attachShape(*shape);
				PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
				pxScene->addActor(*body);
			}
		}
		shape->release();*/


		return pxScene;
	}



	PxMaterial* CPhysXManager::CreateMaterial(const float aStaticFriction, const float aDynamicFriction, const float aRestitution)
	{
		return myPhysics->createMaterial(aDynamicFriction, aStaticFriction, aRestitution);
	}

	void CPhysXManager::SetGlobalPose(PxRigidActor* aActor, const CU::Matrix44f& aTransformation)
	{
		PxTransform transformation;
		transformation.p = { aTransformation.m41, aTransformation.m42 ,aTransformation.m43 };
		transformation.q = MatrixToQuat(aTransformation);

		aActor->setGlobalPose(transformation);
	}

	PxGeometry* CPhysXManager::CreateBoxGeometry(const CU::Vector3f& aSize)
	{
		PxVec3 size = { abs(aSize.x), abs(aSize.y), abs(aSize.z) };
		size *= 0.5f;
		return new PxBoxGeometry(size);
	}
}