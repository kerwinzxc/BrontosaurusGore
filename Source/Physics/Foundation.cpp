#include "stdafx.h"
#include "Foundation.h"
#include "Physics.h"

#include "PhysXAllocator.h"
#include "PhysXErrorHander.h"

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <foundation/PxFoundation.h>
#include <pvd/PxPvd.h>


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




namespace Physics
{
	CFoundation* CFoundation::ourInstance = nullptr;

	void CFoundation::Create()
	{
		if (ourInstance != nullptr)
		{
			DL_ASSERT("CFoundation already created.");
		}

		ourInstance = new CFoundation();
	}

	void CFoundation::Destroy()
	{
		if (ourInstance != nullptr)
		{
			delete ourInstance;
			ourInstance = nullptr;
		}
	}

	CFoundation* CFoundation::GetInstance()
	{
		return ourInstance;
	}

	CPhysics* CFoundation::CreatePhysics()
	{
		if (myPhysics)
		{
			DL_ASSERT("Physics already created, cannot have two instances of physics.");
		}


		bool recordMemoryAllocations = false;
#ifdef _DEBUG
		recordMemoryAllocations = true;
#endif

		physx::PxPhysics* physics = PxCreatePhysics(PX_PHYSICS_VERSION, *myFoundation, physx::PxTolerancesScale(), recordMemoryAllocations, myPvd);
		PX_ERRORCHECK(physics != nullptr, "PxCreatePhysics failed!\n");


		myPhysics = new CPhysics(physics);
		return myPhysics;
	}

	physx::PxCooking& CFoundation::GetCooking()
	{
		return *myCooking;
	}

	void CFoundation::InitializeFoundation()
	{
		PHYSICS_LOG("Initializing PhysX\n");
		myPhysXAllocatorCallback = new CPhysXAllocator();
		myPhysXErrorHandlerCallback = new CPhysXErrorHander();

		CPhysXAllocator& AllocatorCallback = *myPhysXAllocatorCallback;
		CPhysXErrorHander& ErrorCallback = *myPhysXErrorHandlerCallback;


		PHYSICS_LOG("Creating Foundation\n");

		myFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, AllocatorCallback, ErrorCallback);
		PX_ERRORCHECK(myFoundation != nullptr, "PxCreateFoundation failed!\n");


	}

	void CFoundation::InitializePVD()
	{


#ifndef _RETAIL_BUILD
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
	}

	void CFoundation::InitializeCooking()
	{
		physx::PxCookingParams cookingParams = physx::PxCookingParams(physx::PxTolerancesScale());
		myCooking = PxCreateCooking(PX_PHYSICS_VERSION, *myFoundation, cookingParams);
		PX_ERRORCHECK(myCooking != nullptr, "PxCreateCooking failed!\n");
	}

	CFoundation::CFoundation()
	{
		myPhysics = nullptr;
		myFoundation = nullptr;
		myPvd = nullptr;
		myCooking = nullptr;
		myPhysXAllocatorCallback = nullptr;
		myPhysXErrorHandlerCallback = nullptr;

		InitializeFoundation();
		InitializePVD();
		InitializeCooking();
	}

	CFoundation::~CFoundation()
	{
		SAFE_DELETE(myPhysics);

		myPvd->disconnect();
		myPvd->getTransport()->release();
		myPvd->release();
		myFoundation->release();				// Om fel, kanske här
		
		SAFE_DELETE(myPhysXAllocatorCallback);
		SAFE_DELETE(myPhysXErrorHandlerCallback);
	}
}
