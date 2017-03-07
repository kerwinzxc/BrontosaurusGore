#include "stdafx.h"
#include <PxPhysics.h>
#include <PxScene.h>
#include <PxSceneDesc.h>
#include <PxPhysicsAPI.h>

#include "Physics.h"
#include "Foundation.h"
#include "PhysicsScene.h"
#include "Shape.h"


namespace Physics
{
	const physx::PxReal defMatStaticFriction = 0.5f;
	const physx::PxReal defMatDynamicFriction = 0.5f;
	const physx::PxReal defMatRestitution = 0.5f;

	using namespace physx;

	CPhysics::CPhysics(physx::PxPhysics* aPxPhysics)
	{
		myScenes.Init(2);
		myPxPhysics = aPxPhysics;
		myDefaultMaterial = myPxPhysics->createMaterial(defMatStaticFriction, defMatDynamicFriction, defMatRestitution);

		CreateNewScene();
	}

	CPhysics::~CPhysics()
	{
		myScenes.DeleteAll();
		SAFE_RELEASE(myPxPhysics);
		SAFE_RELEASE(myDefaultMaterial);
	}

	CPhysicsScene* CPhysics::CreateNewScene()
	{
		PxScene* pxScene = nullptr;
		PxSceneDesc sceneDesc(myPxPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.82f, 0.0f);

		myDispatcher = PxDefaultCpuDispatcherCreate(0);
		sceneDesc.cpuDispatcher = myDispatcher;
		//maybe GPU Dispatcher
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		pxScene = myPxPhysics->createScene(sceneDesc);

#ifndef _RETAIL_BUILD
		PxPvdSceneClient* pvdClient = pxScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::Enum::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::Enum::eTRANSMIT_SCENEQUERIES, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::Enum::eTRANSMIT_CONSTRAINTS, true);
		}
#endif

		myScenes.Add(new CPhysicsScene(pxScene));
		return myScenes.GetLast();
	}
	CShape* CPhysics::CreateBoxShape(const CU::Vector3f& aSize, const SMaterialData & aMaterialData)
	{
		physx::PxVec3 size = { aSize.x, aSize.y, aSize.z };
		PxBoxGeometry* geometry = new PxBoxGeometry(size);
		return CreateShape(*geometry, aMaterialData);
	}
	CShape* CPhysics::CreateSphereShape(const float aRadius, const SMaterialData & aMaterialData)
	{
		PxSphereGeometry* geometry = new PxSphereGeometry(); geometry->radius = aRadius;
		return CreateShape(*geometry, aMaterialData);
	}
	CShape* CPhysics::CreateMeshShape(const char * aPath, const SMaterialData & aMaterialData)
	{
		PxTriangleMeshDesc desc;
		return nullptr;
	}

	CShape* CPhysics::CreateShape(physx::PxGeometry& aGeometry, const SMaterialData & aMaterialData)
	{
		PxMaterial* material = CreateMaterial(aMaterialData);
		PxShape* shape = myPxPhysics->createShape(aGeometry, *material);
		//release material?
		return new CShape(shape);
	}


	physx::PxMaterial* CPhysics::CreateMaterial(const SMaterialData & aMaterialData)
	{
		if (aMaterialData.aDynamicFriction == defMatDynamicFriction &&
			aMaterialData.aStaticFriction == defMatStaticFriction &&
			aMaterialData.aRestitution == defMatRestitution)
		{
			return myDefaultMaterial;
		}
		return myPxPhysics->createMaterial(aMaterialData.aStaticFriction, aMaterialData.aDynamicFriction, aMaterialData.aRestitution);
	}
}