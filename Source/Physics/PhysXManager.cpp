#include "stdafx.h"
#include "PhysXManager.h"
#include "PhysXAllocator.h"
#include "PhysXErrorHander.h"

#include <PxPhysicsAPI.h>
#include <foundation/PxFoundation.h>
#include <pvd/PxPvd.h>

#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h>


#define PVD_HOST_IP "127.0.0.1"		// IP of the PC which is running PVD
#define PVD_PORT 5425				// TCP port to connect to, where PVD is listening
#define PVD_TIMEOUT 100				// timeout in milliseconds to wait for PVD to respond, consoles and remote PCs need a higher timeout.




#ifndef _RETAIL_BUILD
#define PX_ERRORCHECK(r, m) if (r == false) gPhysXErrorHandlerCallback.reportError(physx::PxErrorCode::eABORT, m, __FILE__, __LINE__);
#else
#define PX_ERRORCHECK(r, m) r; m;
#endif


static CPhysXAllocator gPhysXAllocatorCallback;
static CPhysXErrorHander gPhysXErrorHandlerCallback;

// WE WANT THIS THO // mebe pvd(PxPvdSceneClient)  
//static physx::PxProfileZoneManager* gProfileZoneManager;

CPhysXManager::CPhysXManager()
	: myPhysics(nullptr)
	, myFoundation(nullptr)
	, myPvd(nullptr)
{
	PHYSICS_LOG("Initializing PhysX\n");
	PHYSICS_LOG("Creating Foundation\n");

	myFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gPhysXAllocatorCallback, gPhysXErrorHandlerCallback);
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


}


CPhysXManager::~CPhysXManager()
{
	if (myPvd)
	{
		myPvd->disconnect();
		SAFE_RELEASE(myPvd);
	}
	SAFE_RELEASE(myPhysics);
	SAFE_RELEASE(myFoundation);
}
